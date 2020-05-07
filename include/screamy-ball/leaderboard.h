// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_LEADERBOARD_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_LEADERBOARD_H_

#include "leaderboard.h"
#include "player.h"

#include <sqlite_modern_cpp.h>

#include <string>
#include <vector>

namespace screamy_ball {

/**
 * Represents the overall leaderboard.
 */
class Leaderboard {
 public:
  explicit Leaderboard(const std::string& db_path);
  void AddScoreToLeaderboard(const Player& player);

  std::vector<Player> RetrieveHighScores(const size_t limit);
  std::vector<Player> RetrieveHighScores(const Player&, const size_t limit);

  void Reset();

 private:
  sqlite::database database_;
};

}  // namespace screamy_ball

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_LEADERBOARD_H_
