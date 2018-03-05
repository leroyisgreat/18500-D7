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
#include <gtkmm.h>
#include <gstreamermm.h>

class Gui : public Gtk::Window {
public:
  Gui(const Glib::RefPtr<Gst::PlayBin>& playbin);
  virtual ~Gui();

protected:
  // Signal handlers:
  void on_button_clicked(CameraState state);
  void on_bus_message_sync(const Glib::RefPtr<Gst::Message>& message);
  void on_viewfinder_realize();

  // Child widgets:
  Gtk::Box          l1_box;
  Gtk::Box          l2_box_top;
  Gtk::Box          l2_box_bottom;
  Gtk::Box          l3_box_left;
  Gtk::Button       mode_button_1, mode_button_2, mode_button_3;
  Gtk::Button       op_button_1, op_button_2, op_button_3;
  Gtk::DrawingArea  sink;

  // gst playbin
  Glib::RefPtr<Gst::Element> m_playbin;

	// camera state information:
	CameraState current_state;

  // widget state information
  gulong viewfinder_window_xid;
};

#endif // GUI_H
