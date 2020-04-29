// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_APPS_SCREAMYBALL_H_
#define FINALPROJECT_APPS_SCREAMYBALL_H_

#include <cinder/app/App.h>
#include <cinder/Timer.h>
#include <cinder/params/Params.h>
#include <sphinx/Recognizer.hpp>
#include <screamy-ball/engine.h>

namespace screamyball_app {

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

  void DrawMainMenu();
  void DrawHelp();
  void DrawBackground();
  void DrawBall();
  void DrawObstacles();
  void DrawGameOver();
  void DrawConfirmReset();

  void RecognizeCommands(const std::string& msg);
  void ParseUserInteraction(int event_code);

  void ResetGame();


 private:
  screamy_ball::Engine engine_;
  GameState state_;
  bool printed_game_over_;
  bool paused_;
  bool confirmed_reset_;
  int tile_size_;
  int height_;
  int width_;
  double delay_secs_;
  double last_time_;
  cinder::Timer timer_;
  sphinx::RecognizerRef recognizer_;
  cinder::params::InterfaceGlRef menu_ui_;
  cinder::params::InterfaceGlRef in_game_ui_;

};

}  // namespace screamyball_app

#endif  // FINALPROJECT_APPS_SCREAMYBALL_H_
