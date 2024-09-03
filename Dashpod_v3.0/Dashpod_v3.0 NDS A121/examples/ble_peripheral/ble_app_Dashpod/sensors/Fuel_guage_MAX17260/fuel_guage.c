#include "fuel_guage.h"
#include "twi_init.h"


uint8_t fdata[3],per_g = 85;
static nrf_drv_twi_t m_twi_f; 
static uint8_t address_g;
uint16_t hib;
uint16_t cycles = 0, RCOMP = 0, TempCo = 0, FullCapNorm = 0;
float calibrate = 1.1236; 
volatile bool first = true;

#define MAX17260_SLAVE_ADDR				0x36

#define MAX17260_DATA_LENGTH				16
#define MAX17260_BATTERY_CAPACITY			1500/0.5  			/*(1500 mAh battery)*/
#define MAX17260_CURRENT_LIMIT_VALUE			0x0640                     
#define MAX17260_CMD_VEMPTY_VALUE			0xA561			
#define MAX17260_CMD_MODEL_CFG_VALUE			0x8000
	
#define MAX17260_CMD_POR_REG				0x00
#define MAX17260_CMD_HIB_CFG_REG			0xBA
#define MAX17260_CMD_SLEEP_REG				0x60
#define MAX17260_CMD_DESIGN_CAP_REG			0x18
#define MAX17260_CMD_ICHG_REG				0x1E
#define MAX17260_CMD_VEMPTY_REG				0x3A
#define MAX17260_CMD_MODEL_CFG_REG			0xDB
#define MAX17260_CMD_BATTERY_LEVEL_REG			0x06
#define MAX17260_CMD_BATTERY_LEVEL_DATA_REG             0x05
			
#define MAX17260_CMD_UNKNOWN_STATE			0x1
#define MAX17260_CMD_POR_STATE				0x2
#define MAX17260_CMD_HIB_CFG_STATE			0x3
#define MAX17260_CMD_DESIGN_CAP_STATE			0x4
#define MAX17260_CMD_ICHG_STATE				0x5
#define MAX17260_CMD_VEMPTY_STATE			0x6
#define MAX17260_CMD_MODEL_CFG_STATE			0x7
#define MAX17260_CMD_BATTERY_LEVEL_STATE		0x8

#define  MAX17260_CMD_CNFG1_REG                         0x1D
#define  MAX17260_CMD_CNFG2_REG                         0xBB
#define  MAX17260_CMD_SALRT_REG                         0x03

