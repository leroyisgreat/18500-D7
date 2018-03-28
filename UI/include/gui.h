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
 *  | | | m_ToolPalette                   | | |
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
  void on_button_clicked(CameraState state);

  // Child widgets:
  Gtk::Box            l1_box;
  Gtk::Box            l2_box_top;
  Gtk::Toolbar        l2_toolbar;
  Gtk::Box            l3_box_left;
  Viewfinder          l3_viewfinder;
  Gtk::Button         op_button_1, op_button_2, op_button_3;

	// camera state information:
	CameraState current_state;

private:
  void populate_toolbar();
};

#endif // GUI_H
