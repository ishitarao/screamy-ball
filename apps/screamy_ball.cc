// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include "screamy_ball.h"
#include <cinder/Font.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/app/App.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <gflags/gflags.h>
#include <cmath>

namespace screamyball_app {

using cinder::Color;
using cinder::ColorA;
using cinder::TextBox;
using cinder::ivec2;
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
const char kDifferentFont[] = "Courier New Bold";
#endif

DECLARE_uint32(width);
DECLARE_uint32(height);
DECLARE_uint32(tilesize);
DECLARE_double(delay_secs);

ScreamyBall::ScreamyBall()
    : kTileSize(FLAGS_tilesize),
      kHeight(FLAGS_height),
      kWidth(FLAGS_width),
      kDefaultFontSize(30),
      kTextBoxBuffer(10),
      kLocMultiplier(0.5),
      engine_({2, static_cast<int>(FLAGS_height - 2)},
        FLAGS_width, FLAGS_height),
      state_{GameState::kMenu},
      printed_game_over_(false),
      paused_(false),
      confirmed_reset_(false),
      delay_secs_(FLAGS_delay_secs),
      last_update_secs_(0.00) {}

/* ------------------------------Set Up-------------------------------------- */

void ScreamyBall::setup() {
  cinder::gl::enableDepthWrite();
  cinder::gl::enableDepthRead();
  SetupRecognizer();
  SetupMainMenuUI();
  SetupInGameUI();
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

void ScreamyBall::SetupInGameUI() {
  in_game_ui_ = InterfaceGl::create(getWindow(), "Options",
                                    cinder::app::toPixels(
                                        ivec2(200, kTileSize * 5)));

  in_game_ui_->addButton("Menu",[&]() {
    last_state_ = state_;
    state_ = GameState::kMenu; });

  in_game_ui_->addButton("Jump", [&]() {
    ParseUserInteraction(KeyEvent::KEY_UP); });

  in_game_ui_->addButton("Duck", [&]() {
    if (engine_.state_ == BallState::kDucking) {
      engine_.state_ = BallState::kRolling;
    } else {
      engine_.state_ = BallState::kDucking;
    } });

  in_game_ui_->addButton("Pause",[&]() { paused_ = !paused_; });

  in_game_ui_->addButton("Reset",[&]() {
    last_state_ = state_;
    state_ = GameState::kConfirmingReset; });
}

void ScreamyBall::SetupMainMenuUI() {
  menu_ui_ = InterfaceGl::create(getWindow(), "Main Menu",
                                 cinder::app::toPixels(
                                     ivec2(200, kTileSize * 3)));

  //Start button: fires a lambda that starts the timer and the game when pressed
  menu_ui_->addButton("Start",[&]() {
    engine_.Reset();
    last_state_ = state_;
    state_ = GameState::kPlaying;
    timer_.start(); });

  menu_ui_->addButton("Help",[&]() {
    last_state_ = state_;
    state_ = GameState::kHelp; });

  menu_ui_->addButton("Reset",[&]() {
    last_state_ = state_;
    state_ = GameState::kConfirmingReset; });

  //mParams->addButton( "Leaderboard", bind( &TweakBarApp::button, this ) );
}

/* --------------------------------Update------------------------------------ */

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
  switch (state_) {
    case GameState::kPlaying: {
      if (paused_) return;
      if (timer_.isStopped()) {
        if (last_state_ == GameState::kMenu) {
          timer_.start();
        } else timer_.resume();
      }

      const double current_time = timer_.getSeconds();
      if (current_time - last_update_secs_ >= delay_secs_) {
        engine_.Run();
        if (engine_.state_ == BallState::kCollided) {
          last_state_ = state_;
          state_ = GameState::kGameOver;
        }
        last_update_secs_ = current_time;
      }
      break;
    }

    case GameState::kConfirmingReset: {
      timer_.stop();
      if (confirmed_reset_) {
        ResetGame();
      }
      return;
    }

    default: {
      timer_.stop();
      return;
    }
  }
}

/* ----------------------------------Draw------------------------------------ */

void ScreamyBall::draw() {
  cinder::gl::enableAlphaBlending();

  menu_ui_->hide();
  in_game_ui_->setOptions("Pause", "visible=false");
  in_game_ui_->setOptions("Jump", "visible=false");
  in_game_ui_->setOptions("Duck", "visible=false");
  in_game_ui_->show();

  switch (state_) {
    case GameState::kGameOver: {
      DrawGameOver();
      break;
    }
    case GameState::kMenu: {
      menu_ui_->show();
      in_game_ui_->hide();
      DrawMainMenu();
      break;
    }
    case GameState::kHelp: {
      DrawHelp();
      break;
    }
    case GameState::kConfirmingReset: {
      in_game_ui_->hide();
      DrawConfirmReset();
      break;
    }
    case GameState::kPlaying: {
      in_game_ui_->setOptions("Pause", "visible=true");
      in_game_ui_->setOptions("Jump", "visible=true");
      in_game_ui_->setOptions("Duck", "visible=true");
      if (paused_) return;
      DrawBackground();
      DrawBall();
      DrawObstacles();
    }
  }

  menu_ui_->draw();
  in_game_ui_->draw();

}

template <typename C>
void PrintText(const string& text, float font_size, const C& color,
               const ivec2& size, const cinder::vec2& loc,
               const ColorA& bg_color = ColorA::zero()) {
  cinder::gl::color(color);

  auto box = TextBox()
      .alignment(TextBox::CENTER)
      .font(cinder::Font(kDifferentFont, font_size))
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
  const ivec2 center = getWindowCenter();
  const ivec2 size = {kTileSize * 4, kTileSize * 2};
  const Color color = Color::white();
  PrintText("Screamy Ball", kDefaultFontSize, color, size, center);

  menu_ui_->setPosition({center.x - kTileSize * 2,
                         center.y + kTileSize});
  menu_ui_->setSize(size);
}

void ScreamyBall::DrawHelp() {
  cinder::gl::clear(Color::black());
  const ivec2 pos = {getWindowCenter().x, getWindowPosY()};
  const ivec2 size = {kWidth * kTileSize, kTileSize};
  const Color color = Color::white();
  float_t row = 0;

  ci::fs::path help_path = ci::app::getAssetPath("help.txt");
  auto input_stream = cinder::loadFileStream(help_path);

  PrintText(input_stream->readLine(), kDefaultFontSize, color, size,
            {pos.x, pos.y + (++row) * kTileSize});

  row++;
  while (!input_stream->isEof()) {
    PrintText(input_stream->readLine(), 20, color, size,
        {pos.x, pos.y + row * kTileSize});
    row++;
  }
}

void ScreamyBall::DrawBackground() {
  cinder::gl::clear(Color::black());

  // draw the ground:
  int ground_height = engine_.GetMinHeight();
  cinder::gl::color(Color::white());
  const ivec2 upper_left = {0, kTileSize * ground_height};
  const ivec2 bottom_right = {kWidth * kTileSize, kTileSize * kHeight};
  cinder::gl::drawSolidRect(cinder::Rectf(upper_left, bottom_right));
}

void ScreamyBall::DrawBall() {
  const Location loc = engine_.GetBall().location_;
  const float multiplier_cubed = pow(kLocMultiplier, 3);
  const float center_x = (loc.Row() + kLocMultiplier) * kTileSize;
  const float radius_x = (float)kTileSize * kLocMultiplier;
  cinder::gl::color(Color(1, 0, 0));

  if (engine_.state_ == BallState::kDucking) {
    const ivec2 ellipse_center = { center_x, (loc.Col() - multiplier_cubed)
                                          * kTileSize };
    cinder::gl::drawSolidEllipse(ellipse_center, radius_x,
        ((float)kTileSize * multiplier_cubed));
  } else {
    const ivec2 circle_center = { center_x, (loc.Col() - kLocMultiplier)
                                         * kTileSize };
    cinder::gl::drawSolidCircle(circle_center, radius_x);
  }

}

void ScreamyBall::DrawObstacles() {
  screamy_ball::Obstacle obstacle = engine_.GetObstacle();
  Location loc = obstacle.location_;
  const int obstacle_height = obstacle.GetHeight();
  const float loc_incr = kTileSize * kLocMultiplier;

  cinder::gl::color(Color::gray(0.5)); //the % of grey

  //create spikes
  for (int counter = 0; counter < obstacle.length_; counter++) {
    ivec2 tri_pt_1 = {loc.Row() * kTileSize, loc.Col() * kTileSize};
    ivec2 tri_pt_2 = {(loc.Row() - 1) * kTileSize,
                      loc.Col() * kTileSize};

    ivec2 tri_pt_3 = {(loc.Row() - kLocMultiplier) * kTileSize,
                              (loc.Col() - obstacle_height) * kTileSize};

    if (obstacle.GetObstacleType() == screamy_ball::ObstacleType::kHigh) {
      tri_pt_1.y += loc_incr;
      tri_pt_2.y += loc_incr;
      tri_pt_3.y = (float)(loc.Col() + obstacle_height) * kTileSize + loc_incr;
    }

    cinder::gl::drawSolidTriangle(tri_pt_1, tri_pt_2, tri_pt_3);
    loc = {loc.Row() + 1, loc.Col()};
  }
}

void ScreamyBall::DrawGameOver() {
  // Lazily print.
  if (printed_game_over_) return;

  cinder::gl::clear(Color::black());
  const cinder::vec2 center = getWindowCenter();
  const ivec2 size = {500, 50};
  const Color color = Color::white();
  string elapsed_time = PrettyPrintElapsedTime(timer_.getSeconds());
  PrintText("Your time: " + elapsed_time, kDefaultFontSize, color, size, center);

  printed_game_over_ = true;
}

void ScreamyBall::DrawConfirmReset() {
  cinder::gl::clear(Color::black());
  const cinder::vec2 center = getWindowCenter();
  //2 is to signify 2 lines:
  const ivec2 size = {500, kDefaultFontSize * 2 + kTextBoxBuffer};
  const Color color = Color::black();
  const ColorA bg_color = ColorA::gray(0.75);

  PrintText("Do you really want to reset the game? "
            "Press y for yes, and n for no.", kDefaultFontSize, color, size,
            center, bg_color);
}

/* --------------------------User Interaction-------------------------------- */

void ScreamyBall::RecognizeCommands(const std::string& message) {
  if (message == "higher") {
    ParseUserInteraction(KeyEvent::KEY_UP);
  } else if (message == "lower") {
    ParseUserInteraction(KeyEvent::KEY_DOWN);
  } else if (message == "pause game") {
    ParseUserInteraction(KeyEvent::KEY_p);
  } else if (message == "reset game") {
    ParseUserInteraction(KeyEvent::KEY_r);
  } else if (message == "main menu") {
    ParseUserInteraction(KeyEvent::KEY_m);
  } else if (message == "help me") {
    ParseUserInteraction(KeyEvent::KEY_h);
  }
}

void ScreamyBall::keyDown(KeyEvent event) {
  ParseUserInteraction(event.getCode());
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
      ParseUserInteraction(KeyEvent::KEY_UP);
    } else if (event.isRightDown()) {
      ParseUserInteraction(KeyEvent::KEY_DOWN);
    }
  }
}

