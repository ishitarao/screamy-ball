// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/leaderboard.h>
#include <screamy-ball/player.h>
#include <sqlite_modern_cpp.h>

#include <string>
#include <vector>

namespace screamy_ball {

using std::string;
using std::vector;

LeaderBoard::LeaderBoard(const string& db_path) : database_(db_path) {
  database_ << "CREATE TABLE if not exists leaderboard (\n"
         "  name  TEXT NOT NULL,\n"
         " elapsed_time TEXT NOT NULL\n"
         ");";
}

void LeaderBoard::AddScoreToLeaderBoard(const Player& player) {
  database_ << "INSERT INTO leaderboard (name, elapsed_time) "
               "\nVALUES (?, ?);"
            << player.name << player.elapsed_time;
}

vector<Player> GetPlayers(sqlite::database_binder* rows) {
  vector<Player> players;

  for (auto&& row : *rows) {
    string name;
    string elapsed_time;
    row >> name >> elapsed_time;
    Player player = { name, elapsed_time };
    players.push_back(player);
  }

  return players;
}

vector<Player> LeaderBoard::RetrieveHighScores(const size_t limit) {
  auto rows = database_ << "SELECT * "
                     "\nFROM leaderboard "
                     "\nORDER BY \nelapsed_time DESC "
                     "\nLIMIT ?;" << limit;
  return GetPlayers(&rows);
}

vector<Player> LeaderBoard::RetrieveHighScores(const Player& player,
                                               const size_t limit) {
  auto rows = database_ << "SELECT * \nFROM leaderboard "
                     "\nWHERE name = ? "
                     "\nORDER BY \nelapsed_time DESC "
                     "\nLIMIT ?;" << player.name << limit;
  return GetPlayers(&rows);
}

void LeaderBoard::Reset() {
  database_ << "DELETE \nFROM leaderboard";
}

}  // namespace screamy_ball
