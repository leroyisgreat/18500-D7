# Capstone Group D7
## Weekly Update: 3/22/18

## LeRoy's Entry

This was a bit of a process, but the supplied CD made everything pretty easy.
What wasn't easy was getting OpenCV to compile: The Raspbian package only had
OpenCV 2 that required the Gtk 2 libraries or would somehow link in Gtk 2. This
would cause linking to break, since the rest of the application was made in Gtk
3, and linking both is not allowed.
Instead of compiling from source, I added
debian's Sid (Unstable) repo to apt, then I could just install the binaries and
not compile 2 GB of CV source code on the Pi. I couldn't find Sid's GPG keys, so
I just ignored errors for checking the files.

## Marcus's Entry

For me, this week was spent doing a good deal of research and development.
After implementing HDR in python, we decided to take some time to implement the
algorithm in C++ so that we could more easily take advantage of the parallelism
afforded by OpenCL. Therefore a few hours were spent on this pursuit, ultimately,
however we decided to abandon the idea for now after talking to Evi, because we
felt that it would be more important to implement the algorithms first, and then
move on to optimization if time permitted. Other than this, I spent time beginning
to implement the video stabilization. I read a few research papers on the topic
and reviewed several open source projects on the same topic. Finally, this week
from researching the rpi camera, I realized that it does not have focus control
which meant that we had to abandon our dreams of making an autofocus feature,
and replace it with a similarly impressive and rigorous image processing algorithm.

## Amber's Entry

RPi is registered to CMU-SECURE and the .img of Rapsbian with fully installed
OpenCV libraries is now available for us to use. We ended up ruling out the
Face-Tracking for Auto Focus feature, since the camera for the RPi has a
fixed-focal length, we can’t manually adjust the focus. My next step is to
look into other features for us to implement. Next on the list was RAW to
JPEG compression. Looking into the file structure of the Pi Camera’s images,
 it looks like the -raw option tacks on the original RAW format to the
compressed, (and pre-filtered) JPEG image that the RPi produces. The JPEG
already has color correction, de-noise filtering and a few other filters
(removing general de-bayer artifacts). The Bayer filter for the RPi is BGGR.
My plan is to continue analyzing the structure of the file to see if it’s
feasible to implement a Digital Negative editing program where the user will
control various aspects of the image - RGB value, gamma correction, exposure,
white balance, and many other features. I presume this will take longer than
I first anticipated (given that now there’s more of a ‘customization’ element
to processing these images), so currently I’m researching Image-Stitching
algorithms to use for the Panoramic image mode, as this seems to have more
support on the web.


