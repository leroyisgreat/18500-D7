# Notes

This is a collection of thoughts and processes we had when making this part of
the application. It marks a good place to put weird artifacts that we found
about, say, libraries we used that were either too lengthy or innapropriate for
in-line comments.

## Week 12 (I think? -> April 30) - Panorama

Panorama posed quite a few issues. OpenCV didn't work on some of our computers
(particularly the SIFT functionality) turns out all of the Ubuntu repos leave
out crucial bits, like Stitcher, and everything from opencv\_contrib because
there was a split with non-free parts (like an implementation for SIFT, did you
know that it's copywritten? Yikes.)
There's a really fun installer script at [this GitHub
repo](https://github.com/milq/milq/blob/master/scripts/bash/install-opencv.sh)
and I would really recomend using it. To install non-free (SIFT) I added a line
to the script and cloned the [OpenCV Contrib
repo](https://github.com/opencv/opencv_contrib). Here's the modification:

```
...

# 3. INSTALL THE LIBRARY

sudo apt-get install -y unzip wget
wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip
unzip ${OPENCV_VERSION}.zip
rm ${OPENCV_VERSION}.zip
mv opencv-${OPENCV_VERSION} OpenCV
cd OpenCV
mkdir build
cd build
cmake -DWITH_QT=ON -DWITH_OPENGL=ON -DFORCE_VTK=ON -DWITH_TBB=ON -DWITH_GDAL=ON -DWITH_XINE=ON -DBUILD_EXAMPLES=ON -DENABLE_PRECOMPILED_HEADERS=OFF -DOPENCV_EXTRA_MODULES_PATH=/path/to/opencv_contrib/modules ..
make -j4
sudo make install
sudo ldconfig 

...
```

Almost there. There's a bug in the contrib code that would cause it to fail. So
I replaced the contents of `opencv_contrib/modules/datasets/CMakeLists.txt` with

```
set (the_description "datasets framework")
ocv_define_module (datasets opencv_core opencv_imgcodecs opencv_ml opencv_flann OPTIONAL opencv_text WRAP python )
ocv_warnings_disable (CMAKE_CXX_FLAGS / wd4267) # flann, Win64
```

Then alas it finally compiled with everything I wanted.

Next was to actually make a good panorama implementation.
