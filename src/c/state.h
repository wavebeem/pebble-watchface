#pragma once
#include <pebble.h>
#include <pebble-fctx/fctx.h>

typedef struct {
  int hours;
  int minutes;
  int month;
  int date;
  int steps;
  uint8_t battery_percent;
  FFont *font_noto_sans_regular;
  FFont *font_noto_sans_bold;
} State;

extern State STATE;