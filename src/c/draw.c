#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "state.h"

// TODO: Do something interesting when a "Quick View" is on screen

const int STATUSBAR_HEIGHT = 32;
const int SIDEBAR_WIDTH = 16;

// void
// draw_battery(Layer *layer, GContext *ctx) {
//   // TODO: Show charging state
//   // TODO: Change color on low battery
//   GRect bounds = layer_get_bounds(layer);
//   uint8_t percent = STATE.battery_percent;
//   GRect rect = GRect(0, 0, bounds.size.w * percent / 100, bounds.size.h);
//   rect = grect_inset(rect, GEdgeInsets(4));
//   graphics_context_set_fill_color(ctx, STATE.color_batt);
//   graphics_fill_rect(ctx, rect, 2, GCornersAll);
// }

static int
display_hours(int hours) {
  int display_hours = hours;
  if (clock_is_24h_style()) {
    display_hours %= 12;
    if (display_hours == 0) {
      display_hours = 12;
    }
  }
  return display_hours;
}

static void
draw_time(GRect bounds, FContext *fctx) {
  char hours[255];
  char minutes[255];
  snprintf(hours, sizeof hours, "%02d", display_hours(STATE.hours));
  snprintf(minutes, sizeof minutes, "%02d", STATE.minutes);
  int font_size = bounds.size.h / 2;
  FFont *font = STATE.font_noto_sans;
  GTextAlignment alignment = GTextAlignmentRight;
  FTextAnchor anchor = FTextAnchorTop;
  fctx_set_text_em_height(fctx, font, font_size);
  fctx_set_fill_color(fctx, GColorWhite);
  fctx_begin_fill(fctx);
  int x = bounds.origin.x;
  int y = bounds.origin.y;
  int w = bounds.size.w;
  FPoint offset_hours = FPointI(x + w, y);
  FPoint offset_minutes = FPointI(x + w, y + font_size);
  fctx_set_offset(fctx, offset_hours);
  fctx_draw_string(fctx, hours, font, alignment, anchor);
  fctx_set_offset(fctx, offset_minutes);
  fctx_draw_string(fctx, minutes, font, alignment, anchor);
  fctx_end_fill(fctx);
}

static void
draw_statusbar(GRect bounds, FContext *fctx) {
  char text[255];
  // TODO: Put useful info here
  snprintf(text, sizeof text, "STATUSBAR");
  int font_size = bounds.size.h;
  FFont *font = STATE.font_noto_sans;
  GTextAlignment alignment = GTextAlignmentLeft;
  FTextAnchor anchor = FTextAnchorTop;
  fctx_set_text_em_height(fctx, font, font_size);
  fctx_set_fill_color(fctx, GColorWhite);
  fctx_begin_fill(fctx);
  int x = bounds.origin.x;
  int y = bounds.origin.y;
  FPoint offset = FPointI(x, y);
  fctx_set_offset(fctx, offset);
  fctx_draw_string(fctx, text, font, alignment, anchor);
  fctx_end_fill(fctx);
}

void
draw_main(Layer *layer, GContext *ctx) {
  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  GRect bounds = layer_get_bounds(layer);
  GRect time_bounds = GRect(
    SIDEBAR_WIDTH,
    0,
    bounds.size.w - SIDEBAR_WIDTH,
    bounds.size.h - STATUSBAR_HEIGHT
  );
  GRect statusbar_bounds = GRect(
    0,
    bounds.size.h - STATUSBAR_HEIGHT,
    bounds.size.w,
    STATUSBAR_HEIGHT
  );
  draw_time(time_bounds, &fctx);
  draw_statusbar(statusbar_bounds, &fctx);
  fctx_deinit_context(&fctx);
}

// void
// draw_date(Layer *layer, GContext *ctx) {
//   GRect bounds = layer_get_bounds(layer);
//   char str[255];
//   // TODO: Support multiple date separators
//   snprintf(str, sizeof str, "%02d.%02d", STATE.month, STATE.date);
//   graphics_context_set_text_color(ctx, STATE.color_date);
//   draw_text_secondary(ctx, bounds, str);
// }

// void
// draw_steps(Layer *layer, GContext *ctx) {
//   GRect bounds = layer_get_bounds(layer);
//   char str[255] = "";
//   if (STATE.steps >= 0) {
//     snprintf(str, sizeof str, "%d", STATE.steps);
//   }
//   graphics_context_set_text_color(ctx, STATE.color_steps);
//   draw_text_secondary(ctx, bounds, str);
// }