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
using cinder::app::toPixels;
using cinder::params::InterfaceGl;
using ci::fs::path;
using ci::app::getAssetPath;
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
DECLARE_string(player_name);

ScreamyBall::ScreamyBall()
    : kTileSize(FLAGS_tilesize),
      kHeight(FLAGS_height),
      kWidth(FLAGS_width),
      kDefaultFontSize(30),
      kTextBoxBuffer(10),
      kLeaderboardLimit(3),
      kLocMultiplier(0.5),
      kDefaultVolume(0.25), // music might mess with the speech recognition
      kUiDimensions({ FLAGS_tilesize * 4, FLAGS_tilesize * 3}),
      kPlayerName(FLAGS_player_name),
      engine_({2, static_cast<int>(FLAGS_height - 2)},
        FLAGS_width, FLAGS_height),
      leaderboard_(getAssetPath("screamy_ball.db").string()),
      elapsed_time_("00:00:00"),
      state_(GameState::kMenu),
      last_state_(GameState::kMenu),
      paused_(false),
      confirmed_reset_(false),
      delay_secs_(FLAGS_delay_secs),
      last_update_secs_(0.00),
      timer_(false),
      bg_music_("pokemon_battle_music.mp3"), // same mood
      scream_audio_("scream_audio.mp3") {}

/* ------------------------------Set Up-------------------------------------- */

/**
 * Cinder's standard setup function to set up the initial state of the game.
 */
void ScreamyBall::setup() {
  cinder::gl::enableDepthWrite();
  cinder::gl::enableDepthRead();
  SetupRecognizer();

  SetupMainMenuUi();
  SetupInGameUi();
  SetupGeneralUi();

  SetupInitialLeaderboards();

  SetupMusic(bg_music_);
  SetupMusic(scream_audio_);
  bg_music_.audio_obj_->start();
}

/**
 * Initialises ciSpeech's speech recognizer to 'keyword' mode.
 */
void ScreamyBall::SetupRecognizer() {
  path acoustic_model_path  = getAssetPath("en-us");
  path dict_path = getAssetPath("6247.dic");
  path lang_model_path = getAssetPath("6247.lm");

  //set up the language and the dictionary
  recognizer_ = sphinx::Recognizer::create(acoustic_model_path.string(),
      dict_path.string());

  //event handler for whenever speech is detected
  recognizer_->connectEventHandler(std::bind(
      &ScreamyBall::RecognizeCommands, this, std::placeholders::_1));

  //create and add a model based on the text file
  recognizer_->addModelJsgf("default", lang_model_path.string(),
      true);
  recognizer_->start();
}

/**
 * Initialises in-game UI, which will be displayed everywhere except the main
 * menu.
 */
void ScreamyBall::SetupInGameUi() {
  in_game_ui_ = InterfaceGl::create(getWindow(), "Game Options",
                                    toPixels(kUiDimensions));

  // a lambda is fired to simulate pressing the up key
  in_game_ui_->addButton("Jump", [&]() {
    ParseUserInteraction(KeyEvent::KEY_UP); });

  /* ducking with a button is different from ducking with a keyboard, because
   * Cinder Params does not have a feature to hold down a button. */
  in_game_ui_->addButton("Duck", [&]() {
    if (engine_.state_ == BallState::kDucking) {
      engine_.state_ = BallState::kRolling;
    } else {
      ParseUserInteraction(KeyEvent::KEY_DOWN);
    } });

  in_game_ui_->addParam("Pause", &paused_);
  in_game_ui_->addSeparator();

  // lambda to change the game state to menu when the menu button is pressed
  in_game_ui_->addButton("Menu",[&]() {
    ParseUserInteraction(KeyEvent::KEY_m); });

  in_game_ui_->addButton("Mute",
                         std::bind(&ScreamyBall::Mute,this));

  in_game_ui_->addButton("Reset",[&]() {
    ParseUserInteraction(KeyEvent::KEY_r); });
}

/**
 * Initializes the Main Menu UI.
 */
void ScreamyBall::SetupMainMenuUi() {
  menu_ui_ = InterfaceGl::create(getWindow(), "Main Menu",
                                 toPixels(kUiDimensions));

  //Start button: fires a lambda that starts the timer and the game when pressed
  menu_ui_->addButton("Start",[&]() {
    ResetGame();
    last_state_ = state_;
    state_ = GameState::kPlaying; });

  menu_ui_->addButton("Help",[&]() {
    ParseUserInteraction(KeyEvent::KEY_h); });

  menu_ui_->addButton("Leaderboard", [&]() {
    last_state_ = state_;
    state_ = GameState::kLeaderboard;
  });

  menu_ui_->addButton("Mute",
                      std::bind(&ScreamyBall::Mute,this));

  menu_ui_->addButton("Reset",[&]() {
    ParseUserInteraction(KeyEvent::KEY_r); });
}

