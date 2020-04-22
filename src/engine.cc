// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/engine.h>

namespace screamy_ball {

Engine::Engine(const Location& ball_loc) :
    state_(BallState::kRolling),
    ball_(ball_loc),
    kMaxHeight(ball_loc.Col() - 3),
    kMinHeight(ball_loc.Col()),
    reached_max_height_(false),
    is_obstacle_on_screen(false) {}

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
  if (is_obstacle_on_screen) {
    if (obstacle_.location_.Row() == 0) {
      obstacle_.location_ = {4, 2};
      is_obstacle_on_screen = false;
    }
    return;
  }
  //Obstacle obstacle(ObstacleType::kLow, {})
}

Ball Engine::GetBall() {
  return ball_;
}

Obstacle Engine::GetObstacle() {
  return obstacle_;
}

void Engine::Reset() {
  state_ = BallState::kRolling;
}
}  // namespace screamy-ball
