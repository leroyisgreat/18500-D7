/**
 * @file    viewfinder.h
 *
 * @brief   class for manaing the Viewfinder in the UI by overriding a Cairo object
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

// this #define is for changing the type of the camera easily
// for the Raspberry Pi Camera: #define RPI
// for the standard V4L2 Camera: #define V4L2
#define RPI

#ifndef VIEWFINDER_H
#define VIEWFINDER_H

#include "camera_mode.h"
#include <gtkmm/drawingarea.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

class Viewfinder : public Gtk::DrawingArea {
public:
  /** @brief generic constructor - opens camera device handle and initializes
   * Ctypes infrastructure*/
	Viewfinder();

  /** @brief generic destructor - cleans up camera device handle */
	virtual ~Viewfinder();

  /** @brief setter for camera mode
   *
   * There wasn't an easy way to propogate the mode between the GUI and the
   * viewfinder. From a design persepctive, the GUI should be the only object
   * holding this information, but it didn't make sense to have the viewfinder
   * have access to the object that created it. This was a less-than-elegant
   * solution.
   *
   * @param mode target mode to put the viewfinder into
   */
	inline void set_camera_mode(CameraMode mode) {
		current_mode = mode;
	}

  /** @brief getter for the camera device frame
   *
   * @param fresh if true then a fresh capture will be made and given else a
   * stale frame from the captures vector will be given
   *
   * @return a cv frame as described above
   */
	cv::Mat get_frame(bool fresh = false);

  /** @brief setter for the camera device frame
   *
   * if the current_mode is CONTINUOUS, then this is mostly useless
   * TODO: Consider failing if the camera mode is in CONTINUOUS
   * if the current_mode is something else, then this accesses the top of the
   * captures vector, which should be the frame currently being displayed in the
   * viewfinder
   *
   * @param frame frame to be appended onto captures
   */
  void set_frame(cv::Mat frame);

  /** @brief accessor to the internal camera - sets the camera properties
   *
   * @param propId property to be set
   * @param value value to which the property will be set
   */
  inline void set_property(int propId, double value) {
      camera.set(propId, value);
  }

  void initialize_camera();
  void uninitialize_camera();

private:
  /** @brief timeout function called at FRAMERATE_INTERVAL to re-fetch a frame
   * from the camera
   */
  bool on_timeout();

  /**
   * @brief drawing function claled every time the Widget is altered or the
   * on_timeout interval is triggered
   *
   * @param cr Autopointer to a cairo context that can be edited
   */
  virtual bool on_draw (const Cairo::RefPtr<Cairo::Context> &cr);

  /**
   * @brief Add Heads-Up-display information to viewfinder screen, ontop of
   * capture frame
   *
   * @param cr Autopointer to a cairo context that can be edited
   */
  void draw_hud(const Cairo::RefPtr<Cairo::Context> &cr, 
                int scaled_width, int scaled_height);

  /** 
   * @brief simple function to preface prints with more information
   *
   * @param input information string to be printed
   */
  inline void print(const char *input) {
    std::cout << "VIEWFINDER: " << input << std::endl;
  }

  /** 
   * @brief simple function to easily throw meaningful errors
   *
   * @param info information string to be printed
   * @param err exception to be thrown
   */
  inline void error(const char *err, const char *info) {
    std::cout << "[E] VIEWFINDER: " << info << std::endl;
    std::string error_str("VIEWFINDER: ");
    error_str += err;
    throw std::runtime_error(error_str);
  }

  /** @brief current mode the camera is in - determines HUD elements */
  CameraMode current_mode;

  /** @brief vector of frames recently captures - usually contains last capture
   * taken */
  std::vector<cv::Mat> captures;

  /** @brief actual camera device */
#if defined RPI
  raspicam::RaspiCam_Cv camera;
#elif defined V4L2
  cv::VideoCapture camera;
#else
  void camera = NULL;
#endif

  /** @brief period to call on_timeout in milliseconds
   *
   * 50 -> 20 FPS
   * 33 -> 30 FPS
   */
  const int FRAMERATE_INTERVAL = 50;
};

#endif // VIEWFINDER_H

