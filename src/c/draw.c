#include <pebble.h>
#include "state.h"

void
draw_battery(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  // TODO: Horizontally center this stuff
  const int stroke_width = 2;
  const int margin = 10;
  const int gap = 3;
  const int radius = 5;
  uint8_t percent = STATE.battery_percent;
  const int chunks = bounds.size.w > 144 ? 10 : 8;
  const int percent_chunk_size = 100 / chunks;
  const int n = percent / percent_chunk_size;
  for (int i = 0; i < chunks; i++) {
    graphics_context_set_stroke_width(ctx, stroke_width);
    graphics_context_set_stroke_color(ctx, STATE.color_batt);
    graphics_context_set_fill_color(ctx, STATE.color_batt);
    const GPoint point = GPoint(
      margin + radius + 2 * (radius + gap) * i,
      bounds.size.h - margin
    );
    if (i <= n) {
      graphics_draw_circle(ctx, point, radius);
      graphics_fill_circle(ctx, point, radius);
    } else {
      graphics_draw_circle(ctx, point, radius);
    }
  }
}

void
draw_time(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  char str[255];
  snprintf(str, sizeof str, "%02d:%02d", STATE.hours, STATE.minutes);
  GColor text_color = gcolor_legible_over(STATE.color_time);
  graphics_context_set_text_color(ctx, text_color);
  graphics_context_set_fill_color(ctx, STATE.color_time);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_draw_text(
    ctx,
    str,
    STATE.font_primary,
    bounds,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter,
    NULL
  );
}

void
draw_date(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  char str[255];
  snprintf(str, sizeof str, "%02d.%02d", STATE.month, STATE.date);
  graphics_context_set_text_color(ctx, STATE.color_date);
  graphics_draw_text(
    ctx,
    str,
    STATE.font_secondary,
    bounds,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter,
    NULL
  );
}

void
draw_steps(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  char str[255] = "-";
  if (STATE.steps >= 0) {
    snprintf(str, sizeof str, "%d", STATE.steps);
  }
  graphics_context_set_text_color(ctx, STATE.color_steps);
  graphics_draw_text(
    ctx,
    str,
    STATE.font_secondary,
    bounds,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter,
    NULL
  );
}