/**
 * @file    viewfinder.cpp
 *
 * @brief   implementation for Viewfinder class
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#include "gui.h"
#include "viewfinder.h"
#include <cairomm/context.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <giomm/resource.h>
#include <glibmm/fileutils.h>

Viewfinder::Viewfinder() {
  // open camera
  print("Opening Camera...");
#if defined RPI
  initialize_camera();
  if (!camera.isOpened()) {
    // Camera didn't successfully open.
    std::cerr << "Opening Camera failed." << std::endl; 
    std::cerr << "Are you running this on a Raspberry Pi with the camera connected via the Ribbon Cable?" << std::endl;
#elif defined V4L2
  if (!camera.isOpened(0)) {
    // Camera didn't successfully open.
    std::cerr << "Opening Camera failed." << std::endl; 
#else
  if (false) {
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
  current_mode = CameraMode::CONTINUOUS;
  print("Setup finished.");
}

Viewfinder::~Viewfinder() {
  camera.release();
}

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
	cv::Mat cv_frame, cv_frame1;

  camera.grab();
  if (current_mode == CONTINUOUS) {
    if (!camera.isOpened()) {
      print("Viewfinder in continuous mode, but camera not opened");
      return false;
    }
    // if the current mode is continuous, operate as video playback
    camera.retrieve(cv_frame);
  } else {
    // else show the latest image taken
    if (captures.empty()) {
      print("Viewfinder not in continuous mode, but captures is empty");
      return false;
    }
    cv_frame = captures.back();
  }
 
	if (cv_frame.empty()) {
    print("Frame empty");
    return false;
  }
 
  // apply a threshold to the frame
	cv::cvtColor (cv_frame, cv_frame1, CV_BGR2RGB);

  Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create_from_data(
												cv_frame1.data, 
												Gdk::COLORSPACE_RGB, 
												false, 
												8, 
                        cv_frame1.cols, 
                        cv_frame1.rows, 
                        cv_frame1.step);

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

  Glib::RefPtr<Pango::Layout> layout;
  switch (current_mode) {
    case CameraMode::CONTINUOUS:
      layout = create_pango_layout("LIVE");
      break;
    case CameraMode::SINGLE_CAPTURE:
      layout = create_pango_layout("ONE SHOT");
      break;
    case CameraMode::HDR:
      layout = create_pango_layout("HDR");
      break;
    default:
      layout = create_pango_layout("...");
      break;
	}

  int text_width;
  int text_height;

  //get the text dimensions (it updates the variables -- by reference)
  layout->get_pixel_size(text_width, text_height);

  // Position the text in the top-left
  cr->move_to(20,20);

  layout->show_in_cairo_context(cr);
}

cv::Mat Viewfinder::get_frame(bool fresh) {
  if (fresh) {
    cv::Mat frame;
    camera.grab();
    camera.retrieve(frame);
    set_frame(frame.clone());
  }

  // if there are no frames, throw exception
  if (captures.empty()) throw "no frames on captures vector";

  return captures.back().clone();
}

void Viewfinder::set_frame(cv::Mat frame) {
  // for now, there is only a need to store one element, but it is expected that
  // this will change
  captures.clear();
  captures.push_back(frame);
}

void Viewfinder::initialize_camera() {
  print("Initializing Camera");
#if defined RPI
  if (!camera.open()) {
    // Camera didn't successfully open.
    std::cerr << "Opening Camera failed." << std::endl; 
    std::cerr << "Are you running this on a Raspberry Pi with the camera connected via the Ribbon Cable?" << std::endl;
#elif defined V4L2
  if (!camera.open(0)) {
    // Camera didn't successfully open.
    std::cerr << "Opening Camera failed." << std::endl; 
#else
  if (false) {
#endif
  }
}

void Viewfinder::uninitialize_camera() {
  print("Uninitializing Camera");
  camera.release();
}
