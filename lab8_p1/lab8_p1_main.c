//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 8 part 1
//
//      FILE NAME:  lab8_p1_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This program reads the value from the light sensor on the LP board.
//    It's then displayed on the LCD, and a flag indicates if it's light or dark.
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
void run_lab8_part1(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
const uint16_t LIGHT_TRESHHOLD = 0x00FF;

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool running = true;

// Define a structure to hold different data types

int main(void)
{
  // Configure the LaunchPad board
  clock_init_40mhz();
  launchpad_gpio_init();

  dipsw_init();
  config_pb1_interrupts();

  I2C_mstr_init();
  lcd1602_init();

  OPA0_init(7);
  OPA0_enable();
  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  run_lab8_part1();

  while (1)
    ;

} /* main */

void run_lab8_part1(void)
{
  while (running)
  {
    uint16_t adc_value = ADC0_in(7);
    lcd_set_ddram_addr(LCD_LINE1_ADDR);

    if (adc_value < LIGHT_TRESHHOLD)
    {
      lcd_write_string("Status: Dark ");
    }
    else
    {
      lcd_write_string("Status: Light");
    }

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("ADC = ");
    lcd_write_doublebyte(adc_value);
  }

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");
}

void config_pb1_interrupts(void)
{
  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;

  GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;

  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 1);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

void GROUP1_IRQHandler(void)
{
  if (running)
  {
    running = false;
  }
}