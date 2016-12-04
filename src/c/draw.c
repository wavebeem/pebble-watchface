#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>

#include "state.h"

// TODO: Do something interesting when a "Quick View" is on screen

#define ALT_STYLE DRAW_FG
// #define ALT_COLOR GColorRajah
// #define ALT_COLOR GColorInchworm
// #define ALT_COLOR GColorCeleste
// #define ALT_COLOR GColorCeleste
// #define ALT_COLOR GColorWhite

// #define ALT_STYLE DRAW_BG
// #define ALT_COLOR GColorMidnightGreen

#define BORDER_RADIUS 4

static GColor
current_color() {
  if (STATE.battery_percent <= 30) {
    return GColorSunsetOrange;
  }
  else {
    return GColorWhite;
  }
}

// Apparently 12-hour time is still incredibly popular (.-.)
static int
preferred_hours(int hours) {
  int ret = hours;
  if (!clock_is_24h_style()) {
    ret %= 12;
    if (ret == 0) {
      ret = 12;
    }
  }
  return ret;
}

typedef enum {
  DRAW_BG,
  DRAW_FG
} Style;

static void
draw_string(
  GContext *gctx,
  FContext *fctx,
  Style style,
  GRect bounds,
  GColor color,
  int font_size,
  const char* text
) {
  if (style == DRAW_BG) {
    // Draw the background <(^_^ <)
    graphics_context_set_fill_color(gctx, color);
    graphics_fill_rect(gctx, bounds, BORDER_RADIUS, GCornersAll);
  }

  // Draw the text (-_-;)
  GColor fg_color = style == DRAW_BG
    ? gcolor_legible_over(color)
    : color;
  // FFont *font = STATE.font_noto_sans_regular;
  FFont *font = STATE.font_noto_sans_bold;
  fctx_set_text_cap_height(fctx, font, font_size);
  fctx_set_fill_color(fctx, fg_color);
  fctx_begin_fill(fctx);
  int x = bounds.origin.x;
  int y = bounds.origin.y;
  int w = bounds.size.w;
  int h = bounds.size.h;
  int dy = font_size / -20;
  FPoint offset = FPointI(x + w / 2, y + h / 2 + dy);
  fctx_set_offset(fctx, offset);
  fctx_draw_string(fctx, text, font, GTextAlignmentCenter, FTextAnchorCapMiddle);
  fctx_end_fill(fctx);
}

static void
draw_time(GContext *gctx, FContext *fctx, int font_size, GRect bounds) {
  char str[255];
  snprintf(
    str,
    sizeof str,
    "%02d:%02d",
    preferred_hours(STATE.hours),
    STATE.minutes
  );
  draw_string(
    gctx,
    fctx,
    DRAW_BG,
    bounds,
    current_color(),
    font_size,
    str
  );
}

static void
draw_date(GContext *gctx, FContext *fctx, int font_size, GRect bounds) {
  char str[255];
  snprintf(
    str,
    sizeof str,
    "%02d/%02d",
    STATE.month,
    STATE.date
  );
  draw_string(
    gctx,
    fctx,
    DRAW_FG,
    bounds,
    current_color(),
    font_size,
    str
  );
}

static void
draw_step(GContext *gctx, FContext *fctx, int font_size, GRect bounds) {
  if (STATE.steps == -1) {
    return;
  }
  char str[255];
  snprintf(
    str,
    sizeof str,
    "%d",
    STATE.steps
    // 28065
    // 8065
  );
  draw_string(
    gctx,
    fctx,
    DRAW_FG,
    bounds,
    current_color(),
    font_size,
    str
  );
}

typedef struct {
  GRect top;
  GRect middle;
  GRect bottom;
} AllBounds;

AllBounds
bounds_calc(GRect bounds) {
  int x = bounds.origin.x;
  int y = bounds.origin.y;
  int w = bounds.size.w;
  int h = bounds.size.h;
  int n = 16;
  int chunk = h / n;
  int extra = h - chunk * n;
  int small = 5;
  int large = 6;
  return (AllBounds) {
    .top = GRect(x, y, w, small * chunk), 
    .middle = GRect(x, y + small * chunk, w, large * chunk + extra), 
    .bottom = GRect(x, h - small * chunk, w, small * chunk) 
  };
}

void
draw_main(Layer *layer, GContext *ctx) {
  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  GRect bounds = layer_get_bounds(layer);
  AllBounds all = bounds_calc(bounds);
  draw_date(ctx, &fctx, 32, all.top);
  draw_time(ctx, &fctx, 36, all.middle);
  draw_step(ctx, &fctx, 32, all.bottom);
  fctx_deinit_context(&fctx);
}