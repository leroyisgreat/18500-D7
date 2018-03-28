// thanks https://git.gnome.org//browse/gstreamermm/tree/examples/media_player_gtkmm/player_window.cc

#include "camera_state.h"
#include "gui.h"
#include <iostream>
#include <gdk/gdkx.h>
#include "Python.h"

Gui::Gui()
: l1_box(       Gtk::ORIENTATION_VERTICAL,    4),
  l2_box_top(   Gtk::ORIENTATION_HORIZONTAL,  4),
  l3_box_left(  Gtk::ORIENTATION_VERTICAL,    4),
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
  l1_box.pack_start(l2_toolbar, false, false);
  Gui::populate_toolbar();
  l2_toolbar.set_icon_size(Gtk::BuiltinIconSize::ICON_SIZE_SMALL_TOOLBAR);

  // add left panel to top l2 box
  l2_box_top.pack_start(l3_box_left, false, false);

  // add viewfinder to top l2 box
  l2_box_top.pack_start(l3_viewfinder, true, true);

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
  if (state == CameraState::HDR) {
    Py_Initialize();
    PyObject* py_HDR_str = PyString_FromString((char*)"/home/leroyce/workspace/18500/hdr/hdr.py");
    PyObject* py_HDR = PyImport_Import(py_HDR_str);
    if (!py_HDR) std::cout << "ERROR" << std::endl;
    PyObject* myFunction = PyObject_GetAttrString(py_HDR,(char*)"HDR");
    PyObject_CallObject(myFunction, NULL);
  }
}


void Gui::populate_toolbar() {
  // create the photo capture button
  auto pc_image = new Gtk::Image("resources/shoot.ico");
  auto pc_button = new Gtk::ToolButton(*pc_image, "shoot");
  pc_button->set_tooltip_text("Take photo");
  l2_toolbar.append(*pc_button);

  // create the HDR mode button
  auto hdr_image = new Gtk::Image("resources/hdr.ico");
  auto hdr_button = new Gtk::ToolButton(*hdr_image, "hdr");
  hdr_button->set_tooltip_text("Toggle HDR mode");
  l2_toolbar.append(*hdr_button);
}
