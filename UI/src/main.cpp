#include "gui.h"
#include <gtkmm/application.h>


#include <gstreamermm.h>
#include <glibmm/main.h>
#include <glibmm/convert.h>
#include <iostream>
#include <stdlib.h>
#include <gstreamermm/playbin.h>

int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  Gst::init(argc, argv);
  if(argc < 2) {
    std::cout << "Usage: " << argv[0] << " <media file or uri>" << std::endl;
        return -1;
  }
// Create a playbin element.
#ifndef GSTREAMERMM_DISABLE_DEPRECATED
  Glib::RefPtr<Gst::PlayBin> playbin = Gst::PlayBin::create();
#else
  Glib::RefPtr<Gst::Element> playbin = Gst::ElementFactory::create_element("playbin");
#endif
  
  if(!playbin)
  {
    std::cerr << "The playbin2 element could not be created." << std::endl;
    return EXIT_FAILURE;
  }

  // Take the commandline argument and ensure that it is a uri:
  Glib::ustring uri;

  if(gst_uri_is_valid(argv[1]))
    uri = argv[1];
  else
    uri = Glib::filename_to_uri(argv[1]);

  // Set the playbyin2's uri property.
  playbin->set_property("uri", uri);

  // Create the main loop.
  mainloop = Glib::MainLoop::create();

  // Get the bus from the playbin, and add a bus watch to the default main
  // context with the default priority:
  Glib::RefPtr<Gst::Bus> bus = playbin->get_bus();
  bus->add_watch(sigc::ptr_fun(&on_bus_message));

  // Now set the playbin to the PLAYING state and start the main loop:
  std::cout << "Setting to PLAYING." << std::endl;
  playbin->set_state(Gst::STATE_PLAYING);
  std::cout << "Running." << std::endl;
  mainloop->run();

  // Clean up nicely:
  std::cout << "Returned. Setting state to NULL." << std::endl;
  playbin->set_state(Gst::STATE_NULL);


//////////

  Gui gui;

  //Shows the window and returns when it is closed.
  return app->run(gui);
}
