// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include "screamy_ball.h"

#include <cinder/app/App.h>
#include <cinder/Font.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>

#include <screamy-ball/direction.h>

namespace screamyball_app {

using cinder::Color;
using cinder::ColorA;
using cinder::TextBox;
using cinder::app::KeyEvent;
using screamy_ball::Direction;
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

ScreamyBall::ScreamyBall()
    : printed_game_over_{false},
      paused_{false} { }

void ScreamyBall::setup() {
  cinder::gl::enableDepthWrite();
  cinder::gl::enableDepthRead();
}

void ScreamyBall::update() { }

void ScreamyBall::draw() { }

template <typename C>
void PrintText(const string& text, const C& color, const cinder::ivec2& size,
               const cinder::vec2& loc) {
  cinder::gl::color(color);

  auto box = TextBox()
      .alignment(TextBox::CENTER)
      .font(cinder::Font(kNormalFont, 30))
      .size(size)
      .color(color)
      .backgroundColor(ColorA(0, 0, 0, 0))
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

  PrintText("Game Over :(", color, size, center);

  printed_game_over_ = true;
}

void ScreamyBall::keyDown(KeyEvent event) {
  switch (event.getCode()) {
    case KeyEvent::KEY_UP:
    case KeyEvent::KEY_k:
    case KeyEvent::KEY_w: {
      //engine_.SetDirection(Direction::kUp);
      break;
    }
    case KeyEvent::KEY_DOWN:
    case KeyEvent::KEY_j:
    case KeyEvent::KEY_s: {
      //engine_.SetDirection(Direction::kDown);
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
  }}

}  // namespace screamyball_app
