#include "gui.h"
#include <gtkmm.h>
#include <glibmm.h>
#include <gstreamermm-1.0/gstreamermm.h>
#include <iostream>

int main (int argc, char *argv[]) {
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
	Gst::init(argc, argv);

  /*  
   * begin setting up Gstreamer elements
   */

  Glib::RefPtr<Gst::PlayBin> playbin = Gst::PlayBin::create("playbin");

  if (!playbin) {
    std::cerr << "The playbin could not be created." << std::endl;
    return 1;
  }

  // set the source GstElement to the webcam
  // webcams are typically video-4-linux-2 devices
  // autovideosrc could also work
  // v4l2src has a device property that by default is /dev/video0
  playbin->property_uri() = (Glib::ustring) "v4l2:///dev/video0";
  playbin->property_video_sink() = Gst::XvImageSink::create();

  // begin playing the video
  Gst::StateChangeReturn ret = playbin->set_state(Gst::STATE_PLAYING);
	if (ret == Gst::STATE_CHANGE_FAILURE) {
    std::cerr << "Unable to set the pipeline to the playing state." << std::endl;
		return -1;
	}

  /*  
   * end setting up Gstreamer elements
   * begin setting up Gui element
   */

  // call to implicit constructor
  // this took me a minute to wrap my head around
  Gui gui(playbin);

  // shows the window and returns when it is closed.
  return app->run(gui);
}
