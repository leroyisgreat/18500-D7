#include <opencv3/core.hpp>
#include "opencv3/highgui.hpp"
// #include <opencv2/opencv.hpp>
// #include "loadsave.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <vector>
#include <fstream>
#include <string>

// function for converting image names and times into 2 arrays.
// one of image matrices and one of image times.
void readImagesAndTimes(char** filenames, float* timesArray, int numImages,
                        vector<Mat> &imageList, vector<float> &exposureTimeList)
{
  
  times.assign(timesArray, timesArray + numImages);
  for(int i=0; i < numImages; i++)
  {
    Mat im = imread(filenames[i]);
    images.push_back(im);
  }



}

int main(int argc, char* argv) {
  /* These should be collected from the commandline */
  static const char* filenames[] = {"img_0.033.jpg", "img_0.25.jpg", "img_2.5.jpg", "img_15.jpg"};
  static const float timesArray[] = {1/30.0f, 0.25, 2.5, 15.0};
  int numImages = 4;

  Eigen::MatrixXf responseList;
  Eigen::VectorXf weightList;
  Eigen::MatrixXf hdrImage;

  vector<Mat> images;
  vector<float> times;
  readImagesAndTimes(filenames, timesArray, numImages, image, times);

  // Align input images
  Ptr<AlignMTB> alignMTB = createAlignMTB();
  alignMTB->process(images, images);




}
