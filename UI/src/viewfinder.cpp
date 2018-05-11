/**
 * @file    viewfinder.cpp
 *
 * @brief   implementation for Viewfinder class
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#include "gui.hpp"
#include "viewfinder.hpp"
#include "exceptions.hpp"
#include <cairomm/context.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <giomm/resource.h>
#include <glibmm/fileutils.h>

// PUBLIC METHODS {{{
Viewfinder::Viewfinder() {
  // open camera
  initialize_camera();
  if (!camera.isOpened()) {
    // Camera didn't successfully open.
#if defined RPI
    error(Exceptions::VF_OPEN_FAIL, "Opening Camera failed. Are you running this on a Raspberry Pi with the camera connected via the Ribbon Cable?");
#elif defined V4L2
    error(Exceptions::VF_OPEN_FAIL, "Opening Camera failed.");
#endif
  } else {
    // Waiting for camera to "stabalize"
    print("Sleeping for 3 seconds...");
    sleep(3);
    // Camera opened successfully
    //
    // call on_timeout once every FRAMERATE_INTERVAL (in ms)
    // this is the Gtk(TM) way to redraw a window on command
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &Viewfinder::on_timeout), FRAMERATE_INTERVAL);
  }

  current_mode = ViewfinderMode::STREAM;
  print("Setup finished.");
}

Viewfinder::~Viewfinder() {
  uninitialize_camera();
}

cv::Mat Viewfinder::get_frame(bool fresh) {
  print("Manually getting a frame");
  if (fresh) {
    cv::Mat frame;
    camera.grab();
    camera.retrieve(frame);
    return frame.clone();
  } else {
    // if there are no frames, throw exception
    /* TODO: Figure out good safety checks
    if (still_capture == NULL) 
      error(Exceptions::VF_EMPTY, 
            "Viewfinder getting frame from captures when none available");
    */
    // else
    return still_capture.clone();
  }
}

void Viewfinder::set_frame(cv::Mat frame) {
  print("Manually setting frame");
  still_capture = frame;
}

void Viewfinder::start_writer(std::string location) {
  print("Starting VideoWriter");
  print(location.c_str());
  video_location = location;
  video_record.open(location, 
                    CV_FOURCC('M','J','P','G'),
                    1000.0/FRAMERATE_INTERVAL,
                    cv::Size(camera.get(cv::CAP_PROP_FRAME_WIDTH),
                             camera.get(cv::CAP_PROP_FRAME_HEIGHT)));
  Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &Viewfinder::stop_writer), 10000);
}

bool Viewfinder::stop_writer() {
  print("Stopping VideoWriter");
  video_record.release();
  print("Creating VideoCapture to saved file");
  video_capture.open(video_location);
  return false;
}

