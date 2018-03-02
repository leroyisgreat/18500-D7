#include "gui.h"
#include <gtkmm/application.h>

#include <gstreamermm-1.0/gstreamermm.h>
#include <glibmm/main.h>
#include <glibmm/convert.h>
#include <iostream>
#include <stdlib.h>
#include <gstreamermm/playbin.h>

int main (int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  /////////////////
  // Begin gstreamer test
  /////////////////

  GstElement *pipeline, *source, *sink;
  GstBus *bus;
  //GstMessage *msg;
  GMainLoop *loop;
  GstStateChangeReturn ret;

  Gst::init(argc, argv);
  pipeline = gst_pipeline_new ("test-pipeline");
  source = gst_element_factory_make ("autovideosrc", "source");
  sink = gst_element_factory_make ("autovideosink", "sink");


  if (!pipeline || !source || !sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  g_object_set(G_OBJECT (source), "location", argv[1], NULL);

  gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);

	if (gst_element_link (source, sink) != TRUE) {
	  g_printerr ("Elements could not be linked.\n");
	  gst_object_unref (pipeline);
	  return -1;
	}

	ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
    std::cout << "Unable to set the pipeline to the playing state." << std::endl;
		gst_object_unref (pipeline);
		return -1;
	}

	bus = gst_element_get_bus (pipeline);

	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

  /////////////////
  // End gstreamer test
  /////////////////

  Gui gui;

  //Shows the window and returns when it is closed.
  app->run(gui);

  g_main_loop_unref(loop);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return 1;
}

