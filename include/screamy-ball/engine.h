// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_

#include "ball.h"
#include "obstacle.h"

#include <random>

namespace screamy_ball {
using std::mt19937;

class Engine {
  const int kWindowWidth;
  const int kWindowHeight;
  bool reached_max_height_;

 public:
  const int kMaxHeight;
  const int kMinHeight;
  BallState state_;
  Obstacle obstacle_;
  Ball ball_;

 public:
  Engine(const Location& ball_loc, int  width, int height);
  void Run();
  void Jump();
  Obstacle CreateObstacle();
  Location GetObstacleLocation();
  int GetObstacleLength(mt19937 rng);
  bool HasCollided();
  void Reset();

};

ObstacleType GetObstacleType(mt19937 rng);

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
