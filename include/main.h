#pragma once

#include "pico.h"
#include "stdio.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <pico/time.h>
#include "pico/rand.h"
#include "hardware/watchdog.h"

#include "colors.h"
#include "input.h"
#include "graphics.h"

#include <math.h>
#include <vector>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include "st7789.h"
#ifdef __cplusplus
}
#endif

/*
called when a button chages state
state: true = high, false = low
*/
void ButtonPressed(uint buttonn, bool state);

//TODO: add game selection for user

//Game selection
#define TETRIS

//tetris
#ifdef TETRIS
#include "tetris.h"
#define LAUNCH tetrisLaunch
#define INPUT tetrisInput
#endif

//TODO: add pong