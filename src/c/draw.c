#include <pebble.h>
#include "state.h"

static void
_draw_text(GContext *ctx, GRect bounds, const char* str, GFont font, int offset) {
  const bool clip = true;
  GSize size = graphics_text_layout_get_content_size(str, font, bounds,
    GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
  GRect rect = GRect(0, 0, size.w, size.h);
  grect_align(&rect, &bounds, GAlignCenter, clip);
  // Hack to make text actually vertically centered
  rect.origin.y += offset;
  graphics_draw_text(ctx, str, font, rect,
    GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void
draw_text_primary(GContext *ctx, GRect bounds, const char* str) {
  _draw_text(ctx, bounds, str, STATE.font_primary, -8);
}

static void
draw_text_secondary(GContext *ctx, GRect bounds, const char* str) {
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  _draw_text(ctx, bounds, str, STATE.font_secondary, -5);
}

void
draw_battery(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
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
    graphics_draw_circle(ctx, point, radius);
    if (i <= n) {
      graphics_fill_circle(ctx, point, radius);
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
  draw_text_primary(ctx, bounds, str);
}

void
draw_date(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  char str[255];
  snprintf(str, sizeof str, "%02d.%02d", STATE.month, STATE.date);
  graphics_context_set_text_color(ctx, STATE.color_date);
  draw_text_secondary(ctx, bounds, str);
}

void
draw_steps(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  char str[255] = "-";
  if (STATE.steps >= 0) {
    snprintf(str, sizeof str, "%d", STATE.steps);
  }
  graphics_context_set_text_color(ctx, STATE.color_steps);
  draw_text_secondary(ctx, bounds, str);
}