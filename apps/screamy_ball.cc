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
using cinder::TextBox;
using cinder::app::KeyEvent;
using cinder::app::MouseEvent;
using cinder::app:toPixels;
using cinder::params::InterfaceGl;
using ci::fs::path;
using screamy_ball::BallState;
using screamy_ball::Location;


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
      kUiDimensions({ FLAGS_tilesize * 4, FLAGS_tilesize * 5}),
      engine_({2, static_cast<int>(FLAGS_height - 2)},
        FLAGS_width, FLAGS_height),
      state_(GameState::kMenu),
      last_state_(GameState::kMenu),
      paused_(false),
      confirmed_reset_(false),
      delay_secs_(FLAGS_delay_secs),
      last_update_secs_(0.00),
      timer_(false) {}

/* ------------------------------Set Up-------------------------------------- */

/**
 * Cinder's standard setup function to set up the initial state of the game.
 */
void ScreamyBall::setup() {
  cinder::gl::enableDepthWrite();
  cinder::gl::enableDepthRead();
  SetupRecognizer();
  SetupMainMenuUI();
  SetupInGameUI();
  SetupGeneralUI();
}

/**
 * Initialises ciSpeech's speech recognizer to 'keyword' mode.
 */
void ScreamyBall::SetupRecognizer() {
  ci::fs::path hmm_path  = ci::app::getAssetPath("en-us");
  ci::fs::path dict_path = ci::app::getAssetPath(
      "cmudict-en-us.dict");
  ci::fs::path keyword_path   = ci::app::getAssetPath("key.txt");

  //set up the language and the dictionary
  recognizer_ = sphinx::Recognizer::create(hmm_path.string(),
      dict_path.string());

  //event handler for whenever speech is detected
  recognizer_->connectEventHandler(std::bind(
      &ScreamyBall::RecognizeCommands, this, std::placeholders::_1));

  //create and add a model based on the text file
  recognizer_->addModelJsgf("keyword", keyword_path.string(),
                            true);
  recognizer_->start();
}

/**
 * Initialises in-game UI, which will be displayed everywhere except the main
 * menu.
 */
void ScreamyBall::SetupInGameUI() {
  in_game_ui_ = InterfaceGl::create(getWindow(), "Game Options",
                                    toPixels(kUiDimensions));

  // lambda to change the game state to menu when the menu button is pressed
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

/**
 * Initializes the Main Menu UI.
 */
void ScreamyBall::SetupMainMenuUI() {
  menu_ui_ = InterfaceGl::create(getWindow(), "Main Menu",
                                 toPixels(kUiDimensions));

  //Start button: fires a lambda that starts the timer and the game when pressed
  menu_ui_->addButton("Start",[&]() {
    ResetGame();
    last_state_ = state_;
    state_ = GameState::kPlaying; });

  menu_ui_->addButton("Help",[&]() {
    last_state_ = state_;
    state_ = GameState::kHelp; });

  menu_ui_->addButton("Reset",[&]() {
    last_state_ = state_;
    state_ = GameState::kConfirmingReset; });
}

void ScreamyBall::SetupGeneralUI() {
  general_ui_ = InterfaceGl::create(getWindow(), "Options",
                                 toPixels(kUiDimensions));

  // lambda to change the game state to menu when the menu button is pressed
  general_ui_->addButton("Menu",[&]() {
    last_state_ = state_;
    state_ = GameState::kMenu; });

  general_ui_->addButton("Reset",[&]() {
    last_state_ = state_;
    state_ = GameState::kConfirmingReset; });
}

/* --------------------------------Update------------------------------------ */

/**
 * Cinder's standard update function.
 */
void ScreamyBall::update() {
  switch (state_) {
    case GameState::kPlaying: {
      if (paused_) {
        return;
      }

      if (timer_.isStopped()) {
        if (last_state_ == GameState::kMenu) {
          timer_.start();
        } else timer_.resume();
      }
      RunEngine();
      break;
    }

    case GameState::kConfirmingReset: {
      timer_.stop();
      if (confirmed_reset_) {
        ResetGame();
      }
      return;
    }
    //in all other cases, there is nothing to update.
    default: {
      if (!timer_.isStopped()) {
        timer_.stop();
      }
      return;
    }
  }
}

/**
 * Calls the engine's Run function after the specified
 * number of delay seconds have passed (to control game speed)
 */
void ScreamyBall::RunEngine() {
  const double current_time = timer_.getSeconds();
  if (current_time - last_update_secs_ >= delay_secs_) {
    engine_.Run();
    last_update_secs_ = current_time;
    //change the game state to Game Over if a collision is detected
    if (engine_.state_ == BallState::kCollided) {
      last_state_ = state_;
      state_ = GameState::kGameOver;
      last_update_secs_ = 0.00;
    }
  }
}

/* ----------------------------------Draw------------------------------------ */

/**
 * Cinder's standard draw function to generate graphics for the game.
 */
void ScreamyBall::draw() {
  cinder::gl::enableAlphaBlending();

  menu_ui_->hide();
  in_game_ui_->hide();
  general_ui_->show();

  switch (state_) {
    case GameState::kGameOver: {
      DrawGameOver();
      break;
    }
    case GameState::kMenu: {
      menu_ui_->show();
      general_ui_->hide();
      DrawMainMenu();
      break;
    }
    case GameState::kHelp: {
      DrawHelp();
      break;
    }
    case GameState::kConfirmingReset: {
      DrawConfirmReset();
      break;
    }

    case GameState::kPlaying: {
      in_game_ui_->show();
      general_ui_->hide();
      if (paused_) {
        return;
      }
      DrawBackground();
      DrawBall();
      DrawObstacles();
    }
  }

  menu_ui_->draw();
  in_game_ui_->draw();
  general_ui_->draw();
}

/**
 * Generates a text box with the given parameters.
 * @tparam C Text Color: can be a Color or a ColorA
 * @param text the text to print
 * @param font_size the size of the text
 * @param text_color the color of the text
 * @param size the size of the text box
 * @param loc the position of the text box
 */
template <typename C>
void ScreamyBall::PrintText(const string& text, float font_size,
    const C& text_color, const ivec2& size, const cinder::vec2& loc) {
  cinder::gl::color(text_color);

  auto box = TextBox()
      .alignment(TextBox::CENTER)
      .font(cinder::Font(kDifferentFont, font_size))
      .size(size)
      .color(text_color)
      .backgroundColor(ColorA::zero())
      .text(text);

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {loc.x - box_size.x * kLocMultiplier,
                             loc.y - box_size.y * kLocMultiplier};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);
}

