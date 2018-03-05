#include "camera_state.h"
#include "gui.h"
#include <iostream>

Gui::Gui()
: l1_box(       Gtk::ORIENTATION_VERTICAL,    4),
  l2_box_top(   Gtk::ORIENTATION_HORIZONTAL,  4),
  l2_box_bottom(Gtk::ORIENTATION_HORIZONTAL,  4),
  l3_box_left(  Gtk::ORIENTATION_VERTICAL,    4),
  mode_button_1("Plain"),
  mode_button_2("HDR"),
  mode_button_3("AF"),
  op_button_1("Option 1"),
  op_button_2("Option 2"),
  op_button_3("Option 3")
{
  // set title of new window.
  set_title("18-500 Team D7 GUI");
  // set border width of the window.
  set_border_width(10);
  set_default_size(800,600);

  // add highest level container (vertical box)
  add(l1_box);

  // add second level containers (two horizontal boxes)
  l1_box.pack_start(l2_box_top, true, true);
  l1_box.pack_start(l2_box_bottom, false, false);

  // add left panel to top l2 box
  l2_box_top.pack_start(l3_box_left, false, false);

  // add viewfinder to top l2 box
  l2_box_top.pack_start(l3_viewfinder, true, true);


  // Now when the button is clicked, we call the "on_button_clicked" function
  // with a pointer to "button 1" as it's argument
  mode_button_1.signal_clicked().connect(sigc::bind<CameraState>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), CameraState::NONE));
  mode_button_2.signal_clicked().connect(sigc::bind<CameraState>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), CameraState::HDR));
  mode_button_3.signal_clicked().connect(sigc::bind<CameraState>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), CameraState::AF));

  l2_box_bottom.pack_start(mode_button_1, false, false);
  l2_box_bottom.pack_start(mode_button_2, false, false);
  l2_box_bottom.pack_start(mode_button_3, false, false);

  // call the same signal handler with a different argument,
  // passing a pointer to "button 2" instead.
  op_button_1.signal_clicked().connect(sigc::bind<-1, CameraState>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), CameraState::NONE));
  op_button_2.signal_clicked().connect(sigc::bind<-1, CameraState>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), CameraState::NONE));
  op_button_3.signal_clicked().connect(sigc::bind<-1, CameraState>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), CameraState::NONE));
  
  l3_box_left.pack_start(op_button_1, false, false);
  l3_box_left.pack_start(op_button_2, false, false);
  l3_box_left.pack_start(op_button_3, false, false);


  show_all_children();
}

Gui::~Gui() {}

void Gui::on_button_clicked(CameraState state) {
  current_state = state;
  l3_viewfinder.setCameraState(state);
  // viewfinder must be refreshed
  l3_viewfinder.queue_draw();
  std::cout << "State button - " << state << " was pressed" << std::endl;
}

GdkWindow *Gui::get_viewfinder_window() {
  return l3_viewfinder.get_window()->gobj();
}
