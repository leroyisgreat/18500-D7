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
    m_image = Gdk::Pixbuf::create_from_file("resources/bg.jpg");
  } catch(const Gio::ResourceError& ex) {
    std::cerr << "ResourceError: " << ex.what() << std::endl;
  } catch(const Gdk::PixbufError& ex) {
    std::cerr << "PixbufError: " << ex.what() << std::endl;
  }

  //if (m_image) 
	//	set_size_request(m_image->get_width()/2, m_image->get_height()/2);
}

Viewfinder::~Viewfinder() {}

bool Viewfinder::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  if (!m_image)
    return false;

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  // Draw the image in the middle of the drawing area, or (if the image is
  // larger than the drawing area) draw the middle part of the image.
  Gdk::Cairo::set_source_pixbuf(cr, m_image,
    (width - m_image->get_width())/2, (height - m_image->get_height())/2);
  cr->paint();

  return true;
}
