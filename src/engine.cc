// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/engine.h>

namespace screamy_ball {

Engine::Engine(int ball_loc_x, int ball_loc_y) :
    state_(BallState::kRolling),
    ball_(ball_loc_x, ball_loc_y),
    kMaxHeight(ball_loc_y - 3),
    kMinHeight(ball_loc_y),
    reached_max_height_(false) {}

void Engine::Roll() {
  switch (state_) {
    case BallState::kJumping: {
      Jump();
      break;
    }
    case BallState::kDucking: {

      break;
    }
    case BallState::kRolling: {

    }
    case BallState::kCollided: {

    }
  }
}

void Engine::Jump() {
  if (reached_max_height_) {
    ball_.location_ = {ball_.location_.Row(), ball_.location_.Col() + 1};
    if (ball_.location_.Col() == kMinHeight) {
      reached_max_height_ = false;
      state_ = BallState::kRolling;
    }
  } else {
    ball_.location_ = {ball_.location_.Row(), ball_.location_.Col() - 1};
    if (ball_.location_.Col() == kMaxHeight) {
      reached_max_height_ = true;
    }
  }
}

void Engine::CreateObstacle() {

}

Ball Engine::GetBall() {
  return ball_;
}

void Engine::Reset() {
  state_ = BallState::kRolling;
}
}  // namespace screamy-ball
