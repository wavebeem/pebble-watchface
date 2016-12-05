#include "state.h"

bool
should_vibrate_hourly() {
  return enamel_get_VibrateHourly();
}

bool
is_date_format_MMDD() {
  return enamel_get_DateFormat() == DATEFORMAT_MM_DD;
}

State STATE;