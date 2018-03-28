# Notes

This is a collection of thoughts and processes we had when making this part of
the application. It marks a good place to put weird artifacts that we found
about, say, libraries we used that were either too lengthy or innapropriate for
in-line comments.

## Week 7.5 (Spring Break) - Switching to a pure OpenCV streamer

During the checkin with the TA, they mentioned we should make access to the
camera itself more API-like, with some kind of device lock. A method to do that
is to have an OpenCV thread and pass the thread of with a lock or something.
Doing this by hand gives us a lot of control but there will be so many pitfalls.

Shoutout to
[http://onthim.blogspot.com.au/2015/10/using-opencv-in-gtk-applications.html](this
page) for teaching me how to use OpenCV to stream from the WebCam into Gtk
Widgets.
