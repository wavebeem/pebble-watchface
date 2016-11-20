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
  FFont *font_noto_sans;
  GFont gfont_gothic;
} State;

extern State STATE;