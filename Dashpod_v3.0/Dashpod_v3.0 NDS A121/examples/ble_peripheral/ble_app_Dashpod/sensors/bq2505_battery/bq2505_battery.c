
#include "bq2505_battery.h"
#include "app_config.h"

#if NRFX_CHECK(BQ2505_BATTERY_ENABLED)
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"

#define BATTERY_PULSE_WIDTH_INTERVAL     4//APP_TIMER_TICKS(1)                 /**< Battery pulse width interval (ticks). This value corresponds to 200 micro seconds. */

APP_TIMER_DEF(m_bq2505_battery_timer_id);                      /**< Battery measurement timer. */

uint8_t current_limit_mode;
uint8_t current_limit_mode_count=0;
bool ctrl_pin_state=false;

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	/*  charging state change */
}

void battery_pulse_width_timeout_handler(void * p_context)
{
   if(ctrl_pin_state)
   {	
     nrfx_gpiote_out_clear(BQ2505_CTRL_PIN);
	 ctrl_pin_state=false;
	 if(current_limit_mode_count == current_limit_mode)
	 {
	   uint32_t err_code = app_timer_stop(m_bq2505_battery_timer_id);
		APP_ERROR_CHECK(err_code); 
                current_limit_mode_count = 0;
	 }
   }
   else
   {
	 nrfx_gpiote_out_set(BQ2505_CTRL_PIN);
	 ctrl_pin_state=true;
	 current_limit_mode_count++;
   }
}

void bq2505_set_current_limit_mode(uint8_t mode)
{	
  //if(mode >= BQ2505_CURRENT_LIMIT_MODE_93mA && mode <= BQ2505_CURRENT_LIMIT_MODE_935mA)
  {
    current_limit_mode = mode;
	//nrfx_gpiote_out_set(BQ2505_CTRL_PIN);
	ctrl_pin_state=true;
	//current_limit_mode_count = 1;
	ret_code_t  err_code = app_timer_start(m_bq2505_battery_timer_id, BATTERY_PULSE_WIDTH_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code); 
  }
}

uint8_t bq2505_get_current_limit_mode()
{	
  return current_limit_mode;
}
	
bool bq2505_battery_status()
{
  nrf_gpio_pin_sense_t sense = nrf_gpio_pin_read(BQ2505_STATUS_PIN);
  if(sense == NRF_GPIO_PIN_SENSE_LOW)
  {
	  return false; /*  battery charging */
  }
  else if(sense == NRF_GPIO_PIN_SENSE_HIGH)
  {
	  return true; /*  battery not charging */
  }
  return false;
}

/**@brief Function for configuring ADC to do battery level conversion.
 */
void bq2505_battery_init()
{ 
  ret_code_t err_code;
  if (!nrf_drv_gpiote_is_init())
  {
    err_code = nrf_drv_gpiote_init();
  }

  nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

  err_code = nrf_drv_gpiote_out_init(BQ2505_CTRL_PIN, &out_config);
  APP_ERROR_CHECK(err_code);
/*
  nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
  in_config.pull = NRF_GPIO_PIN_PULLUP;

  err_code = nrf_drv_gpiote_in_init(BQ2505_STATUS_PIN, &in_config, in_pin_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_event_enable(BQ2505_STATUS_PIN, true);
*/
  // Create battery timer.
  app_timer_init();
  err_code = app_timer_create(&m_bq2505_battery_timer_id, APP_TIMER_MODE_REPEATED,battery_pulse_width_timeout_handler);

  APP_ERROR_CHECK(err_code);
}
#endif
