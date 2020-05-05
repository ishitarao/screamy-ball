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

struct Ball {
  Ball(const Location& location) : location_(location) {}
  Location location_;
};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_BALL_H_
