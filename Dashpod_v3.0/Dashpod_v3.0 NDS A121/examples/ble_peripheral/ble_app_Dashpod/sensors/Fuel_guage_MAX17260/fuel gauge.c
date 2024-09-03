#include "fuel_gauge.h"

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

uint8_t fdata[3],per_g = 85;
static nrf_drv_twi_t m_twi_f; 
static uint8_t address_g;
uint16_t hib;
uint16_t cycles = 0, RCOMP = 0, TempCo = 0, FullCapNorm = 0;
float calibrate = 1.118; 
volatile bool first = true;



void setup_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr,uint16_t mAh)
{
        set_i2c_parameter(m_twi,addr);
        uint8_t data[2];

        fdata[0] = MAX17260_CMD_POR_REG;
        nrf_drv_twi_tx(m_twi, addr,fdata, 1,true);
        nrf_drv_twi_rx(m_twi,addr,data,2);

        uint16_t status = data[0]|(data[1]<<8);
        
        status &= 0x0002;

        //if(status)
        {
                fdata[0] = 0x3D;
                uint16_t Fstat;

                int l = 100,i = 0;
                do
                {
                  (nrf_drv_twi_tx(m_twi, addr,fdata, 1,true));
                  (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&Fstat,2));
                  nrf_delay_ms(10);
                  i++;
                  l--;
                }while((Fstat & 1)||(l==0));
            
                if(l == 0)
                {
                      printf("Battery was not properly attached or not initialized properly\n");
                }
                else
                      printf("\n\nBattery Connected\n");

                hib_store(m_twi,addr);
                hib_exit(m_twi,addr);
                ez_config(m_twi,addr,l);
                hib_restore(m_twi,addr);
                clear_por(m_twi,addr);
        }
        printf("Fuel gauge initalized\n\n");
}

void set_i2c_parameter(const nrf_drv_twi_t* m_twi,uint8_t addr)
{
  m_twi_f = *m_twi;
  address_g = addr;
}

void hib_store(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
          // store original hibCFG value
          fdata[0] = MAX17260_CMD_HIB_CFG_REG;
          (nrf_drv_twi_tx(m_twi,addr,fdata,1,false));
          (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&hib,2));
}

void hib_exit(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
           //Exit Hibernate Mode step 1
            fdata[0] = 0x60;
            fdata[1] = 0x90;
            (nrf_drv_twi_tx(m_twi,addr,fdata,2,false));
            
            //Exit Hibernate Mode step 2
            fdata[0] = MAX17260_CMD_HIB_CFG_REG;
            uint16_encode(0x0, &fdata[1]);
            (nrf_drv_twi_tx(m_twi,addr,fdata,3,false));
            
            //Exit Hibernate Mode step 3
            fdata[0] = 0x60;
            uint16_encode(0x0, &fdata[1]);
            (nrf_drv_twi_tx(m_twi,addr,fdata,3,false));
}

void ez_config(const nrf_drv_twi_t *m_twi, uint8_t addr,int l)
{
            //Write DesignCap register
            fdata[0] = MAX17260_CMD_DESIGN_CAP_REG;
            uint16_encode(MAX17260_BATTERY_CAPACITY, &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));
            
            //Write IchargTerm register
            fdata[0] = MAX17260_CMD_ICHG_REG;
            uint16_encode(MAX17260_CURRENT_LIMIT_VALUE, &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));
        
            //Write VEmpty register
            fdata[0] = MAX17260_CMD_VEMPTY_REG;
            uint16_encode(MAX17260_CMD_VEMPTY_VALUE, &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));

            //Write ModelCFG register
            uint16_t Model;
            fdata[0] = MAX17260_CMD_MODEL_CFG_REG;
            (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
            (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&Model,2));
            uint16_encode((Model | MAX17260_CMD_MODEL_CFG_VALUE), &fdata[1]);
            (nrf_drv_twi_tx(m_twi, addr,fdata, 3,false));

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
                nrf_delay_ms(10);
              }while(Model & 0x8000);
              printf("\nwaiting time = %d\n\n",i*10);
            }
}

void hib_restore(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
            //Restore original hibCFG value
            fdata[0] = MAX17260_CMD_HIB_CFG_REG;
            uint16_encode(hib, &fdata[1]);
            (nrf_drv_twi_tx(m_twi,addr,fdata,3,false));
}