void setup_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr,uint16_t mAh)
{     
        if(!is_twi_initialized()) 
        {
            twi_init();
           #ifdef DEBUG
            printf("TWI initialized\n");
            #endif
        }  
        set_i2c_parameter(m_twi,addr);
        uint16_t por_state,data1;
        uint8_t data[2];
        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,true));
        (nrf_drv_twi_rx(m_twi,addr,data,2));
        //printf("POR stat1 = 0x%X %X\n",data[1],data[0]);
        //uint16_encode(0x0002, &fdata[1]);
        //nrf_drv_twi_tx(m_twi, addr,fdata, 3,true);
        //nrf_drv_twi_rx(m_twi,addr,data,2);
        //nrf_delay_ms(10);
        //printf("POR stat1 = 0x%X %X\n",data[1],data[0]);

        uint16_t status = data[0]|(data[1]<<8);
        //printf("POR stat2 = 0x%X %X\n",data[1],data[0]);
        //printf("PoR = 0x%X\n",status);
        status &= 0x0002;
        //printf("POR status = %X\n",status); 

        //if(status)//1
        {
            //printf("Fuel gauge intialization started\n");

            //10ms Wait Loop. Do not continue until FSTAT.DNR==0
            fdata[0] = 0x3D;
            uint16_t Fstat;            
            //printf("Waiting for Battery to connect\n");
            int l = 100,i = 0;
            do
            {
              (nrf_drv_twi_tx(m_twi, addr,fdata, 1,true));
              (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&Fstat,2));
              nrf_delay_ms(10);
              i++;
              l--;
            }while((Fstat & 1)||(l==0));
            //printf("l = %d\t", i);
            if(l == 0)
            {
              printf("Battery was not properly attached or not initialized properly\n");
            }
            else
            {
              #ifdef DEBUG
              printf("\n\nBattery Connected\n");
              #endif
            }

            //uint16_t hib;
            // store original hibCFG value
            fdata[0] = MAX17260_CMD_HIB_CFG_REG;
            (nrf_drv_twi_tx(m_twi,addr,fdata,1,false));
            (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&hib,2));

            //Exit Hibernate Mode step 1
            fdata[0] = 0x60;
            fdata[1] = 0x90;
            (nrf_drv_twi_tx(m_twi,addr,fdata,2,false));
            //nrf_delay_ms(10);
            //Exit Hibernate Mode step 2
            fdata[0] = MAX17260_CMD_HIB_CFG_REG;
            uint16_encode(0x0, &fdata[1]);
            (nrf_drv_twi_tx(m_twi,addr,fdata,3,false));
            //nrf_delay_ms(10);
            //Exit Hibernate Mode step 3
            fdata[0] = 0x60;
            uint16_encode(0x0, &fdata[1]);
            (nrf_drv_twi_tx(m_twi,addr,fdata,3,false));
            //nrf_delay_ms(10);
            
            //Write DesignCap register
            fdata[0] = MAX17260_CMD_DESIGN_CAP_REG;
            uint16_encode(MAX17260_BATTERY_CAPACITY, &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));
            //nrf_delay_ms(10);
            //printf("Capacity set 0x%X %X\n",fdata[2],fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
            (nrf_drv_twi_rx(m_twi,addr,&fdata[1],2));
            //printf("Capacity set 0x%X %X\n",fdata[2],fdata[1]);

            
            //Write IchargTerm register
            fdata[0] = MAX17260_CMD_ICHG_REG;
            uint16_encode(MAX17260_CURRENT_LIMIT_VALUE, &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));
            //nrf_delay_ms(10);
        
            //Write VEmpty register
            fdata[0] = MAX17260_CMD_VEMPTY_REG;
            uint16_encode(MAX17260_CMD_VEMPTY_VALUE, &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));
            //nrf_delay_ms(10);

            //Write ModelCFG register
            uint16_t Model;
            fdata[0] = MAX17260_CMD_MODEL_CFG_REG;
            (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
            (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&Model,2));
            //printf("initial Model cfg = 0x%X\n",Model);
            //printf("seting Model cfg = 0x%X\n",Model | MAX17260_CMD_MODEL_CFG_VALUE);
            uint16_encode((Model | MAX17260_CMD_MODEL_CFG_VALUE), &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));
            //nrf_delay_ms(10);

            //do not continue until ModelCFG.Refresh==0
            //printf("Waiting FG refresh\n");
            fdata[0] = MAX17260_CMD_MODEL_CFG_REG;
            if(l)
            {
              int i = 1;
              do
              { 
                (nrf_drv_twi_tx(m_twi, addr,fdata, 1,true));
                (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&Model,2));
                i++;
                //printf("\r%d",i*10);
                //printf("Model cfg = 0x%X\n",Model);
                nrf_delay_ms(10);
              }while(Model & 0x8000);
              #ifdef DEBUG
              printf("\nwaiting time = %d\n\n",i*10);
              #endif
            }
        
            //Restore original hibCFG value
            fdata[0] = MAX17260_CMD_HIB_CFG_REG;
            uint16_encode(hib, &fdata[1]);
            (nrf_drv_twi_tx(m_twi,addr,fdata,3,false));
            //nrf_delay_ms(10);

            //Read POR register status
            fdata[0] = MAX17260_CMD_POR_REG;
            (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
            (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
            uint16_encode(por_state & 0xFFFD, &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,true));
            //nrf_delay_ms(10);

            #ifdef DEBUG
            printf("Fuel gauge initalized\n\n");
            #endif
        }
}

