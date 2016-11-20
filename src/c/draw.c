#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>

#include "state.h"

// TODO: Do something interesting when a "Quick View" is on screen

static int
preferred_hours(int hours) {
  int ret = hours;
  if (clock_is_24h_style()) {
    ret %= 12;
    if (ret == 0) {
      ret = 12;
    }
  }
  return ret;
}

static void
draw_time(GRect bounds, FContext *fctx) {
  char hours[255];
  char minutes[255];
  snprintf(hours, sizeof hours, "%02d", preferred_hours(STATE.hours));
  snprintf(minutes, sizeof minutes, "%02d", STATE.minutes);
  int font_size = bounds.size.h / 2;
  FFont *font = STATE.font_noto_sans;
  GTextAlignment alignment = GTextAlignmentCenter;
  fctx_set_text_em_height(fctx, font, font_size);
  fctx_set_fill_color(fctx, GColorWhite);
  fctx_begin_fill(fctx);
  int x = bounds.origin.x;
  int y = bounds.origin.y;
  int w = bounds.size.w;
  int h = bounds.size.h;
  FPoint offset_hours = FPointI(x + w / 2, y);
  FPoint offset_minutes = FPointI(x + w / 2, h);
  fctx_set_offset(fctx, offset_hours);
  fctx_draw_string(fctx, hours, font, alignment, FTextAnchorTop);
  fctx_set_offset(fctx, offset_minutes);
  fctx_draw_string(fctx, minutes, font, alignment, FTextAnchorBottom);
  fctx_end_fill(fctx);
}

void
draw_main(Layer *layer, GContext *ctx) {
  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  GRect bounds = layer_get_bounds(layer);
  draw_time(bounds, &fctx);
  fctx_deinit_context(&fctx);
}