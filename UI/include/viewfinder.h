/**
 * @file    viewfinder.h
 *
 * @brief   class for manaing the Viewfinder in the UI by overriding a Cairo object
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef VIEWFINDER_H
#define VIEWFINDER_H

#include "camera_state.h"
#include <gtkmm/drawingarea.h>
#include <opencv2/opencv.hpp>

class Viewfinder : public Gtk::DrawingArea {
public:
	Viewfinder();
	virtual ~Viewfinder();
  void setCameraState(CameraState state);

protected:
  cv::VideoCapture cv_cap;
  bool cv_opened;
  virtual bool on_draw (const Cairo::RefPtr<Cairo::Context> &cr);
  bool on_timeout ();

  CameraState current_state;

private:
  void draw_hud(const Cairo::RefPtr<Cairo::Context> &cr, 
                int scaled_width, int scaled_height);
};

#endif // VIEWFINDER_H

