//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 7
//
//      FILE NAME:  lab7_p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This program uses the SysTick interrupt to poll the dip switches, count
//    how many are on, and display that on the 7 segment display.
//    Simultaneously the main function runs a counter that displays on the LCD
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
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

void lcd_write_string_window(const char *string, uint8_t start_lcd_addr,
                             uint8_t max_lcd_addr);
void run_lab7_part3(void);

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

int main(void)
{
  // Configure the launchpad boards
  clock_init_40mhz();
  launchpad_gpio_init();
  seg7_init();
  sys_tick_init(SYST_TICK_PERIOD_COUNT);

  I2C_mstr_init();
  lcd1602_init();

  dipsw_init();

  run_lab7_part3();

  for (;;)
    ;
} /* main */

void run_lab7_part3(void)
{
  int  cntr = 0;
  bool done = false;

  while (!done)
  {
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);
    lcd_write_byte(cntr);
    
    if (cntr == 99)
    {
      cntr = -1;
    }

    cntr++;
    msec_delay(200);
  }
}

//----------------------------------------------------------------------------
// DESCRIPTION:
//
// This function represents the ISR (Interrupt Service Routine) for
// the SysTick timer. It is called at regular intervals based on the
// configured SysTick period. This ISR polls the dip switches, counts how many
// are active, and displays the number on the 7 segment display.
//
// INPUT PARAMETERS:
//
// none
//
// OUTPUT PARAMETERS:
//
// none
//
// RETURN:
//
// none
//----------------------------------------------------------------------------

void SysTick_Handler(void)
{
  uint8_t num_switches_on = 0;

  // loop through all 4 switches and count how many are on
  for (int i = 0; i < 4; i++)
  {
    uint8_t sw_state = dipsw_read();
    if (sw_state & 1U << i)
    {
      num_switches_on++;
    }
  }

  // display the number of active switches
  seg7_hex(num_switches_on, SEG7_DIG0_ENABLE_IDX);

} /* SysTick_Handler */