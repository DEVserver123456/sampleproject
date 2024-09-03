
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <nrfx.h>

#ifndef IR_ENABLED
#define IR_ENABLED                    1
#endif

#ifndef MEMS_ENABLED
#define MEMS_ENABLED                  1
#endif

#ifndef LED_7_SEG_ENABLED
#define LED_7_SEG_ENABLED             1
#endif

#ifndef LED_COLOR_ENABLED
#define LED_COLOR_ENABLED             1
#endif

#ifndef BUZZER_ENABLED
#define BUZZER_ENABLED                1
#endif

#ifndef BATTERY_ENABLED
#define BATTERY_ENABLED               0
#endif

#ifndef IO_EXPANDER_ENABLED
#define IO_EXPANDER_ENABLED           1
#endif

#ifndef BQ2505_BATTERY_ENABLED
#define BQ2505_BATTERY_ENABLED        1
#endif

#ifndef MAX17260_FUEL_GUAGE_ENABLED
#define MAX17260_FUEL_GUAGE_ENABLED   1
#endif

#endif
