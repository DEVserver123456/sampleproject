
#include "app_config.h"
#include "nrf_gpio.h"
//#include "app_dashpod.h"

#if NRFX_CHECK(BQ2505_BATTERY_ENABLED)

#define BQ2505_CURRENT_LIMIT_MODE_93mA		4
#define BQ2505_CURRENT_LIMIT_MODE_187mA		5
#define BQ2505_CURRENT_LIMIT_MODE_280mA		6
#define BQ2505_CURRENT_LIMIT_MODE_374mA		7
#define BQ2505_CURRENT_LIMIT_MODE_467mA		8
#define BQ2505_CURRENT_LIMIT_MODE_654mA		9
#define BQ2505_CURRENT_LIMIT_MODE_794mA		10
#define BQ2505_CURRENT_LIMIT_MODE_935mA		11

#define BQ2505_CTRL_PIN                         NRF_GPIO_PIN_MAP(0,5)
#define BQ2505_STATUS_PIN			30

void bq2505_battery_init();
void bq2505_set_current_limit_mode(uint8_t);
uint8_t bq2505_get_current_limit_mode();
bool bq2505_battery_status();
#endif
