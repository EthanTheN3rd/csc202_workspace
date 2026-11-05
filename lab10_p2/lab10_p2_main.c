//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 10
//
//      FILE NAME:  lab10_p2_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code uses the UART serial interface to communicate with the
//    console, allowing commands typed in the console to be executed on
//    the launchpad.
//    A menu is displayed in the console with 4 options. The user can
//    type a number to select an option.
//    (1) Will display an incrementing number on seg7 DIG0
//    (2) Will read the temperature from ADC and display on LCD
//    (3) Will Flash the LED bar 3 times
//    (4) Will end the program
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
void run_lab10_part2(void);
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

  // run program
  run_lab10_part2();

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void run_lab10_part2(void)
{
  char choice = 0;

  bool done = false;
  update_seg7(false);

  while (!done)
  {
    display_menu();

    // Read character from UART and echo
    choice = UART_in_char();
    UART_out_char(choice);
    uart_write_string("\r\n");

    if (choice == '1')
    {
      uart_write_string("Incrementing 7-segment display count...\r\n");
      update_seg7(true);
    }
    else if (choice == '2')
    {
      uart_write_string("Updating temperature on LCD...\r\n");
      lcd_clear();
      display_temp();
    }
    else if (choice == '3')
    {
      uart_write_string("Flashing LEDs 3 times...\r\n");
      flash_leds();
    }
    else if (choice == '4')
    {
      // Exit the program

      uart_write_string("Program Exited\r\n");

      lcd_clear();
      lcd_set_ddram_addr(LCD_LINE1_ADDR);
      lcd_write_string("Program Stopped");

      leds_off();
      seg7_off();

      done = true;
    }
    else
    {
      // Handle invalid selection

      uart_write_string("Invalid selection! Please try again.\r\n");
    }
  }
}

//----------------------------------------------------------------------
// update_seg7(bool increment_count)
//
// This function increments a counter (0-9) and displays the value
// on DIG0 of the 7 seg display. A boolian (increment_count) is passed
// to choose whether the count should be increased.
//----------------------------------------------------------------------
void update_seg7(bool increment_count)
{
  static uint8_t count = 0;

  if (increment_count)
  {
    count++;
    if (count > 9)
    {
      count = 0;
    }
  }

  seg7_hex(count, SEG7_DIG0_ENABLE_IDX);
}


//----------------------------------------------------------------------
// display_temp(void)
//
// This function reads the temperature from ADC, and displays it on LCD
//----------------------------------------------------------------------
void display_temp(void)
{
  // calculate temp value
  uint32_t adc_val = ADC0_in(5);
  uint8_t  temp_c  = thermistor_calc_temperature(adc_val);
  uint8_t  temp_f  = (temp_c * 9 / 5) + 32;

  // Write formatted string to LCD Line 1
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Temp = ");
  lcd_write_byte(temp_f);
  lcd_write_string("\xDF"
                   "F");
}

//------------------------------------------------------------------
// flash_leds(void)
//
// This function flashes the led bar 3 times.
// First it turns off seg7 as to not accidentally flash it as well
// LEDs are enabled, and flashed 3 times.
// LEDs are disabled and seg7 is returned to it's previous state
//------------------------------------------------------------------
void flash_leds(void)
{
  leds_enable();
  seg7_off(); // disable seg7 before flashing LED bar

  for (uint8_t i = 0; i < 3; i++)
  {
    leds_on(0xFF); // turn on all 8 LEDs (0xFF)
    msec_delay(250);
    leds_off();
    msec_delay(250);
  }

  leds_disable();

  // Update seg7 with increment_count set to false, so the number
  // displays without increasing the count.
  update_seg7(false);
}

//----------------------------------------------------------------------
// uart_write_string(const char *str)
//
// This function accepts a string, and prints each character to UART out
//----------------------------------------------------------------------
void uart_write_string(const char *str)
{
  while (*str != '\0')
  {
    UART_out_char(*str++);
  }
}

//----------------------------------------------------------------------
// display_menu(void)
//
// This function displays the menu options for pt2 in the serial console
//----------------------------------------------------------------------
void display_menu(void)
{
  uart_write_string("\r\nMENU OPTIONS\r\n");
  uart_write_string(" 1. Increment count on 7-segment display\r\n");
  uart_write_string(" 2. Show current temperature\r\n");
  uart_write_string(" 3. Flash LEDs 3 times\r\n");
  uart_write_string(" 4. End Program\r\n");
  uart_write_string("Enter your selection: ");
}