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
#define V4L2

#ifndef VIEWFINDER_H
#define VIEWFINDER_H

#include "camera_state.h"
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

  /** @brief setter for camera state
   *
   * There wasn't an easy way to propogate the state between the GUI and the
   * viewfinder. From a design persepctive, the GUI should be the only object
   * holding this information, but it didn't make sense to have the viewfinder
   * have access to the object that created it. This was a less-than-elegant
   * solution.
   *
   * @param state target state to put the viewfinder into
   */
	inline void set_camera_state(CameraState state) {
		current_state = state;
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
   * if the current_state is CONTINUOUS, then this is mostly useless
   * TODO: Consider failing if the camera state is in CONTINUOUS
   * if the current_state is something else, then this accesses the top of the
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

  /** @brief current state the camera is in - determines HUD elements */
  CameraState current_state;

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

