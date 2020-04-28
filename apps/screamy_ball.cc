// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include "screamy_ball.h"
#include <cinder/Font.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/app/App.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <gflags/gflags.h>

namespace screamyball_app {

using cinder::Color;
using cinder::ColorA;
using cinder::TextBox;
using cinder::app::KeyEvent;
using cinder::app::MouseEvent;
using cinder::params::InterfaceGl;
using ci::fs::path;
using screamy_ball::BallState;
using screamy_ball::Location;
using std::string;
using std::bind;

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
      printed_game_over_(false),
      paused_(false),
      confirmed_reset_(false),
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
  SetupRecognizer();

}

void ScreamyBall::SetupRecognizer() {
  ci::fs::path hmm_path  = ci::app::getAssetPath("en-us");
  ci::fs::path dict_path = ci::app::getAssetPath("cmudict-en-us.dict");
  ci::fs::path keyword_path   = ci::app::getAssetPath("key.txt");

  //set up the language and the dictionary
  recognizer_ = sphinx::Recognizer::create(hmm_path.string(), dict_path.string());

  //event handler for whenever speech is detected
  recognizer_->connectEventHandler(std::bind(
      &ScreamyBall::RecognizeCommands, this, std::placeholders::_1));

  //create and add a model based on the text file
  recognizer_->addModelJsgf("keyword", keyword_path.string(),
                            true);
  recognizer_->start();
}

void ScreamyBall::SetupUI() {
  //set up main menu UI:
  menu_ui_ = InterfaceGl::create(getWindow(), "Main Menu",
                                 cinder::app::toPixels(cinder::ivec2(200, 400)));

  menu_ui_->addButton( "Start", [&]() { state_ = GameState::kPlaying; } );
  menu_ui_->addButton( "Help", std::bind( &ScreamyBall::update, this ) );


  //mParams->addButton( "Leaderboard", bind( &TweakBarApp::button, this ) );


  //set up in-game UI:
  in_game_ui_ = InterfaceGl::create(getWindow(), "In-Game UI",
                                    cinder::app::toPixels(cinder::ivec2(200, 400)));
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
  if (state_ == GameState::kGameOver || state_ == GameState::kConfirmingReset) {
    timer_.stop();
    if (confirmed_reset_) {
      ResetGame();
    }
    return;
  }
  if (paused_) return;
  if (timer_.isStopped()) {
    timer_.resume();
  }
  const double current_time = timer_.getSeconds();
  if (current_time - last_time_ >= delay_secs_) {
    engine_.Run();
    if (engine_.state_ == BallState::kCollided) {
      state_ = GameState::kGameOver;
    }
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

  DrawBackground();
  DrawButtons();
  DrawBall();
  DrawObstacles();
  if (state_ == GameState::kConfirmingReset) {
    ConfirmReset();
  }

}

template <typename C>
void PrintText(const string& text, int font_size, const C& color,
               const cinder::ivec2& size, const cinder::vec2& loc,
               const ColorA& bg_color = ColorA::zero()) {
  cinder::gl::color(color);

  auto box = TextBox()
      .alignment(TextBox::CENTER)
      .font(cinder::Font(kNormalFont, font_size))
      .size(size)
      .color(color)
      .backgroundColor(bg_color)
      .text(text);

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {loc.x - box_size.x / 2.0, loc.y - box_size.y / 2.0};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);
}

void ScreamyBall::DrawMainMenu() {
  DrawBackground();
  /*
   * Print "Screamy Ball"
   * Options: Play Game, Help pl0x
   */

}

void ScreamyBall::DrawBackground() {
  cinder::gl::clear(Color::black());

  // draw the ground:
  int ground_height = engine_.GetMinHeight();
  cinder::gl::color(Color::white());
  const cinder::ivec2 upper_left = {0, tile_size_ * ground_height};
  const cinder::ivec2 bottom_right = {width_ * tile_size_, tile_size_ * height_};
  cinder::gl::drawSolidRect(cinder::Rectf(upper_left, bottom_right));
}

void ScreamyBall::DrawButtons() {
  const int font_size = 25;
  const cinder::ivec2 size = {tile_size_ * 2, font_size + 10}; //10 is a buffer
  const Color text_color = Color::white();

  const cinder::vec2 pause_button_loc = {tile_size_ * 1.5, tile_size_};
  const cinder::vec2 reset_button_loc = {tile_size_ * 4, tile_size_};
  const ColorA pause_button_color = ColorA(1, 0, 0, 0.75);
  const ColorA reset_button_color = ColorA(0, 1, 0, 0.75);

  PrintText("Pause", font_size, text_color, size, pause_button_loc, pause_button_color);
  PrintText("Reset", font_size, text_color, size, reset_button_loc, reset_button_color);
}

void ScreamyBall::DrawGameOver() {
  // Lazily print.
  if (printed_game_over_) return;

  const cinder::vec2 center = getWindowCenter();
  const cinder::ivec2 size = {500, 50};
  const Color color = Color::white();
  string elapsed_time = PrettyPrintElapsedTime(timer_.getSeconds());
  PrintText("Your time: " + elapsed_time, 30, color, size, center);

  printed_game_over_ = true;
}

