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

/**
 * The main function of Engine, to be called by the app. It checks for
 * collision, it creates obstacles, and it jumps when necessary.
 */
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

/**
 * Changes the ball's location to make it seem like it's jumping.
 */
void Engine::Jump() {
  if (reached_max_height_) {
    ball_.location = {ball_.location.Row(),
                       ball_.location.Col() + 1 };
    if (ball_.location.Col() == kMinHeight) {
      reached_max_height_ = false;
      state_ = BallState::kRolling;
    }
  } else {
    ball_.location = { ball_.location.Row(),
                        ball_.location.Col() - 1 };
    if (ball_.location.Col() == kMaxHeight) {
      reached_max_height_ = true;
    }
  }
}

/**
 * Creates an obstacle and changes its location on every update.
 */
void Engine::CreateObstacle() {
  // make obstacle move towards the ball
  obstacle_.location = {obstacle_.location.Row() - 1,
                         obstacle_.location.Col()};

  // if the obstacle hasn't reached the end of the screen, return
  if (obstacle_.location.Row() != -(obstacle_.length)) {
    return;
  }

  // create a random seed to generate a random type and a random length
  std::random_device dev;
  mt19937 rng(dev());

  obstacle_.type = GetObstacleType(rng);
  obstacle_.length = GetObstacleLength(rng);
  obstacle_.location = GetObstacleLocation();
}

/**
 * Randomly generates an obstacle type.
 * @param rng the random seed.
 * @return the generated ObstacleType enum.
 */
ObstacleType Engine::GetObstacleType(mt19937 rng) {
  std::uniform_int_distribution<mt19937::result_type> rand_bool(0,1);

  if (rand_bool(rng)) {
    return ObstacleType::kHigh;
  }
  return ObstacleType::kLow;
}

/**
 * Calculates the obstacle's next location.
 * @return the obstacle location.
 */
Location Engine::GetObstacleLocation() {
  if (obstacle_.type == ObstacleType::kHigh) {
    return { kWindowWidth, kMinHeight - obstacle_.kHeight - 1 };
  } else {
    return { kWindowWidth, kMinHeight };
  }
}

/**
 * Randomly generates the obstacle's length.
 * @param rng the random seed.
 * @return the generated length.
 */
int Engine::GetObstacleLength(mt19937 rng) {
  std::uniform_int_distribution<mt19937::result_type> rand_length(
      obstacle_.kMinLength, obstacle_.kMaxLength);
  return rand_length(rng);
}

/**
 * Checks if the ball has collided with an obstacle or not.
 * @return true if a collision has occurred, false otherwise.
 */
bool Engine::HasCollided() {
  int obstacle_x = obstacle_.location.Row();
  int ball_x = ball_.location.Row();

  // return false if the obstacle isn't within the range of the ball
  if (obstacle_x - 1 > ball_x || obstacle_x - 1 < ball_x - obstacle_.length) {
    return false;
  }

  switch (obstacle_.type) {
    case ObstacleType::kHigh: {
      return state_ != BallState::kDucking;
    }

    case ObstacleType::kLow: {
      return ball_.location.Col() >= kMinHeight - obstacle_.kHeight;
    }
  }
}

/**
 * Resets the Engine's state and all locations.
 */
void Engine::Reset() {
  state_ = BallState::kRolling;
  reached_max_height_ = false;
  ball_.location = { ball_.location.Row(), kMinHeight };
  obstacle_.location = { kWindowWidth, kMinHeight };
}
}  // namespace screamy-ball