/**
 * Draws the main menu.
 */
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

/**
 * Draws the help page.
 */
void ScreamyBall::DrawHelp() {
  cinder::gl::clear(Color::black());
  const ivec2 pos = {getWindowCenter().x, getWindowPosY()};
  const ivec2 size = {kWidth * kTileSize, kTileSize};
  const Color color = Color::white();
  size_t row = 0;

  auto input_stream = cinder::loadFileStream(ci::app::getAssetPath(
      "help.txt"));

  PrintText(input_stream->readLine(), kDefaultFontSize, color, size, pos);

  while (!input_stream->isEof()) {
    // The font is half the tile size
    PrintText(input_stream->readLine(),
        ((float)(kTileSize - kTextBoxBuffer) / 2), color, size,
        {pos.x, pos.y + (++row) * kTileSize});
  }
}

/**
 * Draws the in-game background.
 */
void ScreamyBall::DrawBackground() {
  cinder::gl::clear(Color::black());

  // draw the ground:
  int ground_height = engine_.GetMinHeight();
  cinder::gl::color(Color::white());
  const ivec2 upper_left = {0, kTileSize * ground_height};
  const ivec2 bottom_right = {kWidth * kTileSize, kTileSize * kHeight};
  cinder::gl::drawSolidRect(cinder::Rectf(upper_left, bottom_right));
}

/**
 * Draws the ball in two states: normally, and while ducking.
 */
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

/**
 * Draws the obstacles, depending on type: 'high' obstacles are the ones
 * the ball is supposed to duck from, while 'low' obstacles are the ones
 * the ball is supposed to jump over.
 */
void ScreamyBall::DrawObstacles() {
  screamy_ball::Obstacle obstacle = engine_.GetObstacle();
  Location loc = obstacle.location_;
  const int obstacle_height = obstacle.kHeight;
  const float loc_incre = kTileSize * kLocMultiplier;

  cinder::gl::color(Color::gray(0.5)); // 0.5 is the % of grey

  //create spikes
  for (int counter = 0; counter < obstacle.length_; counter++) {

    // points of a triangle for a 'low' obstacle
    ivec2 point_1 = {loc.Row() * kTileSize, loc.Col() * kTileSize};
    ivec2 point_2 = {(loc.Row() - 1) * kTileSize,
                      loc.Col() * kTileSize};

    ivec2 point_3 = {(loc.Row() - kLocMultiplier) * kTileSize,
                              (loc.Col() - obstacle_height) * kTileSize};

    // increment location if it's a 'high' obstacle
    if (obstacle.GetObstacleType() == screamy_ball::ObstacleType::kHigh) {
      point_1.y += loc_incre;
      point_2.y += loc_incre;
      point_3.y = (float) (loc.Col() + obstacle_height) * kTileSize + loc_incre;
      cinder::gl::drawSolidRect(cinder::Rectf(point_1,
          {point_2.x, 0}));
    }

    cinder::gl::drawSolidTriangle(point_1, point_2, point_3);

    // update location to draw another spike
    loc = { loc.Row() + 1, loc.Col() };
  }
}

