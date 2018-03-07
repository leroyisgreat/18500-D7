// thanks https://git.gnome.org//browse/gstreamermm/tree/examples/media_player_gtkmm/player_window.cc

#include "camera_state.h"
#include "gui.h"
#include <iostream>
#include <gdk/gdkx.h>

Gui::Gui(const Glib::RefPtr<Gst::PlayBin>& playbin)
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
  /*
   * begin setting up the GUI elements
   * TODO: move this to a separate function
   */

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

  //sink.signal_realize().connect(sigc::mem_fun(*this,
  //            &Gui::on_viewfinder_realize));

  /*
   * end setting up GUI elements
   * begin setting up Gstreamer elements
   * TODO: move this to a separate function
   */

  m_playbin = playbin;
  Glib::RefPtr<Gst::Bus> bus = m_playbin->get_bus();

  //playbin->property_video_sink() = Gst::GtkSink::create();
  GstElement *gtk_sink = gst_element_factory_make ("gtksink", NULL);
  GtkWidget sink;
  g_object_get (gtk_sink, "widget", &sink, NULL);

  playbin->property_video_sink() = Glib::wrap(gtk_sink);
  //m_playbin->set_property("video-sink", Gst::ElementFactory::create_element("gtksink"));


  Gtk::Widget video = Glib::wrap(sink);
  l2_box_top.pack_start(video, true, true);
  // Enable synchronous message emission to set up video (if any) at the
  // exact appropriate time
  bus->enable_sync_message_emission();

  // Connect to bus's synchronous message signal for overlaying
  bus->signal_sync_message().connect(
              sigc::mem_fun(*this, &Gui::on_bus_message_sync));

  /*
   * end setting up Gstreamer elements
   */

  show_all_children();
}

Gui::~Gui() {}

static Glib::RefPtr<Gst::VideoOverlay> find_overlay(Glib::RefPtr<Gst::Element> element) {
  auto overlay = Glib::RefPtr<Gst::VideoOverlay>::cast_dynamic(element);

  if (overlay)
    return overlay;

  auto bin = Glib::RefPtr<Gst::Bin>::cast_dynamic(element);

  if (!bin)
    return overlay;

  for (auto e : bin->get_children())
  {
    overlay = find_overlay(e);
    if (overlay)
      break;
  }

  return overlay;
}

void Gui::on_button_clicked(CameraState state) {
  current_state = state;
  //l3_viewfinder.setCameraState(state);
  // viewfinder must be refreshed
  //l3_viewfinder.queue_draw();
  std::cout << "State button - " << state << " was pressed" << std::endl;
}

//void Gui::on_viewfinder_realize() {
//  viewfinder_window_xid = GDK_WINDOW_XID(sink.get_window()->gobj());
//}

void Gui::on_bus_message_sync(const Glib::RefPtr<Gst::Message>& message) {
  // ignore any message that isn't saying we are able to set overlays
  if (!gst_is_video_overlay_prepare_window_handle_message (message->gobj()))
   return;

  // set the overlay if the viewfinder window has been realized and the overlay
  // is ready
  if (viewfinder_window_xid != 0) {
    Glib::RefPtr<Gst::VideoOverlay> videooverlay =
        find_overlay(Glib::RefPtr<Gst::Element>::cast_dynamic(message->get_source()));

    if (videooverlay) {
      videooverlay->set_window_handle(viewfinder_window_xid);
    }
  } else {
    std::cerr << "Should have obtained video_window_handle by now!" << std::endl;
  }
}

