/**
 * @file    viewfinder.cpp
 *
 * @brief   implementation for Viewfinder class
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#include "camera_state.h"
#include "gui.h"
#include "viewfinder.h"
#include <cairomm/context.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <giomm/resource.h>
#include <glibmm/fileutils.h>
#include <iostream>

Viewfinder::Viewfinder() {
  // open camera
  std::cout << "Opening Raspbery Pi Camera..." << std::endl;
  if (false){//!camera.open()) {
    // Camera didn't successfully open.
    std::cerr << "Opening Raspberry Pi Camera failed." << std::endl; 
    std::cerr << "Are you running this on a Raspberry Pi with the camera connected via the Ribbon Cable?" << std::endl;
  } else {
    // Waiting for camera to "stabalize"
    std::cout << "Sleeping for 3 seconds..." << std::endl;
    sleep(3);
    // Camera opened successfully
    //
    // call on_timeout once every 50ms (20 times a second)
    // this is the Gtk(TM) way to redraw a window on command
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Viewfinder::on_timeout), 50);
  }
  std::cout << "Setup finished." << std::endl;
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
  switch (current_state) {
    case CameraState::CONTINUOUS:
      layout = create_pango_layout("LIVE");
      break;
    case CameraState::SINGLE_CAPTURE:
      layout = create_pango_layout("ONE SHOT");
      break;
    case CameraState::HDR:
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

bool Viewfinder::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
	if (!camera.isOpened()) return false;
 
	cv::Mat cv_frame, cv_frame1;

  // if the current mode is continuous, operate as video playback
  // else show the latest image taken
  if (current_state == CONTINUOUS) {
    camera.grab();
    camera.retrieve(cv_frame);
  } else {
    cv_frame = captures.back();
  }
 
	if (cv_frame.empty()) return false;
 
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

void Viewfinder::set_camera_state(CameraState state) {
  current_state = state;
}

void Viewfinder::get_capture() {
	if (!camera.isOpened()) return;
 
	cv::Mat frame;
  camera.grab();
  camera.retrieve(frame);
  captures.clear();
  captures.push_back(frame.clone());
}

void Viewfinder::save(const char *filename) {
  cv::imwrite(filename, captures.back());
}
