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
  Ball(const Location& location) : location(location) {}
  Location location;
  // theoretically, I could just convert this to a single Location object
  // instead of having a Ball struct, but that isn't expandable: what if I
  // wanted to add more properties to the ball?
};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_BALL_H_
