#include "twi_init.h"
#include "nrf_drv_twi.h"

#include "nrf_gpio.h"

const nrf_drv_twi_t m_twi_init = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

static uint8_t twi_initialized = 0;

int is_twi_initialized(void)
{
        return twi_initialized;

}
 
void twi_init(void)
{
        if(twi_initialized == 0)
        {
          
    
                ret_code_t err_code;

                const nrf_drv_twi_config_t twi_config = {
                   .scl                = NRF_GPIO_PIN_MAP(1,9),//33
                   .sda                = NRF_GPIO_PIN_MAP(0,11),//11
                   .frequency          = NRF_DRV_TWI_FREQ_400K,
                   .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
                   .clear_bus_init     = true
                };

                err_code = nrf_drv_twi_init(&m_twi_init, &twi_config, NULL, NULL);
                APP_ERROR_CHECK(err_code);

                nrf_drv_twi_enable(&m_twi_init);
        }
        twi_initialized = 1;
        #ifdef DEBUG
        printf("TWI Initialized in twi.c file\n");
        #endif
}

void twi_uninit(void)
{
      nrf_drv_twi_uninit(&m_twi_init);
      printf("Uninitialized\n");
}