//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan
//
//       LAB NAME:  Callahan
//
//      FILE NAME:  lab10_p1_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code uses the UART serial interface to read user-typed text in
//    the console, and display it on the LCD when they hit ENTER.
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
#include "uart.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void uart_write_string(const char *str);
void display_menu(void);
void run_lab10_part1(void);
void update_seg7(bool increment_count);
void display_temp(void);
void flash_leds(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define BUFFER_SIZE 11

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold different data types

int main(void)
{
  // Configure the LaunchPad board
  clock_init_40mhz();
  launchpad_gpio_init();

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
  leds_init();
  seg7_init();

  UART_init(115200);

  I2C_mstr_init();
  lcd1602_init();
  lcd_clear();

  run_lab10_part1();

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void run_lab10_part1(void)
{
  char    buffer[BUFFER_SIZE];
  uint8_t buf_idx   = 0;
  char    character = 0;

  leds_off();
  seg7_off();

  bool done = false;

  while (!done)
  {
    // Read character from UART and echo
    character = UART_in_char();
    UART_out_char(character);

    // If the user hits ENTER, break the loop
    if (character == '\r')
    {
      buffer[buf_idx] = '\0';
      done            = true;
    }
    // If the user hits BACKSPACE, go back one space in the buffer
    else if (character == '\b')
    {
      if (buf_idx != 0)
      {
        buf_idx--;
      }
    }
    else
    {
      // Store character to buffer
      buffer[buf_idx] = character;

      if (buf_idx < BUFFER_SIZE)
        // Increment buf_idx
        buf_idx++;
    }
  }

  // display the output
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Name:");
  lcd_write_string(buffer);

  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Program Stopped");
}