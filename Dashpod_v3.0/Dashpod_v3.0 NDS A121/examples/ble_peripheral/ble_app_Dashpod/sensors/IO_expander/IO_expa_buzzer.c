#include "IO_expa_buzzer.h"
#include "twi_init.h"
static nrf_drv_twi_t m_twi_f; 
uint8_t data[2];
uint8_t pins_op;
uint8_t addr = 0x21;
void sound1(float j)
{
int i=100*j;
nrf_drv_twi_rx(&m_twi_f, addr, &pins_op, sizeof(pins_op));
    while(i--)
    {
      data[1] = pins_op | 0x08;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL+10);
      data[1] = pins_op | 0x00;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL);
    }
}

void sound2(int j)
{

nrf_drv_twi_rx(&m_twi_f, addr, &pins_op, sizeof(pins_op));
int i=100*j;
    while(i--)
    {
      data[1] = pins_op | 0x08;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL+50);
      data[1] = pins_op | 0x00;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL);
    }
}


void sound3(int j)
{

nrf_drv_twi_rx(&m_twi_f, addr, &pins_op, sizeof(pins_op));
int i=1+100*j;
    while(i--)
    {
      data[1] = pins_op | 0x08;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL+100);
      data[1] = pins_op | 0x00;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL);
    }
}

void sound4(int j)
{

nrf_drv_twi_rx(&m_twi_f, addr, &pins_op, sizeof(pins_op));
int i=1+j;
    while(i--)
    {
      data[1] = pins_op | 0x08;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL+100);
      data[1] = pins_op | 0x00;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL);
    }
}

void sound5(int j)
{

nrf_drv_twi_rx(&m_twi_f, addr, &pins_op, sizeof(pins_op));
int i=1+j;
    while(i--)
    {
      data[1] = pins_op | 0x00;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL+100);
      data[1] = pins_op | 0x08;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      delay(DEL);
    }
}


void stop()
{
      nrf_drv_twi_rx(&m_twi_f, addr, &pins_op, sizeof(pins_op));
      data[1] = pins_op | 0x00;
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      nrf_delay_ms(50);
}


void tone1()
{
    
        sound1(1);
        sound1(1);
        stop();
    
}


void tone2()
{
    
        sound2(1);
        sound2(1);
        stop();
    
}


void tone3()
{
    
        sound3(1);
        sound3(1);
        stop();
    
}
void tone4()
{
    
        sound4(10);
        sound4(10);
        stop();
    
}

void Beep()
{
        sound1(0.5);
        stop();
}

void IO_buzz_init(const nrf_drv_twi_t* m_twi_r,uint8_t address)
{
        m_twi_f = *m_twi_r;
        addr = address;
        data[0] = 0x01;
        A121_IO_PS_ON();
}

void buzzer_tone()
{
        //printf("In buzzer_tone function\n");
        tone2();
        stop();
        tone1();
        tone3();  
        tone3();
             
        tone2();     
        tone2();
        tone3();  
        tone3();
        stop();
        tone1();
        tone1(); 
    
        tone3();  
        tone3();       
        tone2();     
        tone2();
        tone1();
        tone1();
            
}

void buzzer_tone1()
{
        tone3();  
        tone3();       
        tone2();     
        tone2();
        tone1();
        tone1();
             
        tone2();     
        tone2();
        tone3();  
        tone3();
        stop();
        tone1();
        tone1();

        tone2();
        stop();
        tone1();
        tone3();  
        tone3();
            
}

void A121_IO_PS_ON()
{
    ret_code_t err_code;
    //uint8_t data;
    if(!is_twi_initialized()) 
    {
        twi_init();
        #ifdef DEBUG
        printf("TWI initialized\n");
        #endif
    }


    //i2c_scanner(&m_twi_f,0x19);
    //err_code = nrf_drv_twi_rx(&m_twi_f, addr, &data[0], sizeof(data));
    //nrf_delay_ms(100);

    //address = 0x21;
    /*
    sample_data[1] = 0x04;
    nrf_drv_twi_tx(&m_twi, address, sample_data, sizeof(sample_data),false);
    */
    //nrf_delay_ms(DEL);
    data[0] = 0x03;
    data[1] = 0x01;
    nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
    //Interrupt pin enabling
    data[0] = 0x45;
    data[1] = ~0x01;
    nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
    uint8_t dat;
    nrf_drv_twi_rx(&m_twi_f,addr,&dat,sizeof(dat));
    #ifdef DEBUG
    printf("dat = 0x%X\n", dat);
    #endif
    data[0] = 0x01;
}

void LED_LS(bool funtion)
{
      if(funtion)
      data[1] = IO_EX_EN | 0x10;
      else
      {
        led_ws2812b_reset();
        data[1] = IO_EX_EN | 0x00;
      }
      nrf_drv_twi_tx(&m_twi_f, addr, data, sizeof(data),false);
      nrf_delay_ms(20);
  
}

bool chrg_pin_read()
{
       bool val;
       uint8_t ipdata[3];
       ipdata[0] = 0x00;
       nrf_drv_twi_tx(&m_twi_f, addr, ipdata, 1,false);
       nrf_drv_twi_rx(&m_twi_f,addr,&ipdata[1],1);
       nrf_delay_ms(50);
       nrf_drv_twi_rx(&m_twi_f,addr,&ipdata[2],1);
       //printf("pin values = 0x%X\t0x%X\n",ipdata[1],ipdata[2]);
       val = !(0x01 & ipdata[1])||!(0x01 & ipdata[2]);
       return val;
}

void set_IO_pin(uint8_t pin)
{
      uint8_t set[]={0x01,0x00};
      uint8_t val;
      nrf_drv_twi_tx(&m_twi_f, addr, set, 1,false);
      nrf_drv_twi_rx(&m_twi_f, addr, &val,1);
      //printf("before set val = %x\n",val);
      set[1]=(val|(1<<pin));
      nrf_drv_twi_tx(&m_twi_f, addr, set, 2,false);
       nrf_drv_twi_rx(&m_twi_f, addr, &val,1);
      //printf("after set val = %x\n",val);
}

void clear_IO_pin(uint8_t pin)
{
      uint8_t clear[]={0x01,0x00};
      uint8_t val;
      nrf_drv_twi_tx(&m_twi_f, addr, clear, 1,false);
      nrf_drv_twi_rx(&m_twi_f, addr, &val,1);
      //printf("before clear val = %x\n",val);
      clear[1]=(val&(~(1<<pin)));
      nrf_drv_twi_tx(&m_twi_f, addr, clear, 2,false);
      nrf_drv_twi_rx(&m_twi_f, addr, &val,1);
      //printf("after clear val = %x\n",val);
}