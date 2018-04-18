/**
 * @file    gui.h
 *
 * @brief   top level class for manaing GTK UI elements
 *
 *  ----------------------------------------------
 *  | window                                     |
 *  |                                            |
 *  | ------------------------------------------ |
 *  | | l1_box                                 | |
 *  | |                                        | |
 *  | | -------------------------------------- | |
 *  | | | l2_box_top                         | | |
 *  | | |                                    | | |
 *  | | | ---------------  ----------------- | | |
 *  | | | | l3_box_left |  | l3_viewfinder | | | |
 *  | | | |             |  |               | | | |
 *  | | | |             |  |               | | | |
 *  | | | ---------------  ----------------- | | |
 *  | | -------------------------------------- | |
 *  | |                                        | |
 *  | | -------------------------------------- | |
 *  | | | m_Toolbar                          | | |
 *  | | -------------------------------------- | |
 *  | ------------------------------------------ |
 *  ----------------------------------------------
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef GUI_H 
#define GUI_H

#include "camera_mode.h"
#include <gtkmm.h>
#include "viewfinder.h"

class Gui : public Gtk::Window {
public:
  Gui();
  virtual ~Gui();

private:
  // Signal handlers:
  void on_mode_change(CameraMode mode);
  void on_exposure_change();
  void on_iso_change();
  void on_save();
  void on_rotate();

  // regular functions
  void populate_toolbar();
  void set_current_mode(CameraMode mode);
  void hdr();

  // Child widgets:
  Gtk::Box                      l1_box;
  Gtk::Box                      l2_box_top;
  Gtk::Toolbar                  l2_toolbar;
  Gtk::Stack                    l3_stack;
  Viewfinder                    l3_viewfinder;
  Gtk::Box                      l4_options_CONTINUOUS;
  Gtk::Box                      l4_options_SINGLE_CAPTURE;
  Gtk::Box                      l4_options_HDR;
  Gtk::Button                   save, rotate_vf;
  Glib::RefPtr<Gtk::Adjustment> adjustment_exposure, adjustment_iso;
  Gtk::SpinButton               exposure, iso;
  Gtk::Label                    exposure_label, iso_label, rotate_vf_label;

	// camera Mode information:
	CameraMode current_mode;
};

#endif // GUI_H
