/**
 * @file    camera_state.h
 *
 * @brief   enum for managing what UI elements are being configured
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef CAMERA_STATE_H
#define CAMERA_STATE_H

enum CameraState {
  NONE,
  SINGLE_CAPTURE,
	HDR,
	AF
};

#endif // CAMERA_STATE_H