/**
 * Initalises general UI for all other game states.
 */
void ScreamyBall::SetupGeneralUi() {
  general_ui_ = InterfaceGl::create(getWindow(), "Options",
                                 toPixels(kUiDimensions));

  // lambda to change the game state to menu when the menu button is pressed
  general_ui_->addButton("Menu",[&]() {
    ParseUserInteraction(KeyEvent::KEY_m); });

  general_ui_->addButton("Mute",
                         std::bind(&ScreamyBall::Mute,this));

  general_ui_->addButton("Reset",[&]() {
    ParseUserInteraction(KeyEvent::KEY_r); });
}

/**
 * Sets up the leaderboard at the start of the game.
 */
void ScreamyBall::SetupInitialLeaderboards() {
  top_players_ = leaderboard_.RetrieveHighScores(kLeaderboardLimit);

  current_player_top_scores_ = leaderboard_.RetrieveHighScores
      ({ kPlayerName, elapsed_time_ }, kLeaderboardLimit);
}

/**
 * Initialises the music.
 * @param audio Audio object that contains the asset path and the VoiceRef
 * object.
 */
void ScreamyBall::SetupMusic(Audio& audio) {
  cinder::audio::SourceFileRef music_source = cinder::audio::load(
      cinder::app::loadAsset(audio.asset_name_));

  audio.audio_obj_ = cinder::audio::Voice::create(music_source);
  audio.audio_obj_->setVolume(kDefaultVolume);
}

/* --------------------------------Update------------------------------------ */

/**
 * Cinder's standard update function.
 */
void ScreamyBall::update() {
  // loop audio
  if (!bg_music_.audio_obj_->isPlaying()) {
    bg_music_.audio_obj_->start();
  }

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

    case GameState::kGameOver: {
      timer_.stop();
      elapsed_time_ = PrettyPrintElapsedTime(timer_.getSeconds());
      PopulateLeaderboards();
    }

    case GameState::kConfirmingReset: {
      timer_.stop();
      if (confirmed_reset_) {
        ResetGame();
        leaderboard_.Reset();
      }
      break;
    }
    //in all other cases, there is nothing to update.
    default: {
      if (!timer_.isStopped()) {
        timer_.stop();
      }
      break;
    }
  }
}

/**
 * Calls the engine's Run function after the specified
 * number of delay seconds have passed (to control game speed)
 */
void ScreamyBall::RunEngine() {
  const double current_time = timer_.getSeconds();
  // manage game speed
  if (current_time - last_update_secs_ >= delay_secs_) {
    engine_.Run();
    last_update_secs_ = current_time;

    if (engine_.state_ == BallState::kCollided) {
      last_state_ = state_;
      state_ = GameState::kGameOver;
      last_update_secs_ = 0.00;
    }
  }
}

/**
 * Updates the leaderborads with the latest times.
 */
void ScreamyBall::PopulateLeaderboards() {
  if (top_players_.empty()) {
    Player current_player = { kPlayerName, elapsed_time_ };
    leaderboard_.AddScoreToLeaderboard(current_player);
    top_players_ = leaderboard_.RetrieveHighScores(kLeaderboardLimit);

    current_player_top_scores_ = leaderboard_.RetrieveHighScores
        (current_player, kLeaderboardLimit);

    // It is crucial that these vectors be populated, given that the limit > 0.
    assert(!top_players_.empty());
    assert(!current_player_top_scores_.empty());
  }
}

/**
 * Mutes audio if unmuted, unmutes audio otherwise.
 */
void ScreamyBall::Mute() {
  if (bg_music_.audio_obj_->getVolume() == 0.00) {
    bg_music_.audio_obj_->setVolume(kDefaultVolume);
    scream_audio_.audio_obj_->setVolume(kDefaultVolume);
  } else {
    bg_music_.audio_obj_->setVolume(0.00);
    scream_audio_.audio_obj_->setVolume(0.00);
  }
}

/**
 * Pretty Prints the input time in seconds in the format hh:mm:ss.
 * @param time_secs the time in seconds to pretty print
 * @return the elapsed time in the above format
 */
