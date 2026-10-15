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
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

void lcd_write_string_window(const char *string, uint8_t start_lcd_addr, uint8_t max_lcd_addr);
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

void run_lab7_part3(void) {
  int cntr = 0;
  bool done = false;
  while (!done) {
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR+LCD_CHAR_POSITION_7);
    lcd_write_byte(cntr);
    if (cntr == 99) {
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
// configured SysTick period. This ISR is responsible for managing the
// timing and display of the Morse code for "SOS" on the seven-segment
// display. It controls the blink timing for the letters, with quick
// blinks for "S" (dot-dot-dot) and slower blinks for "O" (dash-dash-dash).
// The sequence of letters is repeated indefinitely.
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
  for (int i = 0; i < 4; i++) {
    uint8_t sw_state = dipsw_read();
    if (sw_state & 1U << i) {
      num_switches_on++;
    }
  }

  seg7_hex(num_switches_on, SEG7_DIG0_ENABLE_IDX);

} /* SysTick_Handler */

void lcd_write_string_window(const char *string, uint8_t start_lcd_addr, uint8_t max_lcd_addr)
{
    bool done = false;
    lcd_set_ddram_addr(start_lcd_addr);
    for (int i = 0; !done && i < (max_lcd_addr-start_lcd_addr); i++) {
      if (string[i+1] == '\0') {
        done = true;
      }
      lcd_write_char(string[i]);
    }

} /* lcd_write_string */