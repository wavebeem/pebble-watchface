#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "state.h"

// TODO: Do something interesting when a "Quick View" is on screen

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

void
draw_main(Layer *layer, GContext *ctx) {
  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  GRect bounds = layer_get_bounds(layer);
  char str[255];
  // TODO: Support 12 hour time also
  snprintf(str, sizeof str, "%02d:%02d", STATE.hours, STATE.minutes);
//   GColor text_color = gcolor_legible_over(STATE.color_time);
//   graphics_context_set_text_color(ctx, text_color);
//   graphics_context_set_fill_color(ctx, text_color);
//   graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  int font_size = 48;
  FPoint offset = FPointI(bounds.size.w / 2, bounds.size.h / 2);
  fctx_set_text_em_height(&fctx, STATE.font_noto_sans, font_size);
  fctx_set_offset(&fctx, offset);
  fctx_set_fill_color(&fctx, GColorWhite);
  fctx_begin_fill(&fctx);
  fctx_draw_string(&fctx, str, STATE.font_noto_sans, GTextAlignmentCenter, FTextAnchorTop);
  fctx_end_fill(&fctx);
  fctx_deinit_context(&fctx);
//   draw_text_primary(ctx, bounds, str);
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