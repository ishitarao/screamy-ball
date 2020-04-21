// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_

#include "ball.h"

namespace screamy_ball {
class Engine {
  const int kMaxHeight;
  const int kMinHeight;
  bool reached_max_height_;
  Ball ball_;

 public:
  BallState state_;

 public:
  Engine();
  void Roll();
  void Jump();
  void Reset();
  Ball GetBall();

};
}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
