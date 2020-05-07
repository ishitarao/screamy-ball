// Copyright (c) 2020 Ishita Rao. All rights reserved.

#define CATCH_CONFIG_MAIN

#include <screamy-ball/engine.h>

#include <catch2/catch.hpp>

using namespace screamy_ball;

const int kWidth = 16;
const int kHeight = 16;

TEST_CASE("Jump test", "[jump]") {
  Location loc = {2, 14};
  Engine engine(loc, kWidth, kHeight);

  SECTION("BallState is not kJumping") {
    engine.state_ = BallState::kDucking;
    engine.Run();
    REQUIRE(engine.ball_.location == loc);
  }

  SECTION("BallState is kJumping") {
    engine.state_ = BallState::kJumping;

    SECTION("Ball has not reached max height") {
      engine.Run();
      REQUIRE(engine.ball_.location == Location(loc.Row(), loc.Col() - 1));
    }

    SECTION("Ball has reached max height") {
      Location initial_loc(loc.Row(), engine.kMaxHeight + 1);
      engine.ball_.location = initial_loc;
      engine.Run();
      REQUIRE(engine.ball_.location == Location(loc.Row(),
          engine.kMaxHeight));

      engine.Run();
      REQUIRE(engine.ball_.location == initial_loc);
    }
  }
}

TEST_CASE("Obstacle Creation test", "[obstacle]") {
  Location loc = {2, 14};
  Engine engine(loc, kWidth, kHeight);

  engine.Run();
  Location obstacle_loc = { 15, 14 };
  REQUIRE(engine.obstacle_.location == obstacle_loc);

  SECTION("Test for when the obstacle has reached the end of the screen") {
    engine.obstacle_.location = { -(engine.obstacle_.length), loc.Col() };
    engine.Run();
    Location new_loc = { kWidth, engine.obstacle_.location.Col() };
    REQUIRE(engine.obstacle_.location == new_loc);
  }
}

TEST_CASE("Collision test", "[collision]") {
  Location loc = {2, 14};
  Engine engine(loc, kWidth, kHeight);

  SECTION("Obstacle is out of range") {
    SECTION("Obstacle is in front of the ball") {
      // by default, when the obstacle is first created, it is out of range.
      engine.Run();
      REQUIRE(engine.state_ != BallState::kCollided);
    }

    SECTION("Obstacle is behind the ball") {
      engine.obstacle_.location = {2 - engine.obstacle_.length,
                                    loc.Col()};
      engine.Run();
      REQUIRE(engine.state_ != BallState::kCollided);
    }
  }

  SECTION("Obstacle is within range but there is no collision") {
    engine.obstacle_.location = loc;

    SECTION("Test for a high obstacle") {
      engine.obstacle_.type = ObstacleType::kHigh;
      engine.state_ = BallState::kDucking;
      engine.Run();

      REQUIRE(engine.state_ != BallState::kCollided);
    }

    SECTION("Test for a low obstacle") {
      engine.obstacle_.type = ObstacleType::kLow;
      engine.state_ = BallState::kJumping;
      engine.ball_.location = {2, 11};
      engine.Run();

      REQUIRE(engine.state_ != BallState::kCollided);
    }
  }

  SECTION("A collision occurs") {
    engine.obstacle_.location = { loc.Row() + 1, loc.Col() };

    SECTION("Test for a high obstacle") {
      engine.obstacle_.type = ObstacleType::kHigh;
      engine.state_ = BallState::kRolling;
      engine.Run();

      REQUIRE(engine.state_ == BallState::kCollided);
    }

    SECTION("Test for a low obstacle") {
      engine.obstacle_.type = ObstacleType::kLow;
      engine.state_ = BallState::kJumping;
      engine.Run();

      REQUIRE(engine.state_ == BallState::kCollided);
    }
  }
}