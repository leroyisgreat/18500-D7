/**
 * @file    viewfinder.cpp
 *
 * @brief   implementation for Viewfinder class
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#include "viewfinder.h"
#include <cairomm/context.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <giomm/resource.h>
#include <glibmm/fileutils.h>
#include <iostream>

Viewfinder::Viewfinder() {
  // TODO: For now, we're loading a static image to the PixBuf for testing 
  //        purposes. Later we'll stream from a camera device on system.
  try {
    image = Gdk::Pixbuf::create_from_file("resources/bg.jpg");
  } catch(const Gio::ResourceError& ex) {
    std::cerr << "ResourceError: " << ex.what() << std::endl;
  } catch(const Gdk::PixbufError& ex) {
    std::cerr << "PixbufError: " << ex.what() << std::endl;
  }

  /*
  if (image) 
    ap = image->get_width() /(double) image->get_height();
		//set_size_request(image->get_width()/2, image->get_height()/2);
    //*/
}

Viewfinder::~Viewfinder() {}

bool Viewfinder::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  if (!image)
    return false;

  Gtk::Allocation allocation = get_allocation();
  const int frame_width = allocation.get_width();
  const int frame_height = allocation.get_height();
  const int img_width = image->get_width();
  const int img_height = image->get_height();

  // Draw the image in the top right corner, fixed aspect ratio to fit the window
  double scale = std::min(frame_width /(double) img_width,
                          frame_height /(double) img_height);
  Glib::RefPtr<Gdk::Pixbuf> image_scaled = 
        image->scale_simple(img_width*scale, 
                            img_height*scale, 
                            Gdk::INTERP_BILINEAR);
  Gdk::Cairo::set_source_pixbuf(cr, image_scaled, 0, 0);

  cr->paint();

  return true;
}
