// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/engine.h>

namespace screamy_ball {

Engine::Engine(const Location& ball_loc, int width, int height) :
    state_(BallState::kRolling),
    ball_(ball_loc),
    kMaxHeight(ball_loc.Col() - 5),
    kMinHeight(ball_loc.Col()),
    kWindowWidth(width),
    kWindowHeight(height),
    reached_max_height_(false),
    obstacle_(ObstacleType::kLow, { width, kMinHeight }) {}

void Engine::Run() {
  if (HasCollided()) {
    state_ = BallState::kCollided;
    return;
  }
  CreateObstacle();
  if (state_ == BallState::kJumping) {
    Jump();
  }
}

void Engine::Jump() {
  if (reached_max_height_) {
    ball_.location_ = {ball_.location_.Row(),
                       ball_.location_.Col() + 1 };
    if (ball_.location_.Col() == kMinHeight) {
      reached_max_height_ = false;
      state_ = BallState::kRolling;
    }
  } else {
    ball_.location_ = { ball_.location_.Row(),
                        ball_.location_.Col() - 1 };
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

  // create a random seed to generate a random type and a random length
  std::random_device dev;
  mt19937 rng(dev());

  obstacle_.type_ = GetObstacleType(rng);
  obstacle_.length_ = GetObstacleLength(rng);
  obstacle_.location_ = GetObstacleLocation();

  return obstacle_;
}

ObstacleType GetObstacleType(mt19937 rng) {
  std::uniform_int_distribution<mt19937::result_type> rand_bool(0,1);

  if (rand_bool(rng)) {
    return ObstacleType::kHigh;
  }
  return ObstacleType::kLow;
}

Location Engine::GetObstacleLocation() {
  if (obstacle_.type_ == ObstacleType::kHigh) {
    return { kWindowWidth, kMinHeight - obstacle_.kHeight - 1 };
  } else {
    return { kWindowWidth, kMinHeight };
  }
}

int Engine::GetObstacleLength(mt19937 rng) {
  std::uniform_int_distribution<mt19937::result_type> rand_length(
      obstacle_.kMinLength, obstacle_.kMaxLength);
  return rand_length(rng);
}

bool Engine::HasCollided() {
  int obstacle_x = obstacle_.location_.Row();
  int ball_x = ball_.location_.Row();

  // return false if the obstacle isn't within the range of the ball
  if (obstacle_x - 1 > ball_x || obstacle_x - 1 < ball_x - obstacle_.length_) {
    return false;
  }

  switch (obstacle_.type_) {
    case ObstacleType::kHigh: {
      return state_ != BallState::kDucking;
    }

    case ObstacleType::kLow: {
      return ball_.location_.Col() >= kMinHeight - obstacle_.kHeight;
    }
  }
}

void Engine::Reset() {
  state_ = BallState::kRolling;
  reached_max_height_ = false;
  ball_.location_ = { ball_.location_.Row(), kMinHeight };
  obstacle_.location_ = { kWindowWidth, kMinHeight };
}
}  // namespace screamy-ball
