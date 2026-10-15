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

void run_lab7_part1(void);
void run_lab7_part2(void);

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

// Defines the seven segment display for SOS
const uint8_t seg7_letter_code[] = {
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00, // s
    0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00, // O
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00  // s
};

// These timings for Morse code dots and dashes can vary slightly based on
// different implementations, but the 1:3 ratio for dots:dashes is a
// typical ratio for Morse code. The delay times in terms of number
// SysTick interrupts.

const uint8_t delay_count[] = {
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x18, // dots
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, // dash
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x30  // dots
};

int main(void)
{
  // Configure the launchpad boards
  clock_init_40mhz();
  launchpad_gpio_init();
  leds_init();
  leds_disable();
  seg7_init();
  sys_tick_init(SYST_TICK_PERIOD_COUNT);

  I2C_mstr_init();
  lcd1602_init();

  dipsw_init();

  // Run pt1
  run_lab7_part1();

  sys_tick_disable();
  seg7_off();


  // wait for user to press PB2
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Press PB2");

  while (is_pb_up(PB2_IDX));
  msec_delay(5);
  while (is_pb_down(PB2_IDX));
  msec_delay(5);
  
  // Run pt2
  run_lab7_part2();
  

  for (;;)
    ;
} /* main */

void run_lab7_part1(void) {
  char msg[] = "Microcontrollers are fun";  
  bool done = false;

  while (!done) {
    for (int i = 0; !done && i <= CHARACTERS_PER_LCD_LINE; i++) {
      lcd_clear();
      lcd_set_ddram_addr(LCD_LINE2_ADDR+(CHARACTERS_PER_LCD_LINE-i));
      lcd_write_string(msg);
      msec_delay(200);

      if (is_pb_down(PB1_IDX)) {
        done = true;
      }
    }

    int substring_idx = 1;
    while (!done && msg[substring_idx] != '\0') {
      char *substring = &msg[substring_idx];
      lcd_clear();
      lcd_set_ddram_addr(LCD_LINE2_ADDR);
      lcd_write_string(substring);
      msec_delay(200);
      substring_idx++;
      
      if (is_pb_down(PB1_IDX)) {
        done = true;
      }
    }
  }

  while (is_pb_down(PB1_IDX));
  msec_delay(5);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Part 1 Done");
}

void run_lab7_part2(void) {
  char msg[] = "Microcontrollers are fun. I love programming in MSPM0+ assembly code!!!";  
  bool done = false;

  while (!done) {
    for (int i = 0; !done && i <= CHARACTERS_PER_LCD_LINE; i++) {
      lcd_clear();
      lcd_write_string_window(msg, LCD_LINE2_ADDR+(CHARACTERS_PER_LCD_LINE-i), LCD_LINE2_ADDR+CHARACTERS_PER_LCD_LINE);
      msec_delay(200);

      if (is_pb_down(PB1_IDX)) {
        done = true;
      }
    }

    int substring_idx = 1;
    while (!done && msg[substring_idx] != '\0') {
      char *substring = &msg[substring_idx];
      lcd_clear();
      lcd_write_string_window(substring, LCD_LINE2_ADDR, LCD_LINE2_ADDR+CHARACTERS_PER_LCD_LINE);
      msec_delay(200);
      substring_idx++;
      
      if (is_pb_down(PB1_IDX)) {
        done = true;
      }
    }
  }

  while (is_pb_down(PB1_IDX));
  msec_delay(5);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Part 2 Done");
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
  static uint16_t delay_time = 1;
  static uint16_t code_index = 0;

  delay_time--;
  if (delay_time == 0)
  {
    // delay time has expires so now move on to next letter to display
    seg7_on(seg7_letter_code[code_index], SEG7_DIG0_ENABLE_IDX);

    // get next delay time
    delay_time = delay_count[code_index];
    code_index++;

    if (code_index == NUM_STATES)
    {
      // we went through SOS message so restart index
      code_index = 0;
    } /* if */
  }   /* if */
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