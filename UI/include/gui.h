/**
 * @file    gui.h
 *
 * @brief   top level class for manaing GTK UI elements
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef GUI_H 
#define GUI_H

#include "camera_state.h"
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

  Viewfinder        l3_viewfinder;
  GdkWindow *get_viewfinder_window();
protected:

  // Signal handlers:
  // Our new improved on_button_clicked(). (see below)
  void on_button_clicked(CameraState state);

  // Child widgets:
  Gtk::Box          l1_box;
  Gtk::Box          l2_box_top;
  Gtk::Box          l2_box_bottom;
  Gtk::Box          l3_box_left;
  Gtk::Button       mode_button_1, mode_button_2, mode_button_3;
  Gtk::Button       op_button_1, op_button_2, op_button_3;


	// camera state information:
	CameraState current_state;
	bool HDR_enable;
	bool AF_enable;
};

#endif // GUI_H