void clear_por(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
        uint16_t por_state;
        //Read POR register status
        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
        (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
        uint16_encode(por_state & 0xFFFD, &fdata[1]);
        (nrf_drv_twi_tx(m_twi, addr,fdata, 3,true));
}

uint8_t read_percentage_fuel_guage(const nrf_drv_twi_t* m_twi,uint8_t addr)
{   
        //printf("\nReading Battery percentage.....\n\n");    
        ret_code_t err_code;
        uint8_t val = 0, val1 = 0;
       
        uint16_t por_state,batt_lvl = 0;//change batt level to uint8_t
        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,true));
        (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
       
        if((!(por_state & 0x0002)) )
        {
          val = MAX17260_CMD_BATTERY_LEVEL_REG; 
          (err_code = nrf_drv_twi_tx(m_twi, addr,&val, 1,true));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&batt_lvl,2));

          if (err_code == NRF_SUCCESS)
          {
              
              printf("Battery floating percentage = %f\n",(float)batt_lvl/256);
              val = (batt_lvl/256)*calibrate;            
          }
          val1 = 0x0E; 
          (err_code = nrf_drv_twi_tx(m_twi, addr,&val1, 1,true));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&batt_lvl,1));

          if (err_code == NRF_SUCCESS)
          {
              val1 = (batt_lvl/256);            
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
        
        ret_code_t err_code;
        float val = 0;
        uint16_t por_state,fcap;

        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
        (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
        

        if(!(por_state & 0x0002))
        {
          fdata[0] = 0x10;
          (err_code = nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&fcap,2));
          
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
        
        ret_code_t err_code;
        float val = 0, fval = 0;
        uint16_t por_state,current,TTE,rcap,fcap,cyc;
        uint8_t voltage;
        fdata[0] = MAX17260_CMD_POR_REG;
        (nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
        (nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&por_state,2));
        
        if(!(por_state & 0x0002))
        {
          fdata[0] = MAX17260_CMD_BATTERY_LEVEL_DATA_REG ; 
          (err_code = nrf_drv_twi_tx(m_twi, addr,fdata, 1,false));
          (err_code = nrf_drv_twi_rx(m_twi, addr,(uint8_t *)&rcap,2));
          
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
          return read_batt_full_cap();
        }

}

void save_learned_params()
{
        fdata[0] = 0x17;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&cycles,2));
        printf("Cycles = %d\n",cycles);
        fdata[0] = 0x38;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&RCOMP,2));
        printf("RCOMP = %d\n",RCOMP);
        fdata[0] = 0x39;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&TempCo,2));
        printf("TempCo = %d\n",TempCo);
        fdata[0] = 0x23;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&FullCapNorm,2));
        printf("FullCapNorm = %d %.2f\n",FullCapNorm,FullCapNorm*0.5);
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
        printf("TTE = 0x%X %.2f\n",TTE, TTE*5.625);
        return TTE*5.625;
}

float read_TTF()
{
        fdata[0] = 0x11;
        uint16_t TTF;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&TTF,2));
        printf("TTF = 0x%X %.2f\n",TTF, TTF*5.625);
        return TTF*5.625;
}

float read_voltage()
{
        fdata[0] = 0x09;
        uint16_t voltage;
        float volt;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&voltage,2));
        
        volt = voltage*0.078125 ;
        printf("volt = %.2fV\n", volt/1000);
        return volt/1000;
}

float read_current()
{
        fdata[0] = 0x0A;
        uint16_t current;
        float curr;
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,(uint8_t *)&current,2));
        
        curr = (((~current)-1)*0.015625);
        printf("Current = %.2fmA\n", curr);
        return curr;
}

uint8_t read_MaxMinVolt(float MaxMinVolt[2])
{
        fdata[0] = 0x1B;
        uint8_t MaxMin[2]= {0,0};
        
        (nrf_drv_twi_tx(&m_twi_f, address_g,fdata, 1,false));
        (nrf_drv_twi_rx(&m_twi_f,address_g,MaxMin,2));
        
        MaxMinVolt[1] = (float)(MaxMin[1] * 20)/1000;
        MaxMinVolt[0] = (float)(MaxMin[0] * 20)/1000;
        printf("MaxVolt = %.2fV\t",MaxMinVolt[1]);
        printf("MinVolt = %.2fV\n",MaxMinVolt[0]);
        return 0;
}

