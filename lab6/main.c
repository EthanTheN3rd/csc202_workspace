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

void wait_for_pb_pressed(uint8_t pb_idx);
void wait_for_pb_released(uint8_t pb_idx);

void run_lab6_part1(void);
void run_lab6_part2(void);
void run_lab6_part3(void);
void run_lab6_part4(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

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

  // Configure the LCD
  I2C_mstr_init();
  lcd1602_init();
  lcd_clear();

  // turn on buttons
  dipsw_init();

  run_lab6_part1();

  // pre-part2
  wait_for_pb_pressed(PB2_IDX);
  lcd_clear();

  wait_for_pb_released(PB2_IDX);
  lcd_write_string("Running Part 2");
  msec_delay(1000);

  run_lab6_part2();

  // instructions to continue
  msec_delay(1000);
  // lcd_clear();
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Press PB2");

  wait_for_pb_pressed(PB2_IDX);
  lcd_clear();

  wait_for_pb_released(PB2_IDX);
  lcd_write_string("Running Part 3");
  msec_delay(1000);

  run_lab6_part3();

  // instructions to continue
  msec_delay(1000);
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Press PB2");

  wait_for_pb_pressed(PB2_IDX);
  lcd_clear();
  // lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Running Part 4");
  msec_delay(1000);

  keypad_init();
  run_lab6_part4();

  // Finishing state
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");

  while (1)
    ;

} /* main */

void wait_for_pb_pressed(uint8_t pb_idx)
{
  while (is_pb_up(pb_idx))
    ;
  msec_delay(10);
}

void wait_for_pb_released(uint8_t pb_idx)
{
  while (is_pb_down(pb_idx))
    ;
  msec_delay(10);
}

void run_lab6_part1(void)
{
  uint8_t offset;

  lcd_clear();

  for (offset = 0; offset < 26; offset++)
  {
    lcd_write_char('A' + offset);
    if (offset == (CHARACTERS_PER_LCD_LINE - 1))
    {
      lcd_set_ddram_addr(LCD_LINE2_ADDR);
    }
  }
}

void run_lab6_part2(void)
{
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_4);
  lcd_write_quadbyte(1234567890);

  wait_for_pb_released(PB1_IDX);
  wait_for_pb_pressed(PB1_IDX);

  lcd_set_ddram_addr(LCD_LINE2_ADDR + LCD_CHAR_POSITION_6);
  lcd_write_doublebyte(1234);

  wait_for_pb_released(PB1_IDX);
  wait_for_pb_pressed(PB1_IDX);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);
  lcd_write_byte(123);

  wait_for_pb_released(PB1_IDX);

  wait_for_pb_pressed(PB1_IDX);
  lcd_clear();
  wait_for_pb_released(PB1_IDX);
  lcd_write_string("Part 2 Done");
}

void run_lab6_part3(void)
{
  int counter = 100;

  wait_for_pb_released(PB2_IDX);

  while (is_pb_up(PB2_IDX))
  {
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);
    lcd_write_byte(counter--);
    if (counter < 0 || is_pb_down(PB1_IDX))
    {
      counter = 100;
    }
    msec_delay(200);
  }
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Part 3 Done");
}

void run_lab6_part4(void)
{

  bool done        = false;
  int  char_pos = 0;

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  while (!done)
  {
    wait_no_key_pressed();
    msec_delay(5); // debounce

    uint8_t key = keypad_scan();

    // keep scanning the keycode until a key is pressed
    while (key == NO_KEY_PRESSED && !done)
    {
      key = keypad_scan();
      if (is_pb_down(PB1_IDX))
      {
        lcd_clear();
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        char_pos = 0;
      }
      if (is_pb_down(PB2_IDX))
      {
        done = true;
      }
    }

    // when char_pos hits 16, wrap to the second line
    // when it hits 32, wrap back to the start
    if (char_pos == CHARACTERS_PER_LCD_LINE)
    {
      lcd_set_ddram_addr(LCD_LINE2_ADDR);
    }
    else if (char_pos == TOTAL_CHARACTERS_PER_LCD)
    {
      lcd_clear();
      char_pos = 0;
    }

    hex_to_lcd(key);
    char_pos++;
  }
}