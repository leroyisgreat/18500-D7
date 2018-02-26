/**
 * @file    viewfinder.h
 *
 * @brief   class for manaing the Viewfinder in the UI by overriding a Cairo object
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef VIEWFINDER_H
#define VIEWFINDER_H

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>

class Viewfinder : public Gtk::DrawingArea {
public:
	Viewfinder();
	virtual ~Viewfinder();

protected:
	//Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

  Glib::RefPtr<Gdk::Pixbuf> image;
};

#endif // VIEWFINDER_H

