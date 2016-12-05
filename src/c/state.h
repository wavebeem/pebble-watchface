#pragma once

#include <pebble.h>
#include <pebble-fctx/fctx.h>

#include <pebble-events/pebble-events.h>
#include "enamel.h"


typedef struct {
  int hours;
  int minutes;
  int seconds;
  int month;
  int date;
  int steps;
  uint8_t battery_percent;
  FFont *font_noto_sans_regular;
  FFont *font_noto_sans_bold;
} State;

bool should_vibrate_hourly();
bool is_date_format_MMDD();

extern State STATE;