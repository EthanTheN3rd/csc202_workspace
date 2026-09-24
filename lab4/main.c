//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 4
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <complex.h>
#include <stdbool.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include "LaunchPad.h"
#include "clock.h"
#include <ti/devices/msp/msp.h>

// #include "LaunchPad.c"
#include "ti/devices/msp/peripherals/m0p/hw_cpuss.h"
#include "uart.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

void run_lab4_part1(void);
void run_lab4_part2(void);
void run_lab4_part3(void);
void run_lab4_part4(void);
void run_lab4_part5(void);
void run_lab4_part6(void);

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

  // initialize leds
  leds_init();
  seg7_init();

  // enter your code here

  run_lab4_part1();
  msec_delay(1000);
  run_lab4_part2();
  msec_delay(500);
  run_lab4_part3();
  msec_delay(500);
  run_lab4_part4();
  msec_delay(500);
  run_lab4_part5();
  msec_delay(500);
  run_lab4_part6();

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void run_lab4_part1(void)
{
  // Make sure the 7 segment LEDs are off and enable the LED bar
  seg7_off();
  leds_enable();

  // flash the specified LEDs
  leds_on(0x3C);
  msec_delay(1000);
  leds_off();
}

void run_lab4_part2(void)
{
  seg7_off(); // turn off any 7 segment LEDs
  
  // main loop runs 5 times
  int i = 0;
  while (i < 5)
  {
    // loop thru LD0-LD7
    for (int k = 0; k < 8; k++)
    {
      uint8_t led_idx = 1U << k;
      leds_on(led_idx);
      msec_delay(200);
      leds_off();
    }
    // go back thru LD6-LD1
    for (int k = 0; k < 6; k++)
    {
      uint8_t led_idx = 1U << (6 - k);
      leds_on(led_idx);
      msec_delay(200);
      leds_off();
    }
    i++;
  }

  // after the last loop LD0 needs to be lit
  led_on(LED_BAR_LD0_IDX);
  msec_delay(200);
  led_off(LED_BAR_LD0_IDX);
}

void run_lab4_part3(void)
{
  seg7_off();
  uint8_t led_counter = 0x0; // Binary counter to be displayed be LEDs
  uint8_t loop_cntr   = 0x0; // keep track of how many times the loop runs

  while (loop_cntr < 2)
  {
    leds_on(led_counter);
    if (loop_cntr == 0)
    {
      msec_delay(100); // first run the delay is 0.1s
    }
    else
    {
      msec_delay(50); // second time delay is 0.05s
    }

    if (led_counter == 255) // increment the loop counter once led_counter made it thru all 256 values
    {
      loop_cntr++;
      msec_delay(500);
    }

    led_counter++;
  }
  leds_off();
}

void run_lab4_part4(void)
{
  leds_disable();
  uint8_t seg7_char_L = 0x38; // bitmask for a 7 segment letter L

  // Flash letter L for 1 second
  seg7_on(seg7_char_L, SEG7_DIG2_ENABLE_IDX);
  msec_delay(1000);
  seg7_off();
}

void run_lab4_part5(void)
{
  leds_disable();
  int i = 0;

  // Flash the number 4 on DIG2 4 times
  while (i < 4)
  {
    uint8_t seg7_num_4 = 0x66; // bitmask for 7 segment number 4
    seg7_on(seg7_num_4, SEG7_DIG0_ENABLE_IDX);
    msec_delay(3000);
    seg7_off();
    msec_delay(2000);
    i++;
  }
}

void run_lab4_part6(void)
{
  leds_disable();

  // bitmasks for the 7 segment letters
  uint8_t seg7_char_C = 0x39;
  uint8_t seg7_char_A = 0x77;
  uint8_t seg7_char_F = 0x71;
  uint8_t seg7_char_E = 0x79;

  // multiplex the word CAFE, looping 200 times
  int i = 0;
  while (i < 200)
  {
    seg7_on(seg7_char_C, SEG7_DIG0_ENABLE_IDX);
    msec_delay(1);
    seg7_on(seg7_char_A, SEG7_DIG1_ENABLE_IDX);
    msec_delay(1);
    seg7_on(seg7_char_F, SEG7_DIG2_ENABLE_IDX);
    msec_delay(1);
    seg7_on(seg7_char_E, SEG7_DIG3_ENABLE_IDX);
    msec_delay(1);
    i++;
  }
  seg7_off();
}