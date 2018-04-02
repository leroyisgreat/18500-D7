/**
 * @file    gui.h
 *
 * @brief   top level class for manaing GTK UI elements
 *
 *  -------------------------------------------
 *  | window                                  |
 *  |                                         |
 *  | --------------------------------------- |
 *  | | l1_box                              | |
 *  | |                                     | |
 *  | | ----------------------------------- | |
 *  | | | l2_box_top                      | | |
 *  | | |                                 | | |
 *  | | | ---------------  -------------- | | |
 *  | | | | l3_box_left |  | viewfinder | | | |
 *  | | | |             |  |            | | | |
 *  | | | |             |  |            | | | |
 *  | | | ---------------  -------------- | | |
 *  | | ----------------------------------- | |
 *  | |                                     | |
 *  | | ----------------------------------- | |
 *  | | | m_Toolbar                       | | |
 *  | | ----------------------------------- | |
 *  | --------------------------------------- |
 *  -------------------------------------------
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef GUI_H 
#define GUI_H

#include "camera_state.h"
#include <gtkmm.h>
#include "viewfinder.h"

class Gui : public Gtk::Window {
public:
  Gui();
  virtual ~Gui();

protected:
  // Signal handlers:
  void on_state_change(CameraState state);
  void on_exposure_change();
  void on_iso_change();
  void on_save();

  // Child widgets:
  Gtk::Box            l1_box;
  Gtk::Box            l2_box_top;
  Gtk::Toolbar        l2_toolbar;
  Gtk::Stack          l3_stack;
  Viewfinder          l3_viewfinder;
  Gtk::Box            l4_options_CONTINUOUS;
  Gtk::Box            l4_options_SINGLE_CAPTURE;
  Gtk::Box            l4_options_HDR;
  Gtk::Button         save;
  Gtk::SpinButton     exposure;
  Gtk::SpinButton     iso;
  Glib::RefPtr<Gtk::Adjustment> adjustment_exposure;
  Glib::RefPtr<Gtk::Adjustment> adjustment_iso;

	// camera state information:
	CameraState current_state;

private:
  void populate_toolbar();
  void set_current_state(CameraState state);
};

#endif // GUI_H
