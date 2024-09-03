#include <stdio.h>
#include <stdint.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "LSM6DSR.h"
#include "led_ws2812b.h"

#ifndef I2C_def
#define I2C_def

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
#endif
/* Number of possible TWI addresses. */

#define IO_EX_EN 0xE6




  /* TWI instance. */
//static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);


/**
 * @brief TWI initialization.
 */

void sound1(float j);

void sound2(int j);

void sound3(int j);

void sound4(int j);

void sound5(int j);

void stop();

void tone1();

void tone2();

void tone3();

void tone4();

void Beep();

void IO_buzz_init(const nrf_drv_twi_t* m_twi_r,uint8_t address);

void buzzer_tone(void);

void buzzer_tone1(void);

void A121_IO_PS_ON();

void LED_LS(bool funtion);

bool chrg_pin_read();

void set_IO_pin(uint8_t pin);

void clear_IO_pin(uint8_t pin);