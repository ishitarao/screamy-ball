//
// Created by Ishita Rao on 21/04/20.
//

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_

#include "location.h"

namespace screamy_ball {

enum class ObstacleType { kHigh, kLow };

class Obstacle {
 public:
  const int kHeight = 2;
  const int kMinLength;
  const int kMaxLength;
  ObstacleType type_;
  int length_;
  Location location_;
  Obstacle();
  Obstacle(ObstacleType type, const Location& location);
  ObstacleType GetObstacleType();

};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
