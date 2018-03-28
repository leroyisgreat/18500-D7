# Notes

This is a collection of thoughts and processes we had when making this part of
the application. It marks a good place to put weird artifacts that we found
about, say, libraries we used that were either too lengthy or innapropriate for
in-line comments.

## Week 8 - Getting the Pi All Sorted Out

This was a bit of a process, but the supplied CD made everything pretty easy.
What wasn't easy was getting OpenCV to compile: The Raspbian package only had
OpenCV 2 that required the Gtk 2 libraries or would somehow link in Gtk 2. This
would cause linking to break, since the rest of the application was made in Gtk
3, and linking both is not allowed. 
Instead of compiling from source, I added
debian's Sid (Unstable) repo to apt, then I could just install the binaries and
not compile 2 GB of CV source code on the Pi. I couldn't find Sid's GPG keys, so
I just ignored errors for checking the files.

