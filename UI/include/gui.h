#ifndef GUI_H 
#define GUI_H

#include "viewfinder.h"
#include <gtkmm/aspectframe.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/window.h>

class Gui : public Gtk::Window {
public:
  Gui();
  virtual ~Gui();

protected:

  // Signal handlers:
  // Our new improved on_button_clicked(). (see below)
  void on_button_clicked(Glib::ustring data);

  // Child widgets:
  Gtk::Box          l1_box;
  Gtk::Box          l2_box_top;
  Gtk::Box          l2_box_bottom;
  Gtk::Box          l3_box_left;
  Gtk::Button m_button1, m_button2;

  Viewfinder        l3_viewfinder;
};

#endif // GUI_H