void Viewfinder::initialize_camera() {
  print("Initializing Camera");
#if defined RPI
  if (!camera.open()) {
    // Camera didn't successfully open.
    error(Exceptions::VF_OPEN_FAIL, 
        "Opening Camera failed. Are you running this on a Raspberry Pi with the camera connected via the Ribbon Cable?");
#elif defined V4L2
  if (!camera.open(0)) {
    // Camera didn't successfully open.
    error(Exceptions::VF_OPEN_FAIL, "Opening Camera failed.");
#endif
  }
}

void Viewfinder::uninitialize_camera() {
  print("Uninitializing Camera");
  camera.release();
}
// }}}

// PRIVATE METHODS {{{
bool Viewfinder::on_timeout() {
  Glib::RefPtr<Gdk::Window> win = get_window();

  if (win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    // force redraw of the window
    win->invalidate_rect(r, false);
  }

  return true;
}

bool Viewfinder::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  // frame1 is the raw image frame from source
  // frame2 is for a color-corrected version
	cv::Mat frame1, frame2;

  // always grab, else there will be a lag in displayed image
  camera.grab();

  // SOURCE MANAGEMENT {{{
  // pick the source of the frame based on the mode
  // both STREAM and VIDEO_RECORD display current frames
  switch(current_mode) {
    case ViewfinderMode::STREAM:
    case ViewfinderMode::VIDEO_RECORD:
      if (!camera.isOpened()) {
        error(Exceptions::VF_OPEN_FAIL, 
              "set to STREAM, but camera not opened");
      }
      camera.retrieve(frame1);

      if (current_mode == ViewfinderMode::STREAM) break;
      // else keep going

      if (!video_record.isOpened()) {
        error(Exceptions::VF_WRITER_OPEN_FAIL, 
              "recording but no writer opened");
      }
      video_record.write(frame1);
      // set the last known frame to the current
      still_capture = frame1;
      break;
    case ViewfinderMode::CAPTURE:
      frame1 = still_capture;
      break;
    case ViewfinderMode::VIDEO_CAPTURE:
      if (!video_capture.isOpened()) {
        error(Exceptions::VF_CAPTURE_OPEN_FAIL, 
              "reading but no reader opened");
      }
      video_capture >> frame1;
      if (frame1.empty()) {
        // VideoCapture reached end of file
        video_capture.release();
        video_capture.open(video_location);
        return true;
      }
      break;
    default:
      error(Exceptions::VF_UNKOWN_MODE, "on_draw() default case");
      break;
  }
 
	if (frame1.empty()) {
    // no data in frame
    // maybe a a corrupt file or a bad stream
    error(Exceptions::FRAME_EMPTY, 
          "currently displaying frame is empty");
  }
  // }}}
 
  // IMAGE PROCESSING {{{
  // apply a threshold to the frame
	cv::cvtColor (frame1, frame2, CV_BGR2RGB);

  Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create_from_data(
												frame2.data, 
												Gdk::COLORSPACE_RGB, 
												false, 
												8, 
                        frame2.cols, 
                        frame2.rows, 
                        frame2.step);

  Gtk::Allocation allocation = get_allocation();
  const int frame_width = allocation.get_width();
  const int frame_height = allocation.get_height();
  const int img_width = buf->get_width();
  const int img_height = buf->get_height();

  // Draw the image in the top right corner, fixed aspect ratio to fit the window
  double scale = std::min(frame_width /(double) img_width,
                          frame_height /(double) img_height);
  int scaled_width = img_width*scale;
  int scaled_height = img_height*scale;
  Glib::RefPtr<Gdk::Pixbuf> buf_scaled = 
        buf->scale_simple(scaled_width, 
                          scaled_height, 
                          Gdk::INTERP_BILINEAR);
  Gdk::Cairo::set_source_pixbuf(cr, buf_scaled, 0, 0);
  cr->paint(); 
		 
  Viewfinder::draw_hud(cr, scaled_width, scaled_height);

  return true;
  // }}}
}

void Viewfinder::draw_hud(const Cairo::RefPtr<Cairo::Context>& cr,
                          int scaled_width, int scaled_height) {
  cr->set_line_width(4);
  cr->set_source_rgb(1.0, 1.0, 1.0);

  cr->save();
  cr->move_to(50,10);
  cr->line_to(10,10);
  cr->line_to(10,50);
  cr->stroke();

  cr->move_to(scaled_width - 50,10);
  cr->line_to(scaled_width - 10,10);
  cr->line_to(scaled_width - 10,50);
  cr->stroke();

  cr->move_to(50,scaled_height - 10);
  cr->line_to(10,scaled_height - 10);
  cr->line_to(10,scaled_height - 50);
  cr->stroke();

  cr->move_to(scaled_width - 50,scaled_height - 10);
  cr->line_to(scaled_width - 10,scaled_height - 10);
  cr->line_to(scaled_width - 10,scaled_height - 50);
  cr->stroke();
  cr->restore();

  std::stringstream ss;
  Glib::RefPtr<Pango::Layout> layout;
  switch (current_mode) {
    case ViewfinderMode::STREAM:
      ss << "LIVE";
      break;
    case ViewfinderMode::CAPTURE:
      ss << "CAP";
      break;
    case ViewfinderMode::VIDEO_CAPTURE:
      ss << "VID_CAP";
      break;
    case ViewfinderMode::VIDEO_RECORD:
      ss << "VID_CAP (REC)";
      break;
    default:
      ss << "...";
      break;
	}
  ss << " ";
  ss << hud_info;
  layout = create_pango_layout(ss.str());

  int text_width;
  int text_height;

  //get the text dimensions (it updates the variables -- by reference)
  layout->get_pixel_size(text_width, text_height);

  // Position the text in the top-left
  cr->move_to(20,20);

  layout->show_in_cairo_context(cr);
}
// }}}

