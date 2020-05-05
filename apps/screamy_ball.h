// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_APPS_SCREAMYBALL_H_
#define FINALPROJECT_APPS_SCREAMYBALL_H_

#include <cinder/Timer.h>
#include <cinder/app/App.h>
#include <cinder/params/Params.h>
#include <screamy-ball/engine.h>

#include <sphinx/Recognizer.hpp>
#include <string>

namespace screamyball_app {

using cinder::ColorA;
using cinder::ivec2;
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
  void SetupRecognizer();
  void SetupMainMenuUI();
  void SetupInGameUI();
  void SetupGeneralUI();

  void RunEngine();

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

  void RecognizeCommands(const std::string& msg);
  void ParseUserInteraction(int event_code);
  bool IsInGameInteraction(int event_code);

  void ResetGame();

 private:
  const size_t kTileSize;
  const size_t kHeight;
  const size_t kWidth;
  const size_t kDefaultFontSize;
  const size_t kTextBoxBuffer;
  const float kLocMultiplier;
  const ivec2 kUiDimensions;

  bool paused_;
  bool confirmed_reset_;
  double delay_secs_;
  double last_update_secs_;

  screamy_ball::Engine engine_;
  GameState state_;
  GameState last_state_;

  cinder::Timer timer_;
  sphinx::RecognizerRef recognizer_;
  cinder::params::InterfaceGlRef menu_ui_;
  cinder::params::InterfaceGlRef in_game_ui_;
  cinder::params::InterfaceGlRef general_ui_;

};

string PrettyPrintElapsedTime(double time_secs);

}  // namespace screamyball_app

#endif  // FINALPROJECT_APPS_SCREAMYBALL_H_