void ScreamyBall::DrawBall() {
  // TODO: Get rid of the magic numbers and make this look more readable
  const Location loc = engine_.GetBall().location_;
  cinder::gl::color(Color(1, 0, 0));

  if (engine_.state_ == BallState::kDucking) {
    const cinder::ivec2 ellipse_center = {(loc.Row() + 0.5) * tile_size_,
                                          (loc.Col() - 0.125) * tile_size_};
    cinder::gl::drawSolidEllipse(ellipse_center,
        ((float)tile_size_ * 0.5f), ((float)tile_size_ * 0.125f));
  } else {
    const cinder::ivec2 circle_center = {(loc.Row() + 0.5) * tile_size_,
                                         (loc.Col() - 0.5) * tile_size_};
    cinder::gl::drawSolidCircle(circle_center, ((float)tile_size_ * 0.5f));
  }

}


void ScreamyBall::DrawObstacles() {
  screamy_ball::Obstacle obstacle = engine_.GetObstacle();
  Location loc = obstacle.location_;
  const int obstacle_height = obstacle.GetHeight();
  cinder::gl::color(Color::gray(0.5));

  //create spikes
  for (int counter = 0; counter < obstacle.length_; counter++) {
    cinder::ivec2 tri_pt_1 = {loc.Row() * tile_size_,
                              loc.Col() * tile_size_};
    cinder::ivec2 tri_pt_2 = {(loc.Row() - 1) * tile_size_,
                              loc.Col() * tile_size_};

    cinder::ivec2 tri_pt_3 = {(loc.Row() - 0.5) * tile_size_,
                              (loc.Col() - obstacle_height) * tile_size_};

    if (obstacle.GetObstacleType() == screamy_ball::ObstacleType::kHigh) {
      tri_pt_1.y += tile_size_ / 2;
      tri_pt_2.y += tile_size_ / 2;
      tri_pt_3.y = (loc.Col() + obstacle_height) * tile_size_ + tile_size_ / 2;
    }

    cinder::gl::drawSolidTriangle(tri_pt_1, tri_pt_2, tri_pt_3);
    loc = {loc.Row() + 1, loc.Col()};
  }

}

void ScreamyBall::RecognizeCommands(const std::string& message) {
  if (message == "higher") {
    ParseUserInteraction(KeyEvent::KEY_UP);
  } else if (message == "lower") {
    ParseUserInteraction(KeyEvent::KEY_DOWN);
  } else if (message == "pause game") {
    ParseUserInteraction(KeyEvent::KEY_p);
  } else if (message == "reset game") {
    ParseUserInteraction(KeyEvent::KEY_r);
  }
}

void ScreamyBall::keyDown(KeyEvent event) {
  ParseUserInteraction(event.getCode());
}

void ScreamyBall::ParseUserInteraction(int event_code) {
  switch (event_code) {
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
      if (paused_) {
        timer_.stop();
      } else {
        timer_.resume();
      }
      break;
    }
    case KeyEvent::KEY_r: {
      state_ = GameState::kConfirmingReset;
      break;
    }
    case KeyEvent::KEY_y: {
      if (state_ == GameState::kConfirmingReset) {
        confirmed_reset_ = true;
      }
      break;
    }
    case KeyEvent::KEY_n: {
      if (state_ == GameState::kConfirmingReset) {
        state_ = GameState::kPlaying;
      }
    }
    default: break;
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

void ScreamyBall::mouseDown(MouseEvent event) {
  if (event.isShiftDown()) {
    if (event.isLeftDown()) {
      ParseUserInteraction(KeyEvent::KEY_p);
    } else if (event.isRightDown()) {
      ParseUserInteraction(KeyEvent::KEY_r);
    }
  } else {
    if (event.isLeftDown()) {
      //if (event.getPos() )

      ParseUserInteraction(KeyEvent::KEY_UP);
    } else if (event.isRightDown()) {
      ParseUserInteraction(KeyEvent::KEY_DOWN);
    }
  }

  //TODO: Create pause and reset buttons, and finish confirm reset with mouse clicks and event.getPos()
}

void ScreamyBall::mouseUp(MouseEvent event) {
  if (event.isRightDown()) {
    engine_.state_ = BallState::kRolling;
  }
}

void ScreamyBall::ConfirmReset() {
  const cinder::vec2 center = getWindowCenter();
  const cinder::ivec2 size = {500, 60};
  const Color color = Color::white();
  const ColorA bg_color = ColorA::gray(0.5);
  PrintText("Do you really want to reset the game? Press y for yes, and n for no.", 30, color, size, center,
      bg_color);
}

void ScreamyBall::ResetGame() {
  if (!confirmed_reset_) {
    state_ = GameState::kPlaying;
    return;
  }
  engine_.Reset();
  paused_ = false;
  printed_game_over_ = false;
  confirmed_reset_ = false;
  state_ = GameState::kPlaying;
  //time_left_ = 0;
  //top_players_.clear();
  //current_player_top_scores_.clear();
}

}  // namespace screamyball_app
