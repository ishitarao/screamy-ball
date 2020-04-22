// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_

#include "ball.h"
#include "obstacle.h"

namespace screamy_ball {
class Engine {
  const int kMaxHeight;
  const int kMinHeight;
  bool reached_max_height_;
  bool is_obstacle_on_screen;
  Ball ball_;
  Obstacle obstacle_;

 public:
  BallState state_;

 public:
  Engine(const Location& ball_loc);
  void Roll();
  void Jump();
  void CreateObstacle();
  Ball GetBall();
  Obstacle GetObstacle();
  void Reset();

};
}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
