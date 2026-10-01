//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 5
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

void msp_printf(char *buffer, unsigned int value);

void run_lab5_part1(void);
void run_lab5_part2(void);
void run_lab5_part3(void);
void run_lab5_part4(void);

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
  UART_init(115200);

  dipsw_init();
  lpsw_init();
  seg7_init();

  run_lab5_part1();
  msec_delay(500);
  run_lab5_part2();
  msec_delay(500);

  seg7_deinit();
  keypad_init();
  leds_init();

  run_lab5_part3();
  run_lab5_part4();

} /* main */

void run_lab5_part1(void)
{
  bool done        = false;
  bool disp_on     = false;
  int  press_count = 0;
  while (!done)
  {

    if (is_pb_down(PB1_IDX))
    {
      press_count++;
      if (disp_on)
      {
        seg7_off();
        disp_on = false;
      }
      else
      {
        seg7_hex(0x3, SEG7_DIG0_ENABLE_IDX);
        disp_on = true;
      }
      msec_delay(5);
      while (is_pb_down(PB1_IDX))
        ;
      msec_delay(5);
    }

    if (press_count == 6)
    {
      done = true;
      msp_printf("\n* P1 DONE * ", 0);
    }
  }
}

void run_lab5_part2(void)
{
  typedef enum
  {
    GET_LOW,
    GET_HIGH,
    DISPLAY
  } state_t;

  state_t current_state = GET_LOW;

  uint8_t low_nibble;
  uint8_t high_nibble;
  int     cycle_count = 0;

  while (cycle_count < 4)
  {
    switch (current_state)
    {
      case GET_LOW:
        low_nibble = dipsw_read();

        if (is_lpsw_down(LP_SW2_IDX))
        {
          current_state = GET_HIGH;
          // debounce
          msec_delay(5);
          while (is_lpsw_down(LP_SW2_IDX))
            ;
          msec_delay(5);
        }
        break;

      case GET_HIGH:
        high_nibble = dipsw_read();

        if (is_lpsw_down(LP_SW2_IDX))
        {
          cycle_count++;
          current_state = DISPLAY;

          // debounce
          msec_delay(5);
          while (is_lpsw_down(LP_SW2_IDX))
            ;
          msec_delay(5);
        }
        break;

      case DISPLAY:
        uint8_t seg7_data = (high_nibble << 4) | (low_nibble & 0x0F);

        // The value will be displayed on either DIG0 or DIG2
        // depending on if PB1 is pressed.
        if (is_pb_down(PB1_IDX))
        {
          seg7_on(seg7_data, SEG7_DIG2_ENABLE_IDX);

          // debounce
          msec_delay(5);
          while (is_pb_down(PB1_IDX))
            ;
          msec_delay(5);
        }
        else
        {
          seg7_on(seg7_data, SEG7_DIG0_ENABLE_IDX);
        }

        if (is_lpsw_down(LP_SW2_IDX))
        {
          current_state = GET_LOW;
          seg7_off();

          // debounce
          msec_delay(5);
          while (is_lpsw_down(LP_SW2_IDX))
            ;
          msec_delay(5);
        }
        break;

      default:
        current_state = GET_LOW;
        break;
    } /* switch */
  }
}

void run_lab5_part3(void)
{
  int cycle_count = 0;
  while (cycle_count < 8)
  {
    leds_enable();
    leds_off();

    uint8_t key_val = getkey_pressed();
    leds_on(key_val);
    msec_delay(5);
    wait_no_key_pressed();
    msec_delay(5);

    cycle_count++;
  }
}

void run_lab5_part4(void)
{
  leds_off();
  int     cycle_count = 0;
  uint8_t key_press   = keypad_scan();

  // keep scanning until we see a key pressed
  while (key_press == NO_KEY_PRESSED)
  {
    key_press = keypad_scan();
  }

  for (int i = 0; i < key_press; i++)
  {
    leds_on(0xFF);
    msec_delay(500);
    leds_off();
    msec_delay(500);
  }
}

void msp_printf(char *buffer, unsigned int value)
{
  unsigned int i   = 0;
  unsigned int len = 0;
  char         string[80];

  len = sprintf(string, buffer, value);

  // Walk through array to send each character to serial port
  for (i = 0; i < len; i++)
  {
    UART_out_char(string[i]);
  }
} /* msp printf */