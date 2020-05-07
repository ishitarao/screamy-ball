// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/leaderboard.h>
#include <screamy-ball/player.h>
#include <sqlite_modern_cpp.h>

#include <string>
#include <vector>

namespace screamy_ball {

using std::string;
using std::vector;

/**
 * Creates a new leaderboard table if it doesn't already exist.
 * @param db_path the path to the database.
 */
Leaderboard::Leaderboard(const string& db_path) : database_(db_path) {
  database_ << "CREATE TABLE if not exists leaderboard (\n"
         "  name  TEXT NOT NULL,\n"
         " elapsed_time TEXT NOT NULL\n"
         ");";
}

/**
 * Adds a player to the leaderboard.
 * @param player the player whose name and time is being added.
 */
void Leaderboard::AddScoreToLeaderboard(const Player& player) {
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

/**
 * Returns a list of the players with the highest scores, in decreasing order.
 * The size of the list should be no greater than `limit`.
 * @param limit The number of records we're limiting the list to.
 * @return a vector containing the list of players with the highest scores.
 */
vector<Player> Leaderboard::RetrieveHighScores(const size_t limit) {
  auto rows = database_ << "SELECT * "
                     "\nFROM leaderboard "
                     "\nORDER BY \nelapsed_time DESC "
                     "\nLIMIT ?;" << limit;
  return GetPlayers(&rows);
}

/**
 * Returns a list of the players with the longest times,
 * that have the same `name` as the input player name.
 * The size of the list should be no greater than `limit`.
 * @param player the player whose times we're retrieving.
 * @param limit The number of records we're limiting the list to.
 * @return a vector containing the list of times of the given player.
 */
vector<Player> Leaderboard::RetrieveHighScores(const Player& player,
                                               const size_t limit) {
  auto rows = database_ << "SELECT * \nFROM leaderboard "
                     "\nWHERE name = ? "
                     "\nORDER BY \nelapsed_time DESC "
                     "\nLIMIT ?;" << player.name << limit;
  return GetPlayers(&rows);
}

/**
 * Deletes all records in the leaderboard.
 */
void Leaderboard::Reset() {
  database_ << "DELETE \nFROM leaderboard";
}

}  // namespace screamy_ball
