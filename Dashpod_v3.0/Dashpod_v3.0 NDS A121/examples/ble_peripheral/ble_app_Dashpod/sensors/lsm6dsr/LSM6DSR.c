#include "LSM6DSR.h"
#include "twi_init.h"

//uint8_t sample_data[2];
uint8_t arr[] ={0x28, 0x29 , 0x2a , 0x2b , 0x2c , 0x2d };
uint8_t arrx[] =  {0x22, 0x23 , 0x24 , 0x25 , 0x26 , 0x27 };
uint8_t dataL, dataH;
uint8_t axis = 2;
 // uint8_t dataLH[12];
 volatile int accel_sens = 9;
uint16_t datax,datay,dataz;
float Gyro_X, Gyro_Y, Gyro_Z,Accel_X,Accel_Y,Accel_Z;
bool detected_device = false; 
ret_code_t err_code;
static  nrf_drv_twi_t m_twi_g;
uint8_t addr_g;
static uint8_t sensitivity = 1;

#define SINGLE_TAP_BIT 5


void setup_gyro_accel(const nrf_drv_twi_t* m_twi,uint8_t addr)
{
        uint8_t act_a[] ={ 0x10, 0x64 };
        uint8_t regv;

        if(!is_twi_initialized()) 
        {
            twi_init();
            #ifdef DEBUG
            printf("TWI initialized\n");
            #endif
        }

        m_twi_g = *m_twi;
        addr_g  = addr;
   
        (nrf_drv_twi_tx(m_twi, addr, act_a,2, false)); 
}

void setup_gyro(const nrf_drv_twi_t* m_twi,uint8_t addr)
{
  
        uint8_t act_g[] ={ 0x11, 0x60 };
        uint8_t regv; 
        
        (nrf_drv_twi_tx(m_twi, addr, act_g,2, false));
}


void setup_Tap_INT(const nrf_drv_twi_t* m_twi,uint8_t addr)
{
        //uint8_t CTRL1_XL[] = {0x10, 0x6C};
        uint8_t TAP_CFG0[] = {0x56, 0x0E};
        uint8_t TAP_CFG1[] = {0x57, 0x70};
        uint8_t TAP_CFG2[] = {0x58, 0x8C};
        uint8_t TAP_THRS_6D[] = {0x59, 0x0C};
        uint8_t INT_DUR2[] = {0x5A, 0x7F};
        uint8_t WAKE_UP_THRS[] = {0x5B, 0x80};
        uint8_t MD2_CFG[] = {0x5E, 0x08};
        
        //(nrf_drv_twi_tx(m_twi, addr, CTRL1_XL, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, TAP_CFG0, 2, false));
        (nrf_drv_twi_tx(m_twi, addr, TAP_CFG1, 2, false));
        (nrf_drv_twi_tx(m_twi, addr, TAP_CFG2, 2, false));
        (nrf_drv_twi_tx(m_twi, addr, TAP_THRS_6D, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, INT_DUR2, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, WAKE_UP_THRS, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, MD2_CFG, 2, false)); 
}

void clear_Tap_INT(const nrf_drv_twi_t* m_twi,uint8_t addr)
{
        uint8_t CTRL1_XL[] = {0x10, 0x00};
        uint8_t TAP_CFG0[] = {0x56, 0x00};
        uint8_t TAP_CFG1[] = {0x57, 0x00};
        uint8_t TAP_CFG2[] = {0x58, 0x00};
        uint8_t TAP_THRS_6D[] = {0x59, 0x00};
        uint8_t INT_DUR2[] = {0x5A, 0x00};
        uint8_t WAKE_UP_THRS[] = {0x5B, 0x00};
        uint8_t MD2_CFG[] = {0x5E, 0x00};
        
        (nrf_drv_twi_tx(m_twi, addr, CTRL1_XL, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, TAP_CFG0, 2, false));
        (nrf_drv_twi_tx(m_twi, addr, TAP_CFG1, 2, false));
        (nrf_drv_twi_tx(m_twi, addr, TAP_CFG2, 2, false));
        (nrf_drv_twi_tx(m_twi, addr, TAP_THRS_6D, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, INT_DUR2, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, WAKE_UP_THRS, 2, false)); 
        (nrf_drv_twi_tx(m_twi, addr, MD2_CFG, 2, false)); 
}

