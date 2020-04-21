// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_

#include "ball.h"

namespace screamy_ball {
class Engine {
 public:
  BallState state_;

 public:
  void Roll();
  void Reset();


};
}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
