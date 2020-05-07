// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_

#include "ball.h"
#include "obstacle.h"

#include <random>

namespace screamy_ball {
using std::mt19937;

/**
 * Represents the Game's Engine, responsible for moving the ball and
 * tracking the ball's and the obstacle's locations.
 */
class Engine {
 public:
  Engine(const Location& ball_loc, int  width, int height);
  void Run();
  void Reset();

  const int kMaxHeight;
  const int kMinHeight;
  BallState state_;
  Obstacle obstacle_;
  Ball ball_;

 private:
  void Jump();
  void CreateObstacle();
  static ObstacleType GetObstacleType(mt19937 rng);
  Location GetObstacleLocation();
  int GetObstacleLength(mt19937 rng);
  bool HasCollided();

  const int kWindowWidth;
  const int kWindowHeight;
  bool reached_max_height_;

};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_ENGINE_H_
