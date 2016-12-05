#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "state.h"
#include "draw.h"

// Convert TTF font to fctx FFont:
// - Use TTF to SVG font converter
// - ./node_modules/.bin/fctx-compiler <YOUR_FONT>.svg -r "[0-9:./ -]"

static Window *s_main_window = NULL;

static Layer *s_layer_main = NULL;
static Layer *s_window_layer = NULL;


static void
update_step_count() {
  STATE.steps = (int)health_service_sum_today(HealthMetricStepCount);
  // STATE.steps = 808908;
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
tick_handler(struct tm *tick, TimeUnits _changed) {
  STATE.hours = tick->tm_hour;
  STATE.minutes = tick->tm_min;
  STATE.date = tick->tm_mday;
  STATE.month = tick->tm_mon + 1;
  STATE.seconds = tick->tm_sec;
  if (STATE.minutes == 0 && STATE.seconds == 0) {
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
main_window_load(Window *window) {
  s_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(s_window_layer);
  s_layer_main = layer_create(bounds);
  layer_set_update_proc(s_layer_main, draw_main);
  layer_add_child(s_window_layer, s_layer_main);
}

static void
main_window_unload(Window *window) {
  layer_destroy(s_layer_main);
}

static WindowHandlers main_window_handlers = {
  .load = main_window_load,
  .unload = main_window_unload
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
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, main_window_handlers);
  window_stack_push(s_main_window, true);
}

static void
init_steps() {
  STATE.steps = (int)health_service_sum_today(HealthMetricStepCount);
  health_service_events_subscribe(health_handler, NULL);
}

static void
init_battery() {
  STATE.battery_percent = battery_state_service_peek().charge_percent;
  // STATE.battery_percent = 30;
  battery_state_service_subscribe(battery_handler);
}

static void
init_fonts() {
  // STATE.font_noto_sans_regular =
  //   ffont_create_from_resource(RESOURCE_ID_NOTO_SANS_REGULAR_FFONT);
  STATE.font_noto_sans_bold =
    ffont_create_from_resource(RESOURCE_ID_NOTO_SANS_BOLD_FFONT);
}

static void
init() {
  init_fonts();
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
