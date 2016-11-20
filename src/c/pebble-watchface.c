#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "state.h"
#include "draw.h"

// TODO: Switch to TextLayers rather than drawing text manually.
// https://developer.pebble.com/docs/c/User_Interface/Layers/TextLayer/

// Convert TTF font to fctx FFont:
// - Use TTF to SVG font converter
// - ./node_modules/.bin/fctx-compiler <YOUR_FONT>.svg -r "[0-9:./ -]"

static Window *s_main_window = NULL;
static Window *s_detail_window = NULL;

static Layer *s_layer_main = NULL;
static TextLayer *s_text_layer_detail = NULL;
static Layer *s_window_layer = NULL;

static bool s_is_detail_window_showing = false;

static char detail_text[1024];

// Ugh this detail stuff should be in its own stuff with a header file...
// I really don't like declaring this stuff ahead of time
static void show_detail_window();
static void hide_detail_window();

static void
tap_handler(AccelAxisType axis, int32_t direction) {
  show_detail_window();
}

static void
init_tap_service() {
  accel_tap_service_subscribe(tap_handler);
}

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

static void
update_detail_text() {
  snprintf(
    detail_text,
    sizeof detail_text,
    "18:06\n"
    "%02d/%02d\n"
    "%d%%\n"
    // "charging\n"
    "%d",
    STATE.month,
    STATE.date,
    STATE.battery_percent,
    STATE.steps
  );
}

static void
detail_window_load(Window *window) {
  s_window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(s_window_layer);
  GRect bounds = grect_inset(window_bounds, GEdgeInsets(10));
  s_text_layer_detail = text_layer_create(bounds);
  text_layer_set_text_alignment(s_text_layer_detail, GTextAlignmentCenter);
  text_layer_set_font(s_text_layer_detail, STATE.gfont_gothic);
  update_detail_text();
  text_layer_set_text(s_text_layer_detail, detail_text);
  Layer *layer = text_layer_get_layer(s_text_layer_detail);
  layer_add_child(s_window_layer, layer);
}

static void
detail_window_unload(Window *window) {
  text_layer_destroy(s_text_layer_detail);
}

static WindowHandlers main_window_handlers = {
  .load = main_window_load,
  .unload = main_window_unload
};

static WindowHandlers detail_window_handlers = {
  .load = detail_window_load,
  .unload = detail_window_unload
};

static void
show_detail_window() {
  if (!s_is_detail_window_showing) {
    window_stack_push(s_detail_window, true);
    s_is_detail_window_showing = true;
    app_timer_register(3000, hide_detail_window, NULL);
  }
}

static void
hide_detail_window() {
  if (s_is_detail_window_showing) {
    window_stack_pop(true);
    s_is_detail_window_showing = false;
  }
}

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

  s_detail_window = window_create();
  window_set_background_color(s_detail_window, GColorWhite);
  window_set_window_handlers(s_detail_window, detail_window_handlers);
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
init_fonts() {
  STATE.gfont_gothic =
    fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  STATE.font_noto_sans =
    ffont_create_from_resource(RESOURCE_ID_NOTO_SANS_REGULAR_FFONT);
}

static void
init() {
  STATE.hours = 0;
  STATE.minutes = 0;
  STATE.month = 1;
  STATE.date = 1;
  STATE.steps = -1;
  init_fonts();
  init_time();
  init_battery();
  init_steps();
  init_window();
  init_tap_service();
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
