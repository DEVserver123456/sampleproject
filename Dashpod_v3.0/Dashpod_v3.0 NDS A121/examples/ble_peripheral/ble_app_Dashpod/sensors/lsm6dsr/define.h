#include "sdk_config.h"
/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif

#define MS_OR_US 0
#define DEL 75

#if MS_OR_US
#define delay  nrf_delay_ms
#elif !MS_OR_US
#define delay  nrf_delay_us
#endif

/* Number of possible TWI addresses. */
#define TWI_ADDRESSES     127

#define LED_WS2812B NRF_GPIO_PIN_MAP(0,4)