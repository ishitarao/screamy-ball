# Screamy Ball

[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![docs](https://img.shields.io/badge/docs-yes-brightgreen)](docs/README.md)

Read [this document](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html) to understand the project
layout.

**Author**: Ishita Rao - [`ir5@illinois.edu`](mailto:ir5@illinois.edu)

---

Screamy Ball is a recreation of Google Chrome’s [T. Rex Game](https://chromedino.com/), except the T. Rex is a ball that 
can be controlled in 3 ways:
1. The mouse
1. The keyboard
1. Spoken commands

### Background knowledge I have on the project
I know how to work with the Cinder library to create graphics for the game, and I know how to use Cinder to add keyboard 
and mouse control to the game. 

### Why I want to do the project
I have lots of memories of playing the T. Rex Game for hours when the internet connection at my house got cut out, so I 
thought it would be fun to recreate the game. Also, I’m very interested in Natural Language Processing in general, and 
I wanted to incorporate speech recognition into my game to get some hands-on practice with an NLP library.

### External Library
For this project, I will be using a Cinder Block: [ciSpeech](https://github.com/Hebali/ciSpeech). It provides Cinder 
support for [CMU PocketSphinx](http://cmusphinx.sourceforge.net/), a speech recognizer written in C++. 