// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_

#include "location.h"

namespace screamy_ball {

/**
 * Represents the obstacle type. 'high' obstacles must be ducked from, while
 * 'low' obstacles must be jumped over.
 */
enum class ObstacleType { kHigh, kLow };

/**
 * Represents an obstacle, or a spike, that the ball must avoid.
 */
struct Obstacle {
  const int kHeight;
  const int kMinLength;
  const int kMaxLength;
  ObstacleType type;
  int length;
  Location location;

  Obstacle(ObstacleType type, const Location& location):
      kMinLength(2),
      kMaxLength(4),
      kHeight(2),
      type(type),
      location(location),
      length(1) {}

};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
