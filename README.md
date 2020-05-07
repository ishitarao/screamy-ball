# Screamy Ball

[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![docs](https://img.shields.io/badge/docs-yes-brightgreen)](docs/README.md)

Read [this document](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html) to understand the project
layout.

**Author**: Ishita Rao - [`ir5@illinois.edu`](mailto:ir5@illinois.edu)

---

Screamy Ball is a retro recreation of Google Chrome’s [T. Rex Game](https://chromedino.com/), except the obstacles are spiked and the T. Rex is a ball that 
can be controlled in 3 ways:
1. The mouse
1. The keyboard
1. Spoken commands

### Dependencies
* [Cinder](https://github.com/cinder/Cinder)
* [ciSpeech](https://github.com/Hebali/ciSpeech) (provides Cinder support for 
                                                  [CMU PocketSphinx](http://cmusphinx.sourceforge.net/), a speech 
                                                  recognizer written in C++)
* [gflags](https://github.com/gflags/gflags)
* [cmake](https://cmake.org/)
* [Catch2](https://github.com/catchorg/Catch2)
                                                                                            
Due to compatibility issues with ciSpeech, you will require Mac OS for the game to work.

### The Game
The aim of the game is for the ball to dodge the spikes, either by jumping, or by ducking. This goes on until the ball 
eventually hits the spikes. Once the game is over, you will be informed of how long you lasted.

To start playing the game, you just click on the start button in the menu. For information on the controls, you can 
click on the help button. Resetting the game resets the game state.

#### Controls

| Action | Keyboard Controls | Mouse Controls | Speech Controls  |
|:------:|:-----------------:|:--------------:|:----------------:|
| Jump   |     Up/w/k        |  Left Click    | Say "Higher"     |
| Duck   |     Down/j/s      |  Right Click   | Say "Lower"      |
| Pause  |      p            |  Pause Button  | Say "Pause Game" |
| Reset  |      r            |  Reset Button  | Say "Reset Game" |
| Menu   |      m            |  Menu Button   | Say "Main Menu"  |
| Help   |      h            |  Help Button   |Say "Instructions"|

Speech recognition can be unpredictable in terms of delay, so you may use it at your own discretion.
