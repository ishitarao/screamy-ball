// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include "screamy_ball.h"

#include <cinder/app/App.h>
#include <cinder/Font.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <gflags/gflags.h>


namespace screamyball_app {

using cinder::Color;
using cinder::ColorA;
using cinder::TextBox;
using cinder::app::KeyEvent;
using screamy_ball::BallState;
using std::string;

#if defined(CINDER_COCOA_TOUCH)
const char kNormalFont[] = "Arial";
const char kBoldFont[] = "Arial-BoldMT";
const char kDifferentFont[] = "AmericanTypewriter";
#elif defined(CINDER_LINUX)
const char kNormalFont[] = "Arial Unicode MS";
const char kBoldFont[] = "Arial Unicode MS";
const char kDifferentFont[] = "Purisa";
#else
const char kNormalFont[] = "Arial";
const char kBoldFont[] = "Arial Bold";
const char kDifferentFont[] = "Papyrus";
#endif

DECLARE_uint32(width);
DECLARE_uint32(height);
DECLARE_uint32(tilesize);
DECLARE_uint32(speed);

ScreamyBall::ScreamyBall()
    : printed_game_over_{false},
      paused_{false},
      state_{GameState::kPlaying} { }

void ScreamyBall::setup() {
  cinder::gl::enableDepthWrite();
  cinder::gl::enableDepthRead();
  timer_.start();

  // Create the interface and give it a name.
  params_ = cinder::params::InterfaceGl::create( getWindow(),
      "App parameters",
      cinder::app::toPixels( cinder::ivec2( 200, 400 ) ) );
}

string PrettyPrintElapsedTime(double time_secs) {
  int seconds = (int)time_secs;

  int hours = seconds / (60 * 60);
  seconds -= hours * (60 * 60);

  int minutes = seconds / 60;
  seconds -= minutes * 60;

  std::stringstream sstream;
  sstream << hours << ':' << minutes << ':' << seconds;
  return sstream.str();
}

void ScreamyBall::update() {
  if (state_ == GameState::kGameOver) {
    timer_.stop();
  }
}

void ScreamyBall::draw() {
  cinder::gl::enableAlphaBlending();

  if (state_ == GameState::kGameOver) {
    if (!printed_game_over_) {
      cinder::gl::clear(Color::black());
    }
    DrawGameOver();
    return;
  }

  if (paused_) return;

  cinder::gl::clear(Color::black());
  DrawBall();
  DrawObstacles();

}

template <typename C>
void PrintText(const string& text, const C& color, const cinder::ivec2& size,
               const cinder::vec2& loc,
               const ColorA& bg_color = ColorA::zero()) {
  cinder::gl::color(color);

  auto box = TextBox()
      .alignment(TextBox::CENTER)
      .font(cinder::Font(kNormalFont, 30))
      .size(size)
      .color(color)
      .backgroundColor(bg_color)
      .text(text);

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {loc.x - box_size.x / 2, loc.y - box_size.y / 2};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);
}

void ScreamyBall::DrawGameOver() {
  // Lazily print.
  if (printed_game_over_) return;

  const cinder::vec2 center = getWindowCenter();
  const cinder::ivec2 size = {500, 50};
  const Color color = Color::white();
  string elapsed_time = PrettyPrintElapsedTime(timer_.getSeconds());
  PrintText("Game Over :(", color, size, center);
  PrintText("Your time: " + elapsed_time, color, size, center);

  printed_game_over_ = true;
}

void ScreamyBall::DrawBall() {

}
void ScreamyBall::DrawObstacles() {
/*
 * If there's no obstacle on the screen, then create an obstacle.
 * If there's an obstacle on the screen, don't create a new obstacle.
 *  However, do make it change its position to be closer to the ball.
 *  Also, you can create obstacles of multiple sizes.
 *
 *  There should be two types of obstacles: high and low.
 *  You have to jump over the high ones and duck beneath the low ones.
 *  Maybe we can randomize their appearance.
 *
 */
}

void ScreamyBall::keyDown(KeyEvent event) {
  switch (event.getCode()) {
    case KeyEvent::KEY_UP:
    case KeyEvent::KEY_k:
    case KeyEvent::KEY_w: {
      engine_.state_ = BallState::kJumping;
      break;
    }
    case KeyEvent::KEY_DOWN:
    case KeyEvent::KEY_j:
    case KeyEvent::KEY_s: {
      engine_.state_ = BallState::kDucking;
      break;
    }

    case KeyEvent::KEY_p: {
      paused_ = !paused_;

//      if (paused_) {
//        last_pause_time_ = system_clock::now();
//      } else {
//        last_intact_time_ += system_clock::now() - last_pause_time_;
//      }
      break;
    }
    case KeyEvent::KEY_r: {
      ConfirmReset();
      ResetGame();
      break;
    }
  }
}

bool ScreamyBall::ConfirmReset() {
  const cinder::vec2 center = getWindowCenter();
  const cinder::ivec2 size = {500, 50};
  const Color color = Color::black();
  const ColorA bg_color = ColorA::gray(0.5);
  PrintText("Do you really want to reset the game?", color, size, center,
      bg_color);

  return false;
}

void ScreamyBall::ResetGame() {
  if (!ConfirmReset()) return;
  engine_.Reset();
  paused_ = false;
  printed_game_over_ = false;
  state_ = GameState::kPlaying;
  //time_left_ = 0;
  //top_players_.clear();
  //current_player_top_scores_.clear();
}

}  // namespace screamyball_app
