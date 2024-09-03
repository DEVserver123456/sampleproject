#include "nrfx.h"
/*
#ifndef TWI_INIT_H
#define TWI_INIT_H

int is_twi_initialized();
void twi_init(void);
#endif
*/
int is_twi_initialized();

void twi_init(void);

void twi_uninit(void);

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
