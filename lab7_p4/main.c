//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
//
//       LAB NAME:  TBD
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code ... *** COMPLETE THIS BASED ON LAB REQUIREMENTS ***
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
#include "clock.h"
#include "lcd1602.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_gpio.h"
#include "ti/devices/msp/peripherals/m0p/hw_cpuss.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

void run_lab7_part4(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);

//----------------------------------------------------------------------------
// Define symbolic constants used by the program
//----------------------------------------------------------------------------
#define MSPMO_CLOCK_FREQUENCY (40E6)
#define SYST_TICK_PERIOD (10.25E-3)
#define SYST_TICK_PERIOD_COUNT (SYST_TICK_PERIOD * MSPMO_CLOCK_FREQUENCY)

#define NUM_STATES (18)

//----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//----------------------------------------------------------------------------
bool PB2_PRESSED = true;
bool PROGRAM_RUNNING = true;

int main(void)
{
  // Configure the launchpad boards
  clock_init_40mhz();
  launchpad_gpio_init();

  // Configure interupts
  asm("CPSIE I");
  config_pb1_interrupts();
  config_pb2_interrupts();
  dipsw_init();

  // Enable LEDs and LCD
  I2C_mstr_init();
  lcd1602_init();
  leds_init();
  leds_enable();

  run_lab7_part4();

  // When program exited
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");
  leds_off();
  NVIC_DisableIRQ(GPIOA_INT_IRQn);

  for (;;)
    ;
} /* main */

void run_lab7_part4(void) {
  int  cntr = 0;
  while (PROGRAM_RUNNING)
  {
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);
    lcd_write_byte(cntr);
    leds_on(cntr);
    if (PB2_PRESSED)
    {
      lcd_set_ddram_addr(LCD_LINE2_ADDR + LCD_CHAR_POSITION_4);
      lcd_write_string("PB2_PRESSED");
    }
    if (cntr == 99)
    {
      cntr = -1;
    }
    cntr++;
    msec_delay(200);
  }

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

  do {
    group_iidx_status = CPUSS->INT_GROUP[1].IIDX;
    switch (group_iidx_status) {

      // GPIOA interrupt
      case (CPUSS_INT_GROUP_IIDX_STAT_INT0):
        gpio_mis = GPIOA->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) == GPIO_CPU_INT_MIS_DIO15_SET)
        {
          if (PB2_PRESSED)
          {
            PB2_PRESSED = false;
          }
          else
          {
            PB2_PRESSED = true;
          }

          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        }
        break;

      // GPIOB interrupt
      case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
        gpio_mis = GPIOB->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) == GPIO_CPU_INT_MIS_DIO18_SET)
        {
          PROGRAM_RUNNING = false;
          GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
        }
        break;

      default:
        break;
    }
  } while (PROGRAM_RUNNING && group_iidx_status != 0);

}