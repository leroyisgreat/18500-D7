/**
 * @file    exceptions.h
 *
 * @brief   enum for managing exception types
 *
 * @date    2018/04/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

enum Exceptions {
  VF_EMPTY = "captures empty",
  VF_OPEN_FAIL = "camera not opened",
  FRAME_EMPTY = "frame empty"
};

#endif // EXCEPTIONS_H