string PrettyPrintElapsedTime(double time_secs) {
  int seconds = (int) time_secs;
  int double_digits = 10;

  int hours = seconds / (kNumSeconds * kNumSeconds);
  seconds -= hours * (kNumSeconds * kNumSeconds);

  int minutes = seconds / kNumSeconds;
  seconds -= minutes * kNumSeconds;

  std::stringstream sstream;
  if (hours < double_digits) {
    sstream << '0' << hours << ':';
  } else {
    sstream << hours << ':';
  }

  if (minutes < double_digits) {
    sstream << '0' << minutes << ':';
  } else {
    sstream << minutes << ':';
  }

  if (seconds < double_digits) {
    sstream << '0' << seconds;
  } else {
    sstream << seconds;
  }

  return sstream.str();
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
    case GameState::kLeaderboard: {
      DrawLeaderboard();
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
  const ivec2 size = { kTileSize * 4, kTileSize * 2 };
  const Color color = Color::white();
  PrintText("Screamy Ball", kDefaultFontSize, color, size, center);

  menu_ui_->setPosition({ center.x - kTileSize * 2,
                         center.y + kTileSize });
}

/**
 * Draws the help page.
 */
void ScreamyBall::DrawHelp() {
  cinder::gl::clear(Color::black());
  const ivec2 pos = {getWindowCenter().x, getWindowPosY()};
  const ivec2 size = { kWidth * kTileSize, kTileSize };
  const Color color = Color::white();
  size_t row = 0;

  auto input_stream = cinder::loadFileStream(ci::app::getAssetPath(
      "help.txt"));

  PrintText(input_stream->readLine(), kDefaultFontSize, color, size, pos);

  while (!input_stream->isEof()) {
    // The font is half the tile size
    PrintText(input_stream->readLine(),
        ((float)(kTileSize - kTextBoxBuffer) / 2), color, size,
        { pos.x, pos.y + (++row) * kTileSize });
  }
}

/**
 * Draws the in-game background.
 */
void ScreamyBall::DrawBackground() {
  cinder::gl::clear(Color::black());

  // draw the ground:
  int ground_height = engine_.kMinHeight;
  cinder::gl::color(Color::white());
  const ivec2 upper_left = { 0, kTileSize * ground_height };
  const ivec2 bottom_right = { kWidth * kTileSize, kTileSize * kHeight };
  cinder::gl::drawSolidRect(cinder::Rectf(upper_left, bottom_right));
}

/**
 * Draws the ball in two states: normally, and while ducking.
 */
void ScreamyBall::DrawBall() {
  const Location loc = engine_.ball_.location;
  const float loc_multiplier_cubed = pow(kLocMultiplier, 3);
  const float center_x = (loc.Row() + kLocMultiplier) * kTileSize;
  const float radius_x = (float)kTileSize * kLocMultiplier;

  cinder::gl::color(Color(1, 0, 0));

  if (engine_.state_ == BallState::kDucking) {
    const ivec2 ellipse_center = { center_x,
                                   (loc.Col() - loc_multiplier_cubed)
                                    * kTileSize };
    cinder::gl::drawSolidEllipse(ellipse_center, radius_x,
        ((float)kTileSize * loc_multiplier_cubed));
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
  screamy_ball::Obstacle obstacle = engine_.obstacle_;
  Location loc = obstacle.location;
  const int obstacle_height = obstacle.kHeight;
  const float loc_incre = kTileSize * kLocMultiplier;

  cinder::gl::color(Color::gray(0.5)); // 0.5 is the % of grey

  //create spikes
  for (int counter = 0; counter < obstacle.length; counter++) {

    // points of a triangle for a 'low' obstacle
    ivec2 point_1 = {loc.Row() * kTileSize, loc.Col() * kTileSize};
    ivec2 point_2 = {(loc.Row() - 1) * kTileSize,
                      loc.Col() * kTileSize};

    ivec2 point_3 = {(loc.Row() - kLocMultiplier) * kTileSize,
                              (loc.Col() - obstacle_height) * kTileSize};

    // increment location if it's a 'high' obstacle
    if (obstacle.type == screamy_ball::ObstacleType::kHigh) {
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
 * Draws the Game Over page.
 */
void ScreamyBall::DrawGameOver() {
  cinder::gl::clear(Color::black());
  const cinder::vec2 center = getWindowCenter();
  const ivec2 size = {kTileSize * 10, kTileSize};
  const Color color = Color::white();
  PrintText("Your time: " + elapsed_time_, kDefaultFontSize, color, size,
      center);
}

/**
 * Draws the leaderboard containing the top player scores, and the
 * leaderboard containing the current player's top scores.
 */
void ScreamyBall::DrawLeaderboard() {

  cinder::gl::clear(Color::black());

  const cinder::vec2 start_text = { getWindowCenter().x, getWindowPosY() +
                                   kTileSize * 2 };
  const cinder::ivec2 size = { kTileSize * 10, kTileSize };
  const Color color = Color::white();

  if (top_players_.empty()) {
    PrintText("Sorry, no leaderboard data is available yet. :(",
        kDefaultFontSize, color, { kTileSize * 10, kTileSize * 2 },
        getWindowCenter());
    return;
  }

  size_t row = 0;
  DrawTopPlayerScores(row, color, size, start_text);
  row++;
  DrawCurrentPlayerScores(row, color, size, start_text);
}

/**
 * Draws the scores of the top players.
 * @param start_row the row from which to start printing text.
 * @param color the color of the text.
 * @param size the size of the text box.
 * @param pos the location of the text.
 */
void ScreamyBall::DrawTopPlayerScores(size_t& start_row, const Color& color,
    const ivec2& size, const ivec2& pos) {
  // print top players
  PrintText("Name - Time", kDefaultFontSize, color, size,
            { pos.x, pos.y + (++start_row) * kTileSize });

  for (const Player& player : top_players_) {
    std::stringstream ss;
    ss << player.name << " - " << player.elapsed_time;
    PrintText(ss.str(), kDefaultFontSize, color, size,
              { pos.x, pos.y + (++start_row) * kTileSize });
  }
}

/**
 * Draws the current player's best times.
 * @param start_row the row from which to start printing text.
 * @param color the color of the text.
 * @param size the size of the text box.
 * @param pos the location of the text.
 */
void ScreamyBall::DrawCurrentPlayerScores(size_t& start_row, const Color& color,
    const ivec2& size, const ivec2& pos) {
  PrintText(kPlayerName + "'s Best Times: ", kDefaultFontSize, color, size,
            { pos.x, pos.y + (++start_row) * kTileSize });

  PrintText("Score - Time", kDefaultFontSize, color, size,
            { pos.x, pos.y + (++start_row) * kTileSize });

  for (const Player& player : current_player_top_scores_) {
    PrintText(player.elapsed_time, kDefaultFontSize, color, size,
              { pos.x,pos.y + (++start_row) * kTileSize} );
  }
}

/**
 * Draws the page that asks the user to confirm if they want to reset or not.
 */
void ScreamyBall::DrawConfirmReset() {
  cinder::gl::clear(Color::black());
  const cinder::vec2 start_text = { getWindowCenter().x, getWindowPosY() +
      kTileSize * 2 };
  //Multiplying font size by 3 since there will be 3 lines of text:
  const ivec2 size = {kTileSize * 10, kDefaultFontSize * 3 +
                      kTextBoxBuffer};
  const Color color = Color::white();

  PrintText("Do you really want to reset the game?", kDefaultFontSize,
      color, size, start_text);

  size_t row = 1;

  PrintText("You will lose your progress and leaderboard data.",
      kDefaultFontSize, color, size,
      { start_text.x, start_text.y + kTileSize * (++row) });

  row++;

  PrintText("Press y for yes, and n for no.", kDefaultFontSize,
            color, size,
            { start_text.x, start_text.y + kTileSize * (++row) });
}

/* --------------------------User Interaction-------------------------------- */

/**
 * Recognizes the user's spoken commands to perform the appropriate action.
 */
void ScreamyBall::RecognizeCommands(const std::string& message) {
  if (message == "HIGHER") {
    ParseUserInteraction(KeyEvent::KEY_UP);
  } else if (message == "LOWER") {
    ParseUserInteraction(KeyEvent::KEY_DOWN);
  } else if (message == "PAUSE GAME") { // just 'pause' is recognized randomly
    ParseUserInteraction(KeyEvent::KEY_p);
  } else if (message == "RESET GAME" || message == "RESET") {
    ParseUserInteraction(KeyEvent::KEY_r);
  } else if (message == "MUTE SOUNDS" || message == "SOUNDS") {
    Mute(); //just 'mute' is recognized randomly, but 'sounds' isn't
  } else if (message == "MAIN MENU" || message == "MENU") {
    ParseUserInteraction(KeyEvent::KEY_m);
  } else if (message == "INSTRUCTIONS") {
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
        last_state_ = GameState::kConfirmingReset;
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
      scream_audio_.audio_obj_->start();
      engine_.state_ = BallState::kJumping;
      return true;
    }

    case KeyEvent::KEY_DOWN:
    case KeyEvent::KEY_s: {
      if (paused_ || engine_.state_ == BallState::kJumping) {
        return true;
      }
      scream_audio_.audio_obj_->start();
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
  elapsed_time_ = "00:00:00";
  top_players_.clear();
  current_player_top_scores_.clear();
}

}  // namespace screamyball_app
