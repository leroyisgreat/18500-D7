#include "gui.h"
#include <gtkmm/application.h>

#include <gdk/gdkx.h>
#include <gtkmm.h>
#include <gstreamermm-1.0/gstreamermm.h>
#include <glibmm/main.h>
#include <glibmm/convert.h>
#include <iostream>
#include <stdlib.h>
#include <gstreamermm/playbin.h>

GMainLoop   *loop;
GstElement  *pipeline, *source, *sink;
GstBus      *bus;

Gui gui;

void hmm() {
  gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY(sink), GDK_WINDOW_XID(gui.get_viewfinder_window()));
	g_main_loop_run(loop);
}

int main (int argc, char *argv[]) {
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  /////////////////
  // Begin gstreamer test
  /////////////////

  GstStateChangeReturn ret;

  Gst::init(argc, argv);

  // if the webcam outputs JPEG images or something similar, then a new element
  // shoud be added here as a "jpegdec" decoder.
  // TODO: consider making flags or something for decoder functionality
	loop      = g_main_loop_new(NULL, FALSE);
  pipeline  = gst_pipeline_new("webcam-player");
  // webcams are typically video-4-linux-2 devices
  // autovideosrc could also work
  // v4l2src has a device property that by default is /dev/video0
  source    = gst_element_factory_make("v4l2src", "source");
  sink      = gst_element_factory_make("xvimagesink", "sink");

  // general NULL safety checks
  if (!pipeline || !source || !sink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);

  // sanity check that the source and sink are actually linked
	if (gst_element_link (source, sink) != TRUE) {
	  g_printerr("Elements source and sink could not be linked.\n");
	  gst_object_unref(pipeline);
	  return -1;
	}

  // begin playing the video
	ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
    std::cout << "Unable to set the pipeline to the playing state." << std::endl;
		gst_object_unref (pipeline);
		return -1;
	}

	bus = gst_element_get_bus (pipeline);

  std::cout << "Connecting signal..." << std::endl;
  gui.l3_viewfinder.signal_realize().connect(sigc::ptr_fun(&hmm));
  std::cout << "Connected" << std::endl;

  /////////////////
  // End gstreamer test
  /////////////////

  //Shows the window and returns when it is closed.
  app->run(gui);

  g_main_loop_unref(loop);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return 1;
}
