// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/engine.h>

namespace screamy_ball {

Engine::Engine() :
    state_(BallState::kRolling),
    ball_(),
    kMaxHeight(6),
    kMinHeight(2),
    reached_max_height_(false) {}

void Engine::Roll() {
  switch (state_) {
    case BallState::kJumping: {
      Jump();
      break;
    }
    case BallState::kDucking: {
      state_ = BallState::kRolling;
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
    ball_.location_ = {ball_.location_.Row(), ball_.location_.Col() - 1};
    if (ball_.location_.Col() == kMinHeight) {
      reached_max_height_ = false;
      state_ = BallState::kRolling;
    }
  }

  ball_.location_ = {ball_.location_.Row(), ball_.location_.Col() + 1};
  if (ball_.location_.Col() == kMaxHeight) {
    reached_max_height_ = true;
  }
}

Ball Engine::GetBall() {
  return ball_;
}

void Engine::Reset() {
  state_ = BallState::kRolling;
}
}  // namespace screamy-ball
