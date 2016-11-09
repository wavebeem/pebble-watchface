#include <pebble.h>

static Window *s_main_window = 0;
static Layer *s_canvas_layer = 0;
static struct tm *time_now = 0;
static BatteryChargeState battery_charge;

static GColor8 color_text;
static GColor8 color_batt;
static GColor8 color_steps;
static GColor8 color_bg;

static int hours = 0;
static int minutes = 0;
static int step_count = -1;

static char time_str[255];
static char step_str[255];

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

static void
draw_battery(GRect bounds, Layer *layer, GContext *ctx) {
  uint8_t percent = battery_charge.charge_percent;
  // TODO: Handle .is_charging
  // TODO: Handle .is_plugged
  const int x = 0;
  const int y = bounds.size.h - 10;
  const int w =(bounds.size.w * percent) / 100;
  const int h = 10; 
  GRect frame = GRect(x, y, w, h);
  graphics_context_set_fill_color(ctx, color_batt);
  graphics_fill_rect(ctx, frame, 0, GCornerNone);
}

static void
draw_time(GRect bounds, Layer *layer, GContext *ctx) {
  GRect frame = GRect(0, 0, bounds.size.w, 50);
  snprintf(time_str, sizeof time_str, "%02d:%02d", hours, minutes);
  graphics_context_set_text_color(ctx, color_text);
  graphics_draw_text(
    ctx,
    time_str,
    fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    // fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49),
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
  GRect frame = GRect(0, 60, bounds.size.w, 50);
  snprintf(step_str, sizeof step_str, "%d", step_count);
  graphics_context_set_text_color(ctx, color_steps);
  graphics_draw_text(
    ctx,
    step_str,
    fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS),
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
  draw_steps(bounds, layer, ctx);
  draw_battery(bounds, layer, ctx);
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
  color_text = GColorWhite;
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
