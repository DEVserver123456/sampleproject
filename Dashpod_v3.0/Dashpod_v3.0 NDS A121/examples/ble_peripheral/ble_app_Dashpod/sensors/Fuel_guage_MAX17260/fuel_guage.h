#include <stdio.h>
#include <stdint.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

void setup_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr,uint16_t mAh);

uint8_t read_percentage_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr);

float read_capacity_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr);

float read_Full_capacity_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr);

void set_i2c_parameter(const nrf_drv_twi_t* m_twi,uint8_t addr);

uint8_t read_batt_per();

float read_batt_cap();

float read_batt_full_cap();