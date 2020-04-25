// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/engine.h>

#include <random>

namespace screamy_ball {

Engine::Engine(const Location& ball_loc, int width, int height) :
    state_(BallState::kRolling),
    ball_(ball_loc),
    kMaxHeight(ball_loc.Col() - 4),
    kMinHeight(ball_loc.Col()),
    kWindowWidth(width),
    kWindowHeight(height),
    reached_max_height_(false),
    obstacle_(ObstacleType::kLow, {ball_loc.Col(), kMinHeight}) {}

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

Obstacle Engine::CreateObstacle() {
  // make obstacle move towards the ball
  obstacle_.location_ = {obstacle_.location_.Row() - 1,
                         obstacle_.location_.Col()};

  // if the obstacle hasn't reached the end of the screen, return
  if (obstacle_.location_.Row() != -(obstacle_.length_)) {
    return obstacle_;
  }

  // generate a random obstacle length and a random obstacle type
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> rand_length(2,5);
  std::uniform_int_distribution<std::mt19937::result_type> rand_bool(0,1);

  ObstacleType type = rand_bool(rng) ? ObstacleType::kHigh : ObstacleType::kLow;

  obstacle_.location_ = {kWindowWidth, kMinHeight};
  obstacle_.type_ = type;
  obstacle_.length_ = rand_length(rng);

  return obstacle_;
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
