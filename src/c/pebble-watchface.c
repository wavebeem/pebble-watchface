#include <pebble.h>
#include "state.h"
#include "draw.h"

// TODO: Switch to TextLayers rather than drawing text manually.
// https://developer.pebble.com/docs/c/User_Interface/Layers/TextLayer/

static Window *s_main_window = 0;

static Layer *s_window_layer = 0;
static Layer *s_layer_time = 0;
static Layer *s_layer_date = 0;
static Layer *s_layer_steps = 0;
static Layer *s_layer_battery = 0;

static void
update_step_count() {
  STATE.steps = (int)health_service_sum_today(HealthMetricStepCount);
}

static void
health_handler(HealthEventType event, void *_context) {
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
  STATE.hours = tick_time->tm_hour;
  STATE.minutes = tick_time->tm_min;
  STATE.date = tick_time->tm_mday;
  STATE.month = tick_time->tm_mon + 1;
  if (STATE.minutes == 0) {
    vibes_short_pulse();
  }
  safe_mark_dirty(s_window_layer);
}

static void
battery_handler(BatteryChargeState c) {
  STATE.battery_percent = c.charge_percent;
  safe_mark_dirty(s_window_layer);
}

static void
window_load(Window *window) {
  s_window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(s_window_layer);
  const int window_w = window_bounds.size.w;
  // const int window_h = window_bounds.size.h;

  s_layer_date = layer_create(GRect(0, 0, window_w, 40));
  layer_set_update_proc(s_layer_date, draw_date);
  layer_add_child(s_window_layer, s_layer_date);

  s_layer_time = layer_create(GRect(0, 40, window_w, 60));
  layer_set_update_proc(s_layer_time, draw_time);
  layer_add_child(s_window_layer, s_layer_time);

  s_layer_steps = layer_create(GRect(0, 100, window_w, 40));
  layer_set_update_proc(s_layer_steps, draw_steps);
  layer_add_child(s_window_layer, s_layer_steps);

  s_layer_battery = layer_create(GRect(0, 120, window_w, 40));
  layer_set_update_proc(s_layer_battery, draw_battery);
  layer_add_child(s_window_layer, s_layer_battery);
}

static void
window_unload(Window *window) {
  layer_destroy(s_layer_time);
  layer_destroy(s_layer_date);
  layer_destroy(s_layer_steps);
  layer_destroy(s_layer_battery);
}

static WindowHandlers window_handlers = {
  .load = window_load,
  .unload = window_unload
};

static void
init_time() {
  time_t t = time(NULL);
  struct tm *time_now = localtime(&t);
  tick_handler(time_now, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void
init_window() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, STATE.color_bg);
  window_set_window_handlers(s_main_window, window_handlers);
  window_stack_push(s_main_window, true);
}

static void
init_steps() {
  health_service_events_subscribe(health_handler, NULL);
}

static void
init_battery() {
  STATE.battery_percent = battery_state_service_peek().charge_percent;
  battery_state_service_subscribe(battery_handler);
}

static void
init_colors() {
  STATE.color_time = GColorWhite;
  STATE.color_date = GColorRajah;
  STATE.color_batt = GColorInchworm;
  STATE.color_steps = GColorRajah;
  STATE.color_bg = GColorBlack;
}

static void
init_fonts() {
  STATE.font_secondary = fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS);
  STATE.font_primary = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
}

static void
init() {
  STATE.hours = 0;
  STATE.minutes = 0;
  STATE.month = 1;
  STATE.date = 1;
  STATE.steps = -1;
  init_fonts();
  init_colors();
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
