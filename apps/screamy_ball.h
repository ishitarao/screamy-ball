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
  kPlaying,
  kGameOver,
};

class ScreamyBall : public cinder::app::App {
 public:
  ScreamyBall();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;

 private:
  void DrawGameOver();
  void ResetGame();
  bool ConfirmReset();
  void DrawBall();
  void DrawObstacles();


 private:
  screamy_ball::Engine engine_;
  bool printed_game_over_;
  bool paused_;
  GameState state_;
  cinder::Timer timer_;
  cinder::params::InterfaceGlRef params_;

};

}  // namespace screamyball_app

#endif  // FINALPROJECT_APPS_SCREAMYBALL_H_
