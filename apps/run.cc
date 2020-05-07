// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <vector>

#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>
#include <gflags/gflags.h>

#include "screamy_ball.h"

using cinder::app::App;
using cinder::app::RendererGl;
using std::string;
using std::vector;

namespace screamyball_app {

DEFINE_uint32(width, 16, "the number of tiles in each row");
DEFINE_uint32(height, 16, "the number of tiles in each column");
DEFINE_uint32(tilesize, 50, "the size of each tile");
DEFINE_double(delay_secs, 0.1, "the delay (in seconds) of the game");
DEFINE_string(player_name, "J o m p", "The name of the player to display");

const int kSamples = 8;
const int kWidth = 800;
const int kHeight = 800;

void ParseArgs(vector<string>* args) {
  gflags::SetUsageMessage(
      "Play a game called Screamy Ball. Pass --helpshort for options.");
  int argc = static_cast<int>(args->size());

  vector<char*> argvs;
  for (string& str : *args) {
    argvs.push_back(&str[0]);
  }

  char** argv = argvs.data();
  gflags::ParseCommandLineFlags(&argc, &argv, true);
}

void SetUp(App::Settings* settings) {
  vector<string> args = settings->getCommandLineArgs();
  ParseArgs(&args);

  const int width = static_cast<int>(FLAGS_width * FLAGS_tilesize);
  const int height = static_cast<int>(FLAGS_height * FLAGS_tilesize);

  settings->setWindowSize(width, height);
  settings->setTitle("Screamy Ball");
}

}  // namespace screamyball_app

// This is a macro that runs the application.
CINDER_APP(screamyball_app::ScreamyBall,
           RendererGl(RendererGl::Options().msaa(screamyball_app::kSamples)),
           screamyball_app::SetUp)
