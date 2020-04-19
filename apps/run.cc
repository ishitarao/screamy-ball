// Copyright (c) 2020 [Your Name]. All rights reserved.

#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>

#include "screamy_ball.h"

using cinder::app::App;
using cinder::app::RendererGl;


namespace screamy_ball {

const int kSamples = 8;
const int kWidth = 800;
const int kHeight = 800;

void SetUp(App::Settings* settings) {
  settings->setWindowSize(kWidth, kHeight);
  settings->setTitle("My CS 126 Application");
}

}  // namespace screamy_ball


// This is a macro that runs the application.
CINDER_APP(screamy_ball::MyApp,
           RendererGl(RendererGl::Options().msaa(screamy_ball::kSamples)),
           screamy_ball::SetUp)
