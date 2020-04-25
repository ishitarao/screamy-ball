// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include "screamy_ball.h"

#include <cinder/app/App.h>
#include <cinder/Font.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <gflags/gflags.h>
#include <sphinx/Recognizer.hpp>


namespace screamyball_app {

using cinder::Color;
using cinder::ColorA;
using cinder::TextBox;
using cinder::app::KeyEvent;
using ci::fs::path;
using screamy_ball::BallState;
using screamy_ball::Location;
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
DECLARE_double(delay_secs);

ScreamyBall::ScreamyBall()
    : engine_({2, static_cast<int>(FLAGS_height - 2)},
        FLAGS_width, FLAGS_height),
      printed_game_over_{false},
      paused_{false},
      state_{GameState::kPlaying},
      tile_size_(FLAGS_tilesize),
      height_(FLAGS_height),
      width_(FLAGS_width),
      delay_secs_(FLAGS_delay_secs),
      last_time_(0.00) {}

void ScreamyBall::setup() {
  cinder::gl::enableDepthWrite();
  cinder::gl::enableDepthRead();
  timer_.start();

  ci::fs::path hmm_path  = ci::app::getAssetPath( "en-us" );
  ci::fs::path dict_path = ci::app::getAssetPath( "cmudict-en-us.dict" );
  ci::fs::path lm_path   = ci::app::getAssetPath( "demo.jsgf" );
  sphinx::RecognizerRef recognizer =
      sphinx::Recognizer::create( hmm_path.string(), dict_path.string() );
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
  const double current_time = timer_.getSeconds();
  if (current_time - last_time_ >= delay_secs_) {
    engine_.Roll();
    engine_.CreateObstacle();
    last_time_ = current_time;
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
  // TODO: Get rid of the magic numbers and make this look more readable
  const Location loc = engine_.GetBall().location_;
  cinder::gl::color(Color(1, 0, 0));

  if (engine_.state_ == BallState::kDucking) {
    const cinder::ivec2 ellipse_center = {loc.Row() * tile_size_ + tile_size_ / 2,
                                          loc.Col() * tile_size_ - (tile_size_) / 8};
    cinder::gl::drawSolidEllipse(ellipse_center,
        ((float)tile_size_ / 2), ((float)tile_size_ / 8));
  } else {
    const cinder::ivec2 circle_center = {loc.Row() * tile_size_ + tile_size_ / 2,
                                         loc.Col() * tile_size_ - tile_size_ / 2};
    cinder::gl::drawSolidCircle(circle_center, ((float)tile_size_ / 2));
  }

}

void ScreamyBall::DrawObstacles() {
  screamy_ball::Obstacle obstacle = engine_.GetObstacle();
  Location loc = obstacle.location_;
  const int obstacle_height = obstacle.GetHeight();
  cinder::gl::color(Color::gray(0.5));

  //create spikes
  for (int counter = 0; counter < obstacle.length_; counter++) {
    const cinder::ivec2 tri_pt_1 = {loc.Row() * tile_size_,
                                    loc.Col() * tile_size_};
    const cinder::ivec2 tri_pt_2 = {(loc.Row() - 1) * tile_size_,
                                    loc.Col() * tile_size_};
    const cinder::ivec2 tri_pt_3 = {(loc.Row() - (0.5)) * tile_size_,
                                    (loc.Col() - obstacle_height) * tile_size_};

    cinder::gl::drawSolidTriangle(tri_pt_1, tri_pt_2, tri_pt_3);
    loc = {loc.Row() + 1, loc.Col()};
  }
}

void ScreamyBall::keyDown(KeyEvent event) {
  switch (event.getCode()) {
    case KeyEvent::KEY_UP:
    case KeyEvent::KEY_k:
    case KeyEvent::KEY_w: {
      if (paused_) {
        return;
      }
      engine_.state_ = BallState::kJumping;
      break;
    }
    case KeyEvent::KEY_DOWN:
    case KeyEvent::KEY_j:
    case KeyEvent::KEY_s: {
      if (paused_ || engine_.state_ == BallState::kJumping) {
        return;
      }
      engine_.state_ = BallState::kDucking;
      break;
    }

    case KeyEvent::KEY_p: {
      paused_ = !paused_;

      break;
    }
    case KeyEvent::KEY_r: {
      ConfirmReset();
      ResetGame();
      break;
    }
  }
}

void ScreamyBall::keyUp(KeyEvent event) {
  switch(event.getCode()) {
    case KeyEvent::KEY_DOWN:
    case KeyEvent::KEY_j:
    case KeyEvent::KEY_s: {
      engine_.state_ = BallState::kRolling;
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