uint8_t read_percentage_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr)
{   
        //printf("\nReading Battery percentage.....\n\n");    
        ret_code_t err_code;
        uint8_t val = 0, val1 = 0;
        //uint8_t batt_lvl = 0;
        uint16_t por_state,batt_lvl = 0;//change batt level to uint8_t
        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,true));
        (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
       // printf("POR stat = 0x%X\n",por_state);
        if((!(por_state & 0x0002)) )
        {
          val = MAX17260_CMD_BATTERY_LEVEL_REG; 
          (err_code = nrf_drv_twi_tx(m_twi, addr,&val, 1,true));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&batt_lvl,2));

          if (err_code == NRF_SUCCESS)
          {
              //printf("SOC register = 0x%X\t", batt_lvl);
              #ifdef DEBUG
              printf("Battery floating percentage = %f\n",(float)batt_lvl/256);
              #endif
              val = (batt_lvl/256)*calibrate;            
          }
          val1 = 0x0E; 
          (err_code = nrf_drv_twi_tx(m_twi, addr,&val1, 1,true));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&batt_lvl,1));

          if (err_code == NRF_SUCCESS)
          {
              val1 = (batt_lvl/256);            
          }

          if(val > 100)
          {
                val = 100;
          }


          return val;
        }
        else
        {
          printf("fuel gauge needs to be Re-Configured in SOC...\n");
          setup_fuel_guage(&m_twi_f,0x36,0000);
          return read_batt_per();
        }

}


float read_Full_capacity_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr)
{     
        //printf("\nReading Battery full capacity register....\n\n");  
        ret_code_t err_code;
        float val = 0;
        uint16_t por_state,fcap;

        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
        (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
        //printf("power reg = 0x%x\n",por_state);

        if(!(por_state & 0x0002))
        {
          fdata[0] = 0x10;
          (err_code = nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&fcap,2));
          //printf("FullCap = 0x%X\n",fcap);
          if (err_code == NRF_SUCCESS)
          {           
             val = (fcap*0.5);            
          }

          return (val);
        }
        else
        {
          printf("fuel gauge needs to be Re-Configured in FullCap...\n");
          setup_fuel_guage(&m_twi_f,0x36,0000);
          return read_batt_full_cap();
        }

}

float read_capacity_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr)
{       
        //printf("\nReading battery remainnig capacity...\n\n");
        ret_code_t err_code;
        float val = 0, fval = 0;
        uint16_t por_state,current,TTE,rcap,fcap,cyc;
        uint8_t voltage;
        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
        (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
        //printf("power reg = 0x%x\n",por_state);
        if(!(por_state & 0x0002))
        {
          fdata[0] = MAX17260_CMD_BATTERY_LEVEL_DATA_REG ; 
          (err_code = nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&rcap,2));
          //printf("RemCap = 0x%X\n", rcap);
          if (err_code == NRF_SUCCESS)
          {
             val = (rcap*0.5);            
          }
          fdata[0] = 0x09;
          (err_code = nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
          (err_code = nrf_drv_twi_rx(m_twi, addr,&voltage,1));
           
          fdata[0] = 0x0A;
          (err_code = nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(int8_t *)&current,1));
            
          fdata[0] = 0x17;
          (err_code = nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&cyc,2));


          return (val);
        }
        else
        {
          printf("fuel gauge needs to be Re-Configured in RemCap...\n");
          setup_fuel_guage(&m_twi_f,0x36,0000);
          return read_batt_cap();
        }

}

