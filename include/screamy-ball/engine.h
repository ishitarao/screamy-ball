// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_

#include "ball.h"
#include "obstacle.h"

namespace screamy_ball {
class Engine {
  const int kMaxHeight;
  const int kMinHeight;
  const int kWindowWidth;
  const int kWindowHeight;
  bool reached_max_height_;
  Ball ball_;
  Obstacle obstacle_;

 public:
  BallState state_;

 public:
  Engine(const Location& ball_loc, int  width, int height);
  void Run();
  void Jump();
  Obstacle CreateObstacle();
  Ball GetBall();
  Obstacle GetObstacle();
  int GetMinHeight();
  bool HasCollided();
  void Reset();

};
}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