uint8_t read_batt_per()
{
        setup_fuel_guage(&m_twi_f,address_g,0);
        //hib_exit(&m_twi_f, address_g);
        float MaxMin_volt[2], voltage, diff;

        voltage = read_voltage();
        read_MaxMinVolt(MaxMin_volt);

        uint8_t per = read_percentage_fuel_guage(&m_twi_f,address_g);

        //hib_restore(&m_twi_f,address_g);

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

void interrupt_set(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
            uint16_t data;
            // Configure SAlert register for 10% battery interrupt
            fdata[0] = MAX17260_CMD_SALRT_REG;
            uint16_encode(0xFF00, &fdata[1]);  // Set alert threshold to 10% SoC
            nrf_drv_twi_tx(m_twi, addr, fdata, 3, false);
            nrf_drv_twi_tx(m_twi,addr,fdata,1,false);
            nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&data,2);
            printf("data = 0x%x\n",data);
            nrf_delay_ms(100);

            // Configure CNFG1 register for interrupt setup
            fdata[0] = MAX17260_CMD_CNFG1_REG;
            uint16_encode(0x2214, &fdata[1]);  // Configure CNFG register (check Aen position)
            nrf_drv_twi_tx(m_twi, addr, fdata, 3, false);
            nrf_drv_twi_tx(m_twi,addr,fdata,1,false);
            nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&data,2);
            printf("data = 0x%x\n",data);
            nrf_delay_ms(100);
  

            // Configure CNFG2 register for interrupt setup
            fdata[0] = MAX17260_CMD_CNFG2_REG;
            uint16_encode(0x3658, &fdata[1]);  // Configure CNFG register (check Aen position)
            nrf_drv_twi_tx(m_twi, addr, fdata, 3, false);
            nrf_drv_twi_tx(m_twi,addr,fdata,1,false);
            nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&data,2);
            printf("data = 0x%x\n",data);
            nrf_delay_ms(100);

}

void clear_alert(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
    uint16_t status;

    // Read the status register
    fdata[0] = MAX17260_CMD_POR_REG;
    nrf_drv_twi_tx(m_twi, addr, fdata, 1, false);
    nrf_drv_twi_rx(m_twi, addr, (uint8_t *)&status, 2);
    printf("Status register = 0x%x\n", status);

    // Clear the corresponding alert flag in the status register
    if (status & 0x0400) 
    {  // Assuming the alert flag for SOC is in bit 9 (example)
        fdata[0] = MAX17260_CMD_POR_REG;
        status &= ~0x0400;  // Clear the alert flag
        uint16_encode(status, &fdata[1]);
        nrf_drv_twi_tx(m_twi, addr, fdata, 3, false);
        printf("Hello world\n");
        nrf_delay_ms(100);

        // Configure SAlert register for 10% battery interrupt
        fdata[0] = MAX17260_CMD_SALRT_REG;
        uint16_encode(0xFF00, &fdata[1]);  // Set alert threshold to 10% SoC
        nrf_drv_twi_tx(m_twi, addr, fdata, 3, false);
    }
}

void check_alert(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
    uint16_t status;

    // Read the status register
    fdata[0] = MAX17260_CMD_POR_REG;
    nrf_drv_twi_tx(m_twi, addr, fdata, 1, false);
    nrf_drv_twi_rx(m_twi, addr, (uint8_t *)&status, 2);
    printf("Status register = 0x%x\n", status);
}

/*void interrupt_clear(const nrf_drv_twi_t *m_twi, uint8_t addr)
{
            uint16_t data;

            // Configure SAlert register for 10% battery interrupt
            fdata[0] = MAX17260_CMD_SALRT_REG;
            uint16_encode(0xFF00, &fdata[1]);  // Set alert threshold to 10% SoC
            nrf_drv_twi_tx(m_twi, addr, fdata, 3, false);
            nrf_drv_twi_tx(m_twi,addr,fdata,1,false);
            nrf_drv_twi_rx(m_twi,addr,(uint8_t *)&data,2);
            printf("data = 0x%x\n",data);
            nrf_delay_ms(100);
}