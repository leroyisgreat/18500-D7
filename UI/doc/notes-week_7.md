# Notes

This is a collection of thoughts and processes we had when making this part of
the application. It marks a good place to put weird artifacts that we found
about, say, libraries we used that were either too lengthy or innapropriate for
in-line comments.

## Week 7 - Getting GStreamer to play nicely

GStreamer is pretty complicated. There aren't a lot of guides out there and I
have to try and find the Gstramer*mm* documentations in particular. I eventually
came across [this Medium
article](https://medium.com/lifesjourneythroughalens/implementing-gstreamer-webcam-usb-internal-streaming-mac-c-clion-76de0fdb8b34)
explaining the general layout and getting me up and running, and then I moved
on to the [official Freedesktop
documentation](https://gstreamer.freedesktop.org/documentation/index.html).

To get the code to compile with `#include <gstreamermm-1.0/gstreamermm.h>`, I
had to update the Makefile to include `gstreamermm-1.0` to the `CFLAGS`
libraries, so that line is now 
    CFLAGS = -Wall -Iinclude `pkg-config gtkmm-3.0 --cflags --libs gstreamermm-1.0\`
It doesn't interfere with any other compiling elements.

### GStreamer
So essentially their are 2 kinds of objects for GStreamer: elements and bins.

All typical objects are called "elements". They are made from a
`gst_element_factory_make()` factory. These elements can have some (possibly zero)
source pads and sink pads. Before they do anything, though, you must change the
element's state to something like `GST_STATE_PLAYING` using
`gst_element_set_state()`. When something is playing, Gst internally sets up new
threads to manage the process of playing something from file.

Bins are containers for elements or other bins. Pipelines link up the sources
and sinks to do what you want. A pipeline is just a special type of bin, and
every GStreamer application has to have at least one pipeline. Imaging that
between every pair of source and sink has a small pipeline, and then you
increasingly step out by grouping larger and larger chunks.
If you change the state of a bin, it will take care of it's contingency, setting
the state recursively.

There is a sink that is a gtkpixbuf, and that at first looked ideal for
integration with the rest of GTK, but there was a note saying it was unusably
slow for playback. So the next thing to do was to set the sink to a xvimagesink
and use GstXOverlay to essentially position that playback box onto another GTK
widget.

It turns out that in order to do the overlay, I had to connect the overlay
action to whenever the Viewfinder element was realized, but in order to do that
I needed the Gui to be made. There didn't seem to be a way to build the Gui
without actually running it.

`https://developer.gnome.org/gstreamermm/1.0/` and
`https://git.gnome.org/browse/gstreamermm/tree/examples/media_player_gtkmm` have
been infinitely helpful resources for figuring out exactly what types the C++
versions of Gst Elements, GObjects, and the like were, and how this project was
going to fit together.