/**
 * Pretty Prints the input time in seconds in the format hh:mm:ss.
 * @param time_secs the time in seconds to pretty print
 * @return the elapsed time in the above format
 */
string PrettyPrintElapsedTime(double time_secs) {
  int seconds = (int) time_secs;

  int hours = seconds / (60 * 60);
  seconds -= hours * (60 * 60);

  int minutes = seconds / 60;
  seconds -= minutes * 60;

  std::stringstream sstream;
  sstream << hours << ':' << minutes << ':' << seconds;
  return sstream.str();
}

/**
 * Draws the Game Over page.
 */
void ScreamyBall::DrawGameOver() {
  cinder::gl::clear(Color::black());
  const cinder::vec2 center = getWindowCenter();
  const ivec2 size = {kTileSize * 10, kTileSize};
  const Color color = Color::white();
  string elapsed_time = PrettyPrintElapsedTime(timer_.getSeconds());
  PrintText("Your time: " + elapsed_time, kDefaultFontSize, color, size,
      center);
}

/**
 * Draws the page that asks the user to confirm if they want to reset or not.
 */
void ScreamyBall::DrawConfirmReset() {
  cinder::gl::clear(Color::black());
  const cinder::vec2 center = getWindowCenter();
  //Multiplying font size by 4 since there will be 4 lines of text:
  const ivec2 size = {kTileSize * 10, kDefaultFontSize * 3 +
                      kTextBoxBuffer};
  const Color color = Color::white();

  PrintText("Do you really want to reset the game? "
            "Press y for yes, and n for no.", kDefaultFontSize, color, size,
            center);
}

/* --------------------------User Interaction-------------------------------- */

/**
 * Recognizes the user's spoken commands to perform the appropriate action.
 */
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

/**
 * Cinder's standard keyDown function, called whenever the user presses a key.
 * @param event the key that was pressed
 */
void ScreamyBall::keyDown(KeyEvent event) {
  ParseUserInteraction(event.getCode());
}

/**
 * Cinder's standard keyUp function, called whenever the user stops
 * pressing a key.
 * @param event the key that was released
 */
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

/**
 * Cinder's standard mouseDown function, called whenever the user uses
 * the mouse.
 * @param event the mouse event that occurred
 */
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

/**
 * Cinder's standard mouseUp function, called whenever the user stops pressing
 * a mouse button.
 * @param event the mouse event that stopped
 */
void ScreamyBall::mouseUp(MouseEvent event) {
  if (event.isRightDown()) {
    engine_.state_ = BallState::kRolling;
  }
}

/**
 * Helper function called whenever the user interacts with the game (via mouse,
 * keyboard, or speech)
 * @param event_code the KeyEvent code that's similar to the user's mouse,
 * speech, or keyboard interaction.
 */
void ScreamyBall::ParseUserInteraction(int event_code) {
  if (state_ == GameState::kPlaying && IsInGameInteraction(event_code)) {
      return;
  }

  switch (event_code) {
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
  }
}

/**
 * Checks if the keys that are solely used in-game (Jump, Duck, Pause)
 * were pressed, and changes state accordingly.
 * @param event_code the KeyEvent code that's similar to the user's mouse,
 * speech, or keyboard interaction.
 * @return true if any of the in-game keys were pressed, false otherwise.
 */
bool ScreamyBall::IsInGameInteraction(int event_code) {
  switch (event_code) {
    case KeyEvent::KEY_UP:
    case KeyEvent::KEY_w: {
      if (paused_) {
        return true;
      }
      engine_.state_ = BallState::kJumping;
      return true;
    }

    case KeyEvent::KEY_DOWN:
    case KeyEvent::KEY_s: {
      if (paused_ || engine_.state_ == BallState::kJumping) {
        return true;
      }
      engine_.state_ = BallState::kDucking;
      return true;
    }

    case KeyEvent::KEY_p: {
      paused_ = !paused_;
      if (paused_) {
        timer_.stop();
      } else {
        timer_.resume();
      }
      return true;
    }
  }
  return false;
}

/* --------------------------------Reset------------------------------------- */

/**
 * Resets the game state; sets all class variables to their initial values.
 */
void ScreamyBall::ResetGame() {
  engine_.Reset();
  paused_ = false;
  confirmed_reset_ = false;
  last_state_ = state_;
  state_ = GameState::kMenu;
  timer_.stop();
  last_update_secs_ = 0.00;
}

}  // namespace screamyball_app
