[![GitHub release](https://img.shields.io/github/release/teeebor/flipper_games?include_prereleases=&sort=semver&color=blue)](https://github.com/teeebor/flipper_games/releases/)
[![License](https://img.shields.io/badge/License-MIT-blue)](/LICENSE)
[![issues - flipper-zero_authenticator](https://img.shields.io/github/issues/teeebor/flipper_games)](https://github.com/teeebor/flipper_games/issues)
![maintained - yes](https://img.shields.io/badge/maintained-yes-blue)
![contributions - welcome](https://img.shields.io/badge/contributions-welcome-blue)
[![Build](https://github.com/teeebor/flipper_games/actions/workflows/main.yml/badge.svg)](https://github.com/teeebor/flipper_games/actions/workflows/main.yml)
# Games for Flipper Zero

## Solitaire
![Play screen](screenshots/solitaire.gif)

### Shortcuts
* Long press up skips the navigation inside the bottom column
* Long press center to automatically place the card to the top rigth section

----

## Blackjack

![Play screen](screenshots/play_scene.png)

----

## Tools

- Flipper Game Engine
    
    Currently work in progress simple game framework for flipper zero

- CMakeLists.txt

    A small cmake file to set up some flipper include paths. I use it to tell Clion what to index.

- build_flipper_application.sh

    A helper script I use to build for multiple firmwares
    > ## Usage
    > ```bash
    > $ build_flipper_application.sh <path_to_the_firmware> <prefix_for_the_fap>
    > ```