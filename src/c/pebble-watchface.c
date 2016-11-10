#include <pebble.h>

// TODO: Switch to TextLayers rather than drawing text manually.
// https://developer.pebble.com/docs/c/User_Interface/Layers/TextLayer/

static Window *s_main_window = 0;
static Layer *s_canvas_layer = 0;
static struct tm *time_now = 0;
static BatteryChargeState battery_charge;

static GColor8 color_time;
static GColor8 color_date;
static GColor8 color_batt;
static GColor8 color_steps;
static GColor8 color_bg;

static int hours = 0;
static int minutes = 0;
static int month = 1;
static int date = 1;
static int step_count = -1;

const char *EMOJI_PUCKERED = "\U0001f617";
const char *EMOJI_SMILE = "\U0001f60a";

// static bool
// step_data_is_available() {
//   const time_t start = time_start_of_today();
//   const time_t end = time(NULL)
//   return HealthServiceAccessibilityMaskAvailable &
//     health_service_metric_accessible(HealthMetricStepCount, start, end);
// }

static void
update_step_count() {
  step_count = (int)health_service_sum_today(HealthMetricStepCount);
}

static void
health_handler(HealthEventType event, void *context) {
  if (event != HealthEventSleepUpdate) {
    update_step_count();
  }
}

static void
safe_mark_dirty(Layer *l) {
  if (l) {
    layer_mark_dirty(l);
  }
}

static void
tick_handler(struct tm *tick_time, TimeUnits _changed) {
  hours = tick_time->tm_hour;
  minutes = tick_time->tm_min;
  date = tick_time->tm_mday;
  month = tick_time->tm_mon + 1;
  if (minutes == 0) {
    vibes_short_pulse();
  }
  safe_mark_dirty(s_canvas_layer);
}

static void
battery_handler(BatteryChargeState c) {
  battery_charge = c;
  safe_mark_dirty(s_canvas_layer);
}

// static void
// draw_battery_line(GRect bounds, Layer *layer, GContext *ctx) {
//   uint8_t percent = battery_charge.charge_percent;
//   // TODO: Handle .is_charging
//   // TODO: Handle .is_plugged
//   const int x = 0;
//   const int y = bounds.size.h - 10;
//   const int w =(bounds.size.w * percent) / 100;
//   const int h = 10; 
//   GRect frame = GRect(x, y, w, h);
//   graphics_context_set_fill_color(ctx, color_batt);
//   graphics_fill_rect(ctx, frame, 0, GCornerNone);
// }

static void
draw_battery_chunks(GRect bounds, Layer *layer, GContext *ctx) {
  const int gap = 3;
  const int radius = 4;
  uint8_t percent = battery_charge.charge_percent;
  const int n = percent / 10;
  for (int i = 0; i < 10; i++) {
    graphics_context_set_stroke_width(ctx, 2);
    graphics_context_set_stroke_color(ctx, color_batt);
    graphics_context_set_fill_color(ctx, color_batt);
    const GPoint point = {
      radius + 2 * (radius + gap) * i,
      bounds.size.h - 10
    };
    if (i <= n) {
      graphics_draw_circle(ctx, point, radius);
      graphics_fill_circle(ctx, point, radius);
    } else {
      graphics_draw_circle(ctx, point, radius);
    }
  }
}

static void
draw_time(GRect bounds, Layer *layer, GContext *ctx) {
  GRect frame = GRect(0, 50, bounds.size.w, 50);
  char str[255];
  snprintf(str, sizeof str, "%02d:%02d", hours, minutes);
  GColor text_color = gcolor_legible_over(color_time);
  graphics_context_set_text_color(ctx, text_color);
  graphics_context_set_fill_color(ctx, color_time);
  graphics_fill_rect(ctx, frame, 0, GCornerNone);
  graphics_draw_text(
    ctx,
    str,
    fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    // fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49),
    frame,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter,
    NULL
  );
}

static void
draw_date(GRect bounds, Layer *layer, GContext *ctx) {
  GRect frame = GRect(0, 10, bounds.size.w, 50);
  char str[255];
  snprintf(str, sizeof str, "%02d/%02d", month, date);
  graphics_context_set_text_color(ctx, color_date);
  graphics_draw_text(
    ctx,
    str,
    fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS),
    frame,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter,
    NULL
  );
}

static void
draw_steps(GRect bounds, Layer *layer, GContext *ctx) {
  if (step_count == -1) {
    return;
  }
  GRect frame = GRect(0, 100, bounds.size.w, 50);
  const char *emoji = EMOJI_SMILE;
  char str[255];
  // snprintf(str, sizeof str, "%d %s", step_count, emoji);
  snprintf(str, sizeof str, "%d", step_count);
  graphics_context_set_text_color(ctx, color_steps);
  graphics_draw_text(
    ctx,
    str,
    fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS),
    frame,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter,
    NULL
  );
}

static void
update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_antialiased(ctx, true);
  draw_time(bounds, layer, ctx);
  draw_date(bounds, layer, ctx);
  draw_steps(bounds, layer, ctx);
  draw_battery_chunks(bounds, layer, ctx);
}

static void
window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_canvas_layer = layer_create(window_bounds);
  layer_set_update_proc(s_canvas_layer, update_proc);
  layer_add_child(window_layer, s_canvas_layer);
}

static void
window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

static WindowHandlers window_handlers = {
  .load = window_load,
  .unload = window_unload
};

static void
init_time() {
  time_t t = time(NULL);
  time_now = localtime(&t);
  tick_handler(time_now, MINUTE_UNIT);
}

static void
init_window() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, color_bg);
  window_set_window_handlers(s_main_window, window_handlers);
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void
init_steps() {
  health_service_events_subscribe(health_handler, NULL);
}

static void
init_battery() {
  battery_charge = battery_state_service_peek();
  battery_state_service_subscribe(battery_handler);
}

static void
init() {
  srand(time(NULL));
  color_time = GColorWhite;
  color_date = GColorRichBrilliantLavender;
  color_batt = GColorRajah;
  color_steps = GColorMelon;
  color_bg = GColorBlack;
  init_time();
  init_battery();
  init_steps();
  init_window();
}

static void
deinit() {
  window_destroy(s_main_window);
}

int
main() {
  init();
  app_event_loop();
  deinit();
}
