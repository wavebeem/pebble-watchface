#pragma once
#include <pebble.h>

typedef struct {
  int hours;
  int minutes;
  int month;
  int date;
  int steps;
  uint8_t battery_percent;
  GFont font_primary;
  GFont font_secondary;
  GColor8 color_time;
  GColor8 color_date;
  GColor8 color_batt;
  GColor8 color_steps;
  GColor8 color_bg;
} State;

extern State STATE;