void ScreamyBall::mouseUp(MouseEvent event) {
  if (event.isRightDown()) {
    engine_.state_ = BallState::kRolling;
  }
}

void ScreamyBall::ParseUserInteraction(int event_code) {
  switch (event_code) {
    case KeyEvent::KEY_UP:
    case KeyEvent::KEY_k:
    case KeyEvent::KEY_w: {
      if (paused_ || state_ != GameState::kPlaying) {
        return;
      }
      engine_.state_ = BallState::kJumping;
      break;
    }

    case KeyEvent::KEY_DOWN:
    case KeyEvent::KEY_j:
    case KeyEvent::KEY_s: {
      if (paused_ || engine_.state_ == BallState::kJumping
        || state_ != GameState::kPlaying) {
        return;
      }
      engine_.state_ = BallState::kDucking;
      break;
    }

    case KeyEvent::KEY_p: {
      if (state_ != GameState::kPlaying) {
        return;
      }
      paused_ = !paused_;
      if (paused_) {
        timer_.stop();
      } else {
        timer_.resume();
      }
      break;
    }
    case KeyEvent::KEY_m: {
      last_state_ = state_;
      state_ = GameState::kMenu;
      timer_.stop();
      engine_.Reset();
      break;
    }
    case KeyEvent::KEY_h: {
      last_state_ = state_;
      state_ = GameState::kHelp;
      timer_.stop();
      break;
    }

    case KeyEvent::KEY_r: {
      last_state_ = state_;
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
        state_ = last_state_;
      }
      break;
    }
    default: break;
  }
}

/* --------------------------------Reset------------------------------------- */

void ScreamyBall::ResetGame() {
  engine_.Reset();
  paused_ = false;
  printed_game_over_ = false;
  confirmed_reset_ = false;
  last_state_ = state_;
  state_ = GameState::kMenu;
  timer_.stop();
  last_update_secs_ = 0.00;
  //top_players_.clear();
  //current_player_top_scores_.clear();
}

}  // namespace screamyball_app
