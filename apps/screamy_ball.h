// Copyright (c) 2020 Ishita Rao. All rights reserved.

#ifndef FINALPROJECT_APPS_SCREAMYBALL_H_
#define FINALPROJECT_APPS_SCREAMYBALL_H_

#include <cinder/app/App.h>
#include <cinder/Timer.h>
#include <sphinx/Recognizer.hpp>
#include <screamy-ball/engine.h>

namespace screamyball_app {

enum class GameState {
  kPlaying,
  kGameOver,
  kConfirmingReset,
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
  void RecognizeCommands(const std::string& msg);
  void ParseUserInteraction(int event_code);
  void DrawBackground();
  void DrawButtons();
  void DrawGameOver();
  void ResetGame();
  void ConfirmReset();
  void DrawBall();
  void DrawObstacles();


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

};

}  // namespace screamyball_app

#endif  // FINALPROJECT_APPS_SCREAMYBALL_H_