void setup_accel_sensitivity(uint8_t sens)
{
        uint8_t CTRL1_XL[]={0x10,0xA0};
        switch(sens)
        {
          case 0://2g
          sensitivity = 1;
          CTRL1_XL[1] = 0xA0;
          break;
          case 1://4g
          sensitivity = 2;
          CTRL1_XL[1] = 0xA8;
          break;
          case 2://8g
          sensitivity = 4;
          CTRL1_XL[1] = 0xAC;
          break;
          case 3://16g
          sensitivity = 8;
          CTRL1_XL[1] = 0xA4;
          break;
        }
        (nrf_drv_twi_tx(&m_twi_g, addr_g, CTRL1_XL, 2, false));
}


void read_gyro_accel(const nrf_drv_twi_t* m_twi,uint8_t addr,uint8_t *dataLH)
{
        //delay(DEL);       
        (nrf_drv_twi_tx(m_twi, addr, &arr[0], 1, true)); // Send first register addr
        (nrf_drv_twi_rx(m_twi, addr, dataLH, 12)); // Receive data from the first register 
        //delay(DEL);   
}

void read_gyro(const nrf_drv_twi_t* m_twi,uint8_t addr,uint8_t *dataLH)
{
        delay(DEL);       
        (nrf_drv_twi_tx(m_twi, addr, &arrx[0], 1, true)); // Send first register addr
        (nrf_drv_twi_rx(m_twi, addr, dataLH, 12)); // Receive data from the first register 
        delay(DEL);   
}

void read_accel(const nrf_drv_twi_t* m_twi,uint8_t addr,uint8_t *dataLH)
{
        //delay(DEL);       
        (nrf_drv_twi_tx(m_twi, addr, arr, 1, true)); // Send first register addr
        (nrf_drv_twi_rx(m_twi, addr, dataLH, 6)); // Receive data from the first register 
        //delay(DEL);   
}

 void process_accel_gyro(short int *raw_data,float *proc_data)
 {
    for(int i = 0; i<3; i++)
    {
      proc_data[i] = ((((float)raw_data[i]/65535)*255)/accel_sens)*5;
    }
 } 

  void process_raw_accel(short int *raw_data,float *proc_data)
 {
    for(int i = 0; i<3; i++)
    {
      proc_data[i] = ((float)raw_data[i] * (0.061*sensitivity))/1000;
    }
 }

  void process_gyro(short int *raw_data,float *proc_data)
 {
    for(int i = 0; i<3; i++)
    {
      proc_data[i] = ((((float)raw_data[i]/65535)*255)/accel_sens)*5;
    }
 }

  void i2c_scanner(const nrf_drv_twi_t* m_twi,uint8_t addr)
  {
  //printf("sample data = %E\n\n\n....\n",Gyro_X);
    uint8_t data = 0xff;
    for (; addr <= TWI_ADDRESSES; addr++)
    {
        err_code = nrf_drv_twi_tx(m_twi,addr,&data,1,false);
        nrf_delay_ms(50);
        if (err_code == NRF_SUCCESS)
        {
            detected_device = true;
            #ifdef DEBUG
            printf("\nTWI device detected at addr 0x%x.", addr);
            printf("\nreceived data %c\n", data);
            #endif
            //break;
        }
    }

    if (!detected_device)
    {
        printf("No device was found.\n");
    }
  //delay(DEL+1000000);
  }

bool is_tap_detected(const nrf_drv_twi_t* m_twi)
{
        //printf("\nWaiting for tap\n");
        uint8_t tap_src_value;

        // Write the TAP_SRC register address
        uint8_t TAP_SRC = 0x1C;
        ret_code_t err_code = nrf_drv_twi_tx(m_twi, 0x6B, &TAP_SRC, 1, true);

        //if (err_code != NRF_SUCCESS) 
        //{
        //    printf("Error writing TAP_SRC address: %d\n", err_code);
        //    return 0;
        //}

        // Read the TAP_SRC register
        err_code = nrf_drv_twi_rx(m_twi, 0x6B, &tap_src_value, 1);

        if (err_code != NRF_SUCCESS)
        {
            printf("Error reading TAP_SRC: %d\n", err_code);
            return 0;
        }

        if (tap_src_value & (1 << SINGLE_TAP_BIT)) 
        {
            //printf("Single tap detected\n");
            return true;
        }
    
         return false;
}

void tap_enable(const nrf_drv_twi_t* m_twi)
{
        uint8_t TAP_SRC = 0x1C;
        ret_code_t err_code = nrf_drv_twi_tx(m_twi, 0x6B, &TAP_SRC, 1, true);
}
