#!/bin/bash
cc -o game game.c player.c screen.c -lSDL2 -lm -I include -g
