// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_APPS_SCREAMYBALL_H_
#define FINALPROJECT_APPS_SCREAMYBALL_H_

#include <cinder/Timer.h>
#include <cinder/app/App.h>
#include <cinder/audio/Voice.h>
#include <cinder/params/Params.h>
#include <screamy-ball/engine.h>
#include <screamy-ball/leaderboard.h>
#include <screamy-ball/player.h>

#include <sphinx/Recognizer.hpp>
#include <string>
#include <utility>

namespace screamyball_app {

using cinder::ColorA;
using cinder::ivec2;
using cinder::audio::VoiceRef;

using screamy_ball::Player;
using std::string;

enum class GameState {
  kMenu,
  kHelp,
  kPlaying,
  kGameOver,
  kConfirmingReset
};

class ScreamyBall : public cinder::app::App {
 public:
  ScreamyBall();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;
  void keyUp(cinder::app::KeyEvent) override;
  void mouseDown(cinder::app::MouseEvent) override;
  void mouseUp(cinder::app::MouseEvent) override;

 private:
  struct Audio {
    VoiceRef audio_obj_;
    const string asset_name_;
    explicit Audio(string asset_name):
        asset_name_(std::move(asset_name)) {}
  };

  void SetupRecognizer();
  void SetupMainMenuUi();
  void SetupInGameUi();
  void SetupGeneralUi();
  void SetupMusic(Audio& audio);

  void PopulateLeaderboards();
  void RunEngine();
  void Mute();

  template <typename C>
  void PrintText(const string& text, float font_size, const C& text_color,
                 const ivec2& size, const cinder::vec2& loc);
  void DrawMainMenu();
  void DrawHelp();
  void DrawBackground();
  void DrawBall();
  void DrawObstacles();
  void DrawGameOver();
  void DrawConfirmReset();

  void RecognizeCommands(const std::string& message);
  void ParseUserInteraction(int event_code);
  bool IsInGameInteraction(int event_code);

  void ResetGame();

 private:
  const size_t kTileSize;
  const size_t kHeight;
  const size_t kWidth;
  const size_t kDefaultFontSize;
  const size_t kTextBoxBuffer;
  const size_t kLeaderboardLimit;
  const float kLocMultiplier;
  const float kDefaultVolume;
  const ivec2 kUiDimensions;
  const string kPlayerName;

  bool paused_;
  bool confirmed_reset_;
  double delay_secs_;
  double last_update_secs_;
  string elapsed_time_;
  GameState state_;
  GameState last_state_;

  screamy_ball::Engine engine_;
  screamy_ball::LeaderBoard leaderboard_;
  std::vector<Player> top_players_;
  std::vector<Player> current_player_top_scores_;

  cinder::Timer timer_;
  cinder::params::InterfaceGlRef menu_ui_;
  cinder::params::InterfaceGlRef in_game_ui_;
  cinder::params::InterfaceGlRef general_ui_;
  Audio bg_music_;
  Audio scream_audio_;
  sphinx::RecognizerRef recognizer_;

};

const int kNumSeconds = 60;
string PrettyPrintElapsedTime(double time_secs);

}  // namespace screamyball_app

#endif  // FINALPROJECT_APPS_SCREAMYBALL_H_
