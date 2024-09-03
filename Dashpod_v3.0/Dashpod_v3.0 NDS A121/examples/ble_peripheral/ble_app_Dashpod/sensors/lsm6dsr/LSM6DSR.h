#include <stdio.h>
#include <stdint.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

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

 void setup_gyro_accel(const nrf_drv_twi_t* m_twi,uint8_t addr);

 void setup_gyro(const nrf_drv_twi_t* m_twi,uint8_t addr);

 void setup_Tap_INT(const nrf_drv_twi_t* m_twi,uint8_t addr);

 void clear_Tap_INT(const nrf_drv_twi_t* m_twi,uint8_t addr);

 void setup_accel_sensitivity(uint8_t sens);

 void read_gyro_accel (const nrf_drv_twi_t* m_twi,uint8_t addr,uint8_t *dataLH);

 void read_gyro(const nrf_drv_twi_t* m_twi,uint8_t addr,uint8_t *dataLH);

 void read_accel(const nrf_drv_twi_t* m_twi,uint8_t addr,uint8_t *dataLH);

 void process_accel_gyro(short int *raw_data,float *proc_data);

 void process_raw_accel(short int *raw_data,float *proc_data);

 void process_gyro(short int *raw_data,float *proc_data);

 void i2c_scanner(const nrf_drv_twi_t* m_twi,uint8_t addr);

 bool is_tap_detected(const nrf_drv_twi_t* m_twi);

 void tap_enable(const nrf_drv_twi_t* m_twi);