// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>

#include "screamy_ball.h"

using cinder::app::App;
using cinder::app::RendererGl;


namespace screamyball_app {

const int kSamples = 8;
const int kWidth = 800;
const int kHeight = 800;

void SetUp(App::Settings* settings) {
  settings->setWindowSize(kWidth, kHeight);
  settings->setTitle("My CS 126 Application");
}

}  // namespace screamyball_app


// This is a macro that runs the application.
CINDER_APP(screamyball_app::ScreamyBall,
           RendererGl(RendererGl::Options().msaa(screamyball_app::kSamples)),
           screamyball_app::SetUp)
