#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>

#include "state.h"

// TODO: Do something interesting when a "Quick View" is on screen

// Apparently 12-hour time is still incredibly popular (.-.)
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
draw_string_with_bg(
  GContext *gctx,
  FContext *fctx,
  GRect bounds,
  GColor bg_color,
  int font_size,
  const char* text
) {
  // Draw the background <(^_^ <)
  graphics_context_set_fill_color(gctx, bg_color);
  graphics_fill_rect(gctx, bounds, 0, GCornerNone);

  // Draw the text (-_-;)
  GColor fg_color = gcolor_legible_over(bg_color);
  FFont *font = STATE.font_noto_sans;
  fctx_set_text_em_height(fctx, font, font_size);
  fctx_set_fill_color(fctx, fg_color);
  fctx_begin_fill(fctx);
  int x = bounds.origin.x;
  int y = bounds.origin.y;
  int w = bounds.size.w;
  int h = bounds.size.h;
  FPoint offset = FPointI(x + w / 2, y);
  fctx_set_offset(fctx, offset);
  fctx_draw_string(fctx, text, font, GTextAlignmentCenter, FTextAnchorTop);
  fctx_end_fill(fctx);
}

static void
draw_time(GContext *gctx, FContext *fctx, GRect bounds) {
  char str[255];
  snprintf(
    str,
    sizeof str,
    "%02d:%02d",
    preferred_hours(STATE.hours),
    STATE.minutes
  );
  draw_string_with_bg(
    gctx,
    fctx,
    bounds,
    GColorWhite,
    54,
    str
  );
}

void
draw_main(Layer *layer, GContext *ctx) {
  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  GRect bounds = layer_get_bounds(layer);
  draw_time(ctx, &fctx, bounds);
  fctx_deinit_context(&fctx);
}