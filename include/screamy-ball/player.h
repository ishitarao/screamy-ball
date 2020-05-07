// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_PLAYER_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_PLAYER_H_

#include <string>

namespace screamy_ball {

/**
 * A Player object contains the player's name and their elapsed_time during
 * the current game. This struct is for leaderboard purposes.
 */
struct Player {
  Player(const std::string& name, const std::string& elapsed_time) :
    name(name),
    elapsed_time(elapsed_time) {}

  std::string name;
  std::string elapsed_time;
};

}  // namespace screamy_ball

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_PLAYER_H_
