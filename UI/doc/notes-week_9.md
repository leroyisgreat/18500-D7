# Notes

This is a collection of thoughts and processes we had when making this part of
the application. It marks a good place to put weird artifacts that we found
about, say, libraries we used that were either too lengthy or innapropriate for
in-line comments.

## Week 9 - API Work

Here much of the Viewfinder code was finalized. The viewfinder itself was
re-written to be much more well-designed and hard to mess up - things were
properly marked as public, private, inline, etc. And proper accessors/mutators
were added. Also the Python interface, save, and HDR functionality was moved out
of the viewfinder and into the GUI, but now the quesiton was how to make the
Python application (HDR) know that the viewfinder existed and that it was the
way to access the camera?

-- asside -- up until now the HDR functionality was done by just calling the
script from within C++, having it write to file, then having the C++ program
read from file. As gracious as this is for working (two separate threads
accessing the camera device descriptor - how did this work?) this had to go 
with a proper pipeline replacing it.

Many comments were added, like a final Gtk widget nesting description (here it
is if you'd like to see it:```
   ----------------------------------------------
   | window                                     |
   |                                            |
   | ------------------------------------------ |
   | | l1_box                                 | |
   | |                                        | |
   | | -------------------------------------- | |
   | | | l2_box_top                         | | |
   | | |                                    | | |
   | | | ---------------  ----------------- | | |
   | | | | l3_box_left |  | l3_viewfinder | | | |
   | | | |             |  |               | | | |
   | | | |             |  |               | | | |
   | | | ---------------  ----------------- | | |
   | | -------------------------------------- | |
   | |                                        | |
   | | -------------------------------------- | |
   | | | m_Toolbar                          | | |
   | | -------------------------------------- | |
   | ------------------------------------------ |
   ----------------------------------------------
```)
