#include <stdio.h>
#include <stdint.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif

#define MS_OR_US 0
#define DEL 200

#if MS_OR_US
#define delay  nrf_delay_ms
#elif !MS_OR_US
#define delay  nrf_delay_us
#endif

/* Number of possible TWI addresses. */
#define TWI_ADDRESSES     127

uint8_t address;
uint8_t sample_data[2];

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/**
 * @brief TWI initialization.
 */
void twi_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
        .scl = NRF_GPIO_PIN_MAP(1, 9),  // 33
        .sda = NRF_GPIO_PIN_MAP(0, 11), // 11
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = true
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

int main(void)
 {
    ret_code_t err_code;
    uint8_t data;
    uint16_t datax,datay,dataz;
    bool detected_device = false;
  //  freopen("debug:stdout", "w", stdout);
    float Gyro_X, Gyro_Y, Gyro_Z,Accel_X,Accel_Y,Accel_Z;
    char red,green,blue;

    twi_init();
printf("sample data = %E\n\n\n....\n",Gyro_X);

    for (address = 0x60; address <= TWI_ADDRESSES; address++)
    {
        err_code = nrf_drv_twi_rx(&m_twi, address, &data, sizeof(data));
        nrf_delay_ms(100);
        if (err_code == NRF_SUCCESS)
        {
            detected_device = true;
            printf("\nTWI device detected at address 0x%x.", address);
            printf("\nreceived data %c\n", data);
            break;
        }
    }

    if (!detected_device)
    {
        printf("No device was found.\n");
    }
  delay(DEL+1000000);
    address = 0x6b;
    uint8_t act_g[] ={ 0x11 , 0x10 };
      uint8_t act_a[] ={ 0x10 , 0x10 };
    uint8_t arr[] ={0x28, 0x29 , 0x2a , 0x2b , 0x2c , 0x2d };
    uint8_t arrx[] =  {0x22, 0x23 , 0x24 , 0x25 , 0x26 , 0x27 };
  uint8_t xl_off[] ={0x73,0xef};
uint8_t yl_off[] ={0x74,0xef};
uint8_t zl_off[] ={0x75,0xef};
uint8_t dataL, dataH;

    nrf_drv_twi_tx(&m_twi,address,xl_off,2,false);
    nrf_drv_twi_tx(&m_twi,address,yl_off,2,false);
    nrf_drv_twi_tx(&m_twi,address,zl_off,2,false);

     nrf_drv_twi_tx(&m_twi, address, act_g,2, false);
     nrf_drv_twi_tx(&m_twi, address, act_a,2, false);

   // nrf_drv_twi_xfer_desc_t m_xfer_data;
   // nrf_drv_twi_xfer_type_t m_type;
 //   m_type.
    
    //m_xfer_data = NRF_DRV_TWI_XFER_DESC_TXRX(address,&arr[0],1,&arr[1],1);
     
      /* nrf_drv_twi_xfer_desc_t m_xfer_data= {
       .type = NRF_DRV_TWI_XFER_TXRX,                                 \
        .address = address,                                               \
        .primary_length   = 1,                                    \
        .secondary_length = 1,                                    \
        .p_primary_buf    = &arr[0],                                      \
        .p_secondary_buf  = &arr[1],  
                                           \
    };*/
    while (true)
    {
    /* nrf_drv_twi_xfer_desc_t m_xfer_data = NRF_DRV_TWI_XFER_DESC_TXRX(address,&arr[0],sizeof(arr[0]),&dataH,sizeof(dataH));
      data = nrf_drv_twi_xfer(&m_twi,&m_xfer_data,0);
      delay(DEL+1000000);
      nrf_drv_twi_xfer_desc_t m_xfer_data1 = NRF_DRV_TWI_XFER_DESC_TXRX(address,&arr[1],sizeof(arr[1]),&dataL,sizeof(dataL));
      data = nrf_drv_twi_xfer(&m_twi,&m_xfer_data1,0);
       delay(DEL);*/
/*
        delay(DEL+1000000);
        nrf_drv_twi_tx(&m_twi, address, &arr[0], 1, true); // Send first register address
        nrf_drv_twi_rx(&m_twi, address, &dataL, 1); // Receive data from the first register
        delay(5); // Wait for some time
        nrf_drv_twi_tx(&m_twi, address, &arr[1], 1, true); // Send second register address with a repeat start
        nrf_drv_twi_rx(&m_twi, address,&dataH, 1); // Receive data from the second register
        delay(DEL); // Wait for some time
         // Wait for some time before the next iteration
        datax =(~((dataL | dataH << 8) - 1));
        Gyro_X=(float)datax/131.0;
        //printf("%u",sizeof(int));
        printf("x_gyro = %.3f\t\t", Gyro_X);

        // delay(DEL+1000000);
        nrf_drv_twi_tx(&m_twi, address, &arr[2], 1, true); // Send first register address
        nrf_drv_twi_rx(&m_twi, address, &dataL, 1); // Receive data from the first register
        delay(5); // Wait for some time
        nrf_drv_twi_tx(&m_twi, address, &arr[3], 1, true); // Send second register address with a repeat start
        nrf_drv_twi_rx(&m_twi, address,&dataH, 1); // Receive data from the second register
        delay(DEL); // Wait for some time
         // Wait for some time before the next iteration
        datay = ~((dataL | dataH << 8) - 1);
        Gyro_Y=datay/131.0;
        printf("y_gyro = %.3f\t\t",Gyro_Y );


        // delay(DEL+1000000);
        nrf_drv_twi_tx(&m_twi, address, &arr[4], 1, true); // Send first register address
        nrf_drv_twi_rx(&m_twi, address, &dataL, 1); // Receive data from the first register
        delay(5); // Wait for some time
        nrf_drv_twi_tx(&m_twi, address, &arr[5], 1, true); // Send second register address with a repeat start
        nrf_drv_twi_rx(&m_twi, address,&dataH, 1); // Receive data from the second register
        delay(DEL); // Wait for some time
         // Wait for some time before the next iteration
        dataz = ~((dataL | dataH << 8) - 1);
        Gyro_Z=dataz/131.0;
        printf("z_gyro = %.3f \n", Gyro_Z);
        
*/
        //------------------------------------------------------------------------------------

         delay(DEL+1000000);
        nrf_drv_twi_tx(&m_twi, address, &arrx[0], 1, true); // Send first register address
        nrf_drv_twi_rx(&m_twi, address, &dataL, 1); // Receive data from the first register
        delay(5); // Wait for some time
        nrf_drv_twi_tx(&m_twi, address, &arrx[1], 1, true); // Send second register address with a repeat start
        nrf_drv_twi_rx(&m_twi, address,&dataH, 1); // Receive data from the second register
        delay(DEL); // Wait for some time
         // Wait for some time before the next iteration
        datax = ~((dataL | dataH << 8) - 1);
        red=((float)datax/65535)*255;
        //Accel_X=datax/16384.0;
        printf("x_acc = %x\t",red );

        // delay(DEL+1000000);
        nrf_drv_twi_tx(&m_twi, address, &arrx[2], 1, true); // Send first register address
        nrf_drv_twi_rx(&m_twi, address, &dataL, 1); // Receive data from the first register
        delay(5); // Wait for some time
        nrf_drv_twi_tx(&m_twi, address, &arrx[3], 1, true); // Send second register address with a repeat start
        nrf_drv_twi_rx(&m_twi, address,&dataH, 1); // Receive data from the second register
        delay(DEL); // Wait for some time
         // Wait for some time before the next iteration
        datay = ~((dataL | dataH << 8) - 1);
        green=((float)datay/65535)*255;
        //Accel_Y=datay/16384.0;
        printf("y_acc = %x\t", green);


         //delay(DEL+1000000);
        nrf_drv_twi_tx(&m_twi, address, &arrx[4], 1, true); // Send first register address
        nrf_drv_twi_rx(&m_twi, address, &dataL, 1); // Receive data from the first register
        delay(5); // Wait for some time
        nrf_drv_twi_tx(&m_twi, address, &arrx[5], 1, true); // Send second register address with a repeat start
        nrf_drv_twi_rx(&m_twi, address,&dataH, 1); // Receive data from the second register
        delay(DEL); // Wait for some time
         // Wait for some time before the next iteration
        dataz = ~((dataL | dataH << 8) - 1);
        blue=((float)dataz/65535)*255;
        blue=~blue;
       // Accel_Z=dataz/16384.0;
        printf("z_acc = %x\n", blue);
        
        
    }
    }


