//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 8 part 3
//
//      FILE NAME:  lab8_p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code reads a value from the potentiometer, and displays it on both
//    the LCD and LED bar. Additionally it reads the tempurature in the room
//    using the thermister on the LP board, and converts the value to
//    farenheight to be displayed on the LCD.
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include "LaunchPad.h"
#include "adc.h"
#include "clock.h"
#include "lcd1602.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_adc12.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void update_temp(void);
void run_lab8_part2(void);


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool PROGRAM_RUNNING = true;
uint8_t ADC_TEMP = 0;

// Define a structure to hold different data types

int main(void)
{
  // Configure the LaunchPad board
  clock_init_40mhz();
  launchpad_gpio_init();

  dipsw_init();
  config_pb1_interrupts();
  config_pb2_interrupts();

  I2C_mstr_init();
  lcd1602_init();

  leds_init();
  leds_enable();

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  run_lab8_part2();

  while (1)
    ;

} /* main */

void run_lab8_part2(void)
{
  while (PROGRAM_RUNNING)
  {
    asm("CPSID i");
    uint16_t adc_value = ADC0_in(7);
    asm("CPSIE i"); 

    uint16_t led_value = adc_value/455;
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("ADC = ");
    lcd_write_doublebyte(adc_value);

    uint8_t celsius_temp = (uint8_t) thermistor_calc_temperature(ADC_TEMP);
    uint8_t farenheight_temp = (celsius_temp*1.8)+32;
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Temp = ");
    lcd_write_byte(farenheight_temp);
    lcd_write_char(0xDF);
    lcd_write_string("F");

    
    leds_off();
    for (int i = 0; i < led_value; i++) {      
      led_on(i);
    }
  }

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");
}

void update_temp(void) {
  
  
}

void config_pb1_interrupts(void)
{
  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;

  GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;

  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 1);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

void config_pb2_interrupts(void)
{
  GPIOA->POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;

  GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;

  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 0);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

void GROUP1_IRQHandler(void)
{
  uint32_t group_iidx_status;
  uint32_t gpio_mis = GPIOA->CPU_INT.MIS;

  do
  {
    group_iidx_status = CPUSS->INT_GROUP[1].IIDX;
    switch (group_iidx_status)
    {

      // GPIOA interrupt (PB2 Pressed)
      case (CPUSS_INT_GROUP_IIDX_STAT_INT0):
        gpio_mis = GPIOA->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) ==
            GPIO_CPU_INT_MIS_DIO15_SET)
        {
          // When PB2 pressed:
          ADC_TEMP = ADC0_in(5);

          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        }
        break;

      // GPIOB interrupt (PB1 pressed)
      case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
        gpio_mis = GPIOB->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) ==
            GPIO_CPU_INT_MIS_DIO18_SET)
        {
          PROGRAM_RUNNING     = false;
          GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
        }
        break;

      default:
        break;
    }
  } while (PROGRAM_RUNNING && group_iidx_status != 0);
}