void save_learned_params()
{
        fdata[0] = 0x17;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&cycles,2));
        #ifdef DEBUG
        printf("Cycles = %d\n",cycles);
        #endif
        fdata[0] = 0x38;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&RCOMP,2));
        #ifdef DEBUG
        printf("RCOMP = %d\n",RCOMP);
        #endif
        fdata[0] = 0x39;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&TempCo,2));
        #ifdef DEBUG
        printf("TempCo = %d\n",TempCo);
        #endif
        fdata[0] = 0x23;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&FullCapNorm,2));
        #ifdef DEBUG
        printf("FullCapNorm = %d %.2f\n",FullCapNorm,FullCapNorm*0.5);
        #endif
}


void upload_learned_params()
{
        fdata[0] = 0x17;
        uint16_encode(cycles,&fdata[1]);
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 3,false));
        fdata[0] = 0x38;
        uint16_encode(RCOMP, &fdata[1]);
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 3,false));
        fdata[0] = 0x39;
        uint16_encode(TempCo, &fdata[1]);
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 3,false));
        fdata[0] = 0x23;
        uint16_encode(FullCapNorm, &fdata[1]);
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 3,false));
}

float read_TTE()
{
        fdata[0] = 0x11;
        uint16_t TTE;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&TTE,2));
        #ifdef DEBUG
        printf("TTE = 0x%X %.2f\n",TTE, TTE*5.625);
        #endif
        return TTE*5.625;
}

float read_TTF()
{
        fdata[0] = 0x11;
        uint16_t TTF;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&TTF,2));
        #ifdef DEBUG
        printf("TTF = 0x%X %.2f\n",TTF, TTF*5.625);
        #endif
        return TTF*5.625;
}

float read_voltage()
{
        fdata[0] = 0x09;
        uint16_t voltage;
        float volt;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&voltage,2));
        //printf("Voltage register = 0x%X\t",voltage);
        volt = voltage*0.078125 ;
        #ifdef DEBUG
        printf("volt = %.2fV\n", volt/1000);
        #endif
        return volt/1000;
}

float read_current()
{
        fdata[0] = 0x0A;
        uint16_t current;
        float curr;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&current,2));
        //printf("Current register = ~0x%X\t",current = ~current);
        curr = (((~current)-1)*0.015625);
        #ifdef DEBUG
        printf("Current = %.2fmA\n", curr);
        #endif
        return curr;
}

uint8_t read_MaxMinVolt(float MaxMinVolt[2])
{
        fdata[0] = 0x1B;
        uint8_t MaxMin[2]= {0,0};
        //float MaxMinVolt[2];
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,MaxMin,2));
        //printf("MaxMinvolt reg = 0x%X%X\t\t",MaxMin[1],MaxMin[0]);
        MaxMinVolt[1] = (float)(MaxMin[1] * 20)/1000;
        MaxMinVolt[0] = (float)(MaxMin[0] * 20)/1000;
        #ifdef DEBUG
        printf("MaxVolt = %.2fV\t",MaxMinVolt[1]);
        printf("MinVolt = %.2fV\n",MaxMinVolt[0]);
        #endif
        return 0;
}

void set_i2c_parameter(const nrf_drv_twi_t* m_twi,uint8_t addr)
{
  m_twi_f = *m_twi;
  address_g = addr;
}

uint8_t read_batt_per()
{
  setup_fuel_guage(&m_twi_f,address_g,0);

  float MaxMin_volt[2], voltage, diff;
  //save_learned_params();
  //read_TTE();
  //read_TTF();
  //read_current();
  voltage = read_voltage();
  read_MaxMinVolt(MaxMin_volt);

  uint8_t per = read_percentage_fuel_guage(&m_twi_f,address_g);

  printf("\nRemaining charge = %d%%\n\n", per);


  //upload_learned_params();
  return per;
}

float read_batt_cap()
{
  return read_capacity_fuel_guage(&m_twi_f,address_g);
}

float read_batt_full_cap()
{
  return read_Full_capacity_fuel_guage(&m_twi_f,address_g);
}