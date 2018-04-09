// thanks https://git.gnome.org//browse/gstreamermm/tree/examples/media_player_gtkmm/player_window.cc

#include "camera_state.h"
#include "gui.h"
#include <iostream>
#include <gdk/gdkx.h>
#include <opencv2/opencv.hpp>
#include <python2.7/Python.h>

Gui::Gui()
: l1_box(                   Gtk::ORIENTATION_VERTICAL,  4),
  l2_box_top(               Gtk::ORIENTATION_HORIZONTAL,4),
  l4_options_CONTINUOUS(    Gtk::ORIENTATION_VERTICAL,  4),
  l4_options_SINGLE_CAPTURE(Gtk::ORIENTATION_VERTICAL,  4),
  l4_options_HDR(           Gtk::ORIENTATION_VERTICAL,  4),
  save("Save"),
  adjustment_exposure(Gtk::Adjustment::create(1.0, 1.0, 100.0, 1.0, 10.0, 0.0)),
  adjustment_iso(Gtk::Adjustment::create(1.0, 1.0, 100.0, 1.0, 10.0, 0.0)),
  exposure(adjustment_exposure),
  iso(adjustment_iso),
  exposure_label("Exposure [1-100]", Gtk::ALIGN_START),
  iso_label("ISO [1-100]", Gtk::ALIGN_START)
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

  // add left panel to top l2 box
  l2_box_top.pack_start(l3_stack, false, false);
  // add viewfinder to top l2 box
  l2_box_top.pack_start(l3_viewfinder, true, true);

  l3_stack.add(l4_options_CONTINUOUS, "continuous options");
  l3_stack.add(l4_options_SINGLE_CAPTURE, "single capture options");
  l3_stack.add(l4_options_HDR, "HDR options");
  l3_stack.set_visible_child(l4_options_CONTINUOUS);
  l4_options_SINGLE_CAPTURE.pack_start(save, false, false);
  l4_options_SINGLE_CAPTURE.pack_start(exposure_label, false, false);
  l4_options_SINGLE_CAPTURE.pack_start(exposure, false, false);
  l4_options_SINGLE_CAPTURE.pack_start(iso_label, false, false);
  l4_options_SINGLE_CAPTURE.pack_start(iso, false, false);
  save.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_save));
  exposure.signal_changed().connect(sigc::mem_fun(*this, &Gui::on_exposure_change));
  iso.signal_changed().connect(sigc::mem_fun(*this, &Gui::on_iso_change));

  // setup Python environment
  Py_Initialize();
  
  show_all_children();
  std::cout << "GUI Setup finished." << std::endl;
}

Gui::~Gui() {}

void Gui::on_state_change(CameraState state) {
  std::cout << "Changing Camera State" << std::endl;

  Gui::set_current_state(state);
  // viewfinder must be refreshed
  l3_viewfinder.queue_draw();

  switch (state) {
    case CameraState::CONTINUOUS:
      l3_stack.set_visible_child(l4_options_CONTINUOUS);
      break;
    case CameraState::SINGLE_CAPTURE:
      l3_stack.set_visible_child(l4_options_SINGLE_CAPTURE);
      l3_viewfinder.get_frame(true);
      l3_viewfinder.queue_draw();
      break;
    case CameraState::HDR:
      l3_stack.set_visible_child(l4_options_HDR);
      hdr();
      break;
    default:
      std::cerr << "Error unknown camera state enterred" << std::endl;
  }
}

void Gui::on_exposure_change() {
  l3_viewfinder.set_property(
      CV_CAP_PROP_AUTO_EXPOSURE,0);
  l3_viewfinder.set_property(
      CV_CAP_PROP_EXPOSURE, exposure.get_value_as_int());
}

void Gui::on_iso_change() {
  l3_viewfinder.set_property(
      CV_CAP_PROP_GAIN, iso.get_value_as_int());
}

void Gui::on_save() {
  std::stringstream ss;
  ss << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  ss << ".jpg";
  cv::Mat frame = l3_viewfinder.get_frame();
  cv::imwrite(ss.str().c_str(), frame);
  Gui::on_state_change(CameraState::CONTINUOUS);
}

void Gui::populate_toolbar() {
  // create the photo capture button
  auto pc_image = new Gtk::Image("resources/shoot.ico");
  auto pc_button = new Gtk::ToolButton(*pc_image, "shoot");
  pc_button->set_tooltip_text("Take photo");
  // link photo capture button to function
  pc_button->signal_clicked().connect(sigc::bind<CameraState>(
      sigc::mem_fun(*this, &Gui::on_state_change),
      CameraState::SINGLE_CAPTURE));
  l2_toolbar.append(*pc_button);

  // create the photo capture button
  auto live_image = new Gtk::Image("resources/live.ico");
  auto live_button = new Gtk::ToolButton(*live_image, "live");
  live_button->set_tooltip_text("Continuous Shooting");
  // link photo capture button to change camera state
  live_button->signal_clicked().connect(sigc::bind<CameraState>(
      sigc::mem_fun(*this, &Gui::on_state_change),
      CameraState::CONTINUOUS));
  l2_toolbar.append(*live_button);

  // create the HDR mode button
  auto hdr_image = new Gtk::Image("resources/hdr.ico");
  auto hdr_button = new Gtk::ToolButton(*hdr_image, "hdr");
  hdr_button->set_tooltip_text("Toggle HDR mode");
  hdr_button->signal_clicked().connect(sigc::bind<CameraState>(
      sigc::mem_fun(*this, &Gui::on_state_change),
      CameraState::HDR));
  l2_toolbar.append(*hdr_button);
}

void Gui::set_current_state(CameraState state) {
  current_state = state;
  l3_viewfinder.set_camera_state(state);
}

void Gui::hdr() {
  FILE* file = fopen("~/workspace/18500-D7/hdr/runhdrpi.py", "r");
  PyRun_SimpleFile(file, "~/workspace/18500-D7/hdr/runhdrpi.py");
}

