//
// Created by Ishita Rao on 21/04/20.
//

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_

#include "location.h"

namespace screamy_ball {

enum class ObstacleType { kHigh, kLow };

class Obstacle {
  static const int kHeight = 2;

 public:
  ObstacleType obstacle_type_;
  Location location_;
  Obstacle();
  Obstacle(ObstacleType type, const Location& location);
  ObstacleType GetObstacleType();
  int GetHeight();

};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
