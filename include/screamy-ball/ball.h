//
// Created by Ishita Rao on 20/04/20.
//

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_BALL_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_BALL_H_

#include "location.h"

namespace screamy_ball {
enum class BallState {
  kRolling,
  kDucking,
  kJumping,
  kCollided
};

class Ball {
 public:
  Ball(const Location& location) : location_(location) {}
  Location location_; //confirm if this is okay
};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_BALL_H_
