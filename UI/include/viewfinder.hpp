/**
 * @file    viewfinder.hpp
 *
 * @brief   class for manaing the Viewfinder in the UI by overriding a Cairo object
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

// this #define is for changing the type of the camera easily
// for the Raspberry Pi Camera: #define RPI
// for the standard V4L2 Camera: #define V4L2
#ifndef VIEWFINDER_HPP
#define VIEWFINDER_HPP

#define V4L2

#include "viewfinder_mode.hpp"
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

  /** @brief setter for viewfinder mode
   *
   * @param mode target mode to put the viewfinder into
   */
	inline void set_mode(ViewfinderMode mode) {
		current_mode = mode;
	}

  /** @brief getter for viewfinder mode
   *
   * @return current mode
   */
	inline ViewfinderMode get_mode() {
		return current_mode;
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
   * if the current_mode is STREAM, then this is mostly useless
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

  /**
   * @brief starts captureing video
   *
   * @param path to save video
   */
  void start_capture(std::string location);

  /**
   * @brief stops capturing video
   */
  bool stop_capture();

  /** @brief initializes camera device for use */
  void initialize_camera();

  /** @brief uninitializes camera device fo other programs to use it */
  void uninitialize_camera();

  /** @brief string to relay more information from GUI to viewfinder */
  std::string hud_info;

protected:
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

private:
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
  inline void print(std::string input) {
    std::cout << "VIEWFINDER: " << input << std::endl;
  }

  /** 
   * @brief simple function to easily throw meaningful errors
   *
   * @param info information string to be printed
   * @param err exception to be thrown
   */
  inline void error(const char *err, std::string info) {
    std::cout << "[E] VIEWFINDER: " << info << std::endl;
    std::string error_str("VIEWFINDER: ");
    error_str += err;
    throw std::runtime_error(error_str);
  }

  /** @brief current mode the camera is in - determines HUD elements */
  ViewfinderMode current_mode;

  /** @brief recently captured frame, when waiting to save */
  cv::Mat still_capture;

  /** @brief recently captured video, when waiting to save */
  cv::VideoWriter video_capture;

  /** @brief actual camera device */
#if defined RPI
  raspicam::RaspiCam_Cv camera;
#elif defined V4L2
  cv::VideoCapture camera;
#endif

  /** @brief period to call on_timeout in milliseconds
   *
   * 50 -> 20 FPS
   * 40 -> 25 FPS
   * 33 -> 30 FPS
   */
  const int FRAMERATE_INTERVAL = 40;
};

#endif // VIEWFINDER_HPP
