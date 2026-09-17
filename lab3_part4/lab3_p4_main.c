//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 3, part 4
//
//      FILE NAME:  lab3_p4_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program runs on the Texas Instruments MSPM0 LaunchPad 
//    (p/n LP-MSPM0G3507) and also serves as hands-on exercise for practicing
//    bitwise operations in C within an embedded systems context using the 
//    MSPM0 (ARM Cortex-M0+).

//    Students will manipulate a simulated 32-bit hardware register and learn
//    how to safely modify individual bits without affecting others using
//    proper read-modify-write techniques.
//
//    Key concepts reinforced in this lab include:
//      - Bit masking using symbolic constants
//      - Setting, clearing, and testing individual bits
//      - Read-modify-write operations for register control
//
//    Students are required to:
//      - Define symbolic constants for bit masks
//      - Implement the following functions:
//          * set_bit()
//          * clear_bit()
//          * check_bit()
//      - Complete a series of problems that apply these operations
//
//    Each problem requires modifying the register using a single line of code
//    (read-modify-write). After each operation, the updated register value or
//    result should be displayed using the msp_printf() function via UART.
//
//    The program outputs the results of each operation, allowing students to
//    observe how bitwise manipulation affects the register value.
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"
#include "uart.h"


//-----------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------
void msp_printf(char* buffer, unsigned int value);
uint32_t set_bit(uint32_t reg_value, uint32_t bit_mask);
uint32_t clear_bit(uint32_t reg_value, uint32_t bit_mask);
bool check_bit(uint32_t reg_value, uint32_t bit_mask);

//-----------------------------------------------------------------------------
// Define symbolic constants used by program
//-----------------------------------------------------------------------------
#define BAUD_RATE                                                       (115200)

// Symbolic constants to the bit fields with the register
#define PIE_BIT_MASK                                                    (0x0001)
#define EME_BIT_MASK                                                    (0x0002)
#define RD_BIT_MASK                                                     (0x0004)
#define CRS_BIT_MASK                                                    (0x0070)
#define A3A2A1A0_BIT_MASK                                               (0xF000)
#define A2_BIT_MASK                                                     (0x4000)
#define PRS_BIT_MASK                                                    (0x0E00)
#define MD_BIT_MASK                                                     (0x0008)
#define MODE_BIT_MASK                                                   (0x0180)
#define MODE_LSB_BIT_MASK                                               (0x0080)

//-----------------------------------------------------------------------------
// Define global variable and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

int main(void)
{
  // Create a pointer to memory location of the register
  volatile uint32_t* test_reg32 = (uint32_t *)(&SysTick->LOAD);

  // create local variable to hold register value
  uint32_t reg_value;

  // Zero out the register  
  *test_reg32  = 0x0;
  
  clock_init_40mhz();
  launchpad_gpio_init();
  lp_leds_init();

  UART_init(BAUD_RATE);

  msp_printf("******* PROGRAM RUNNING *******\r\n\r\n",0);
  msp_printf("      CSC202 FALL 2026\r\n",0);
  msp_printf("ARM M0+ C Bitwise Test Program\r\n",0);
  msp_printf("--------------------------------------------------------\r\n",0);

  // Display the size of the test register
  msp_printf("The size of the test reg is 0x%X bytes\r\n", sizeof(*test_reg32));

  // Display the value of the test register
  msp_printf("The starting value of test reg is 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 1: Set the PIE bit in test register (test_reg32)
  // ***************************************************************************
  msp_printf("PROBLEM 1: Setting PIE bit\r\n", 0);

  *test_reg32 = set_bit(*test_reg32, PIE_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 2: Set the RD bit in test register
  // ***************************************************************************
  msp_printf("PROBLEM 2: Setting RD bit\r\n", 0);

  *test_reg32 = set_bit(*test_reg32, RD_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 3: Set the CRS bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 3: Setting CRS bits\r\n", 0);

  *test_reg32 = set_bit(*test_reg32, CRS_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 4: Set the A[3:0] bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 4: Setting A[3:0] bits\r\n", 0);

  *test_reg32 = set_bit(*test_reg32, A3A2A1A0_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 5: Use an IF statement to test it A2 bit is set
  //            if A2 = 1 then print "Bit A2 is 1"
  //            else print "The bit A2 is 0"
  // ***************************************************************************
  msp_printf("PROBLEM 5: Testing bit A2\r\n", 0);

  if (check_bit(*test_reg32, A2_BIT_MASK)) {
    msp_printf("    Bit A2 is 1", 0);
  } else {
    msp_printf("    Bit A2 is 0", 0);
  }

  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 6: Clear A2 bit in test register
  // ***************************************************************************
  msp_printf("PROBLEM 6: Clearing A[2] bit\r\n", 0);

  *test_reg32 = clear_bit(*test_reg32, A2_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 7: Clear CRS bits and set PRS bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 7: Clear CRS bits and set PRS bits\r\n", 0);

  *test_reg32 = clear_bit(*test_reg32, CRS_BIT_MASK);
  *test_reg32 = set_bit(*test_reg32, PRS_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 8: Use an IF statement to test if A2 is set
  //            if A2 = 1 then
  //                print "Bit A2=1 so clearing it"
  //                modify the reg to clear the bit
  //            else
  //                print "Bit A2=0 so setting it"
  //                modify the reg to set the bit
  // ***************************************************************************
  msp_printf("PROBLEM 8: Testing bit A2\r\n", 0);

  if (check_bit(*test_reg32, A2_BIT_MASK)) {
    msp_printf("    Bit A2=1 so clearing it", 0);
    reg_value = clear_bit(*test_reg32, A2_BIT_MASK);
  } else {
    msp_printf("    Bit A2=0 so setting it", 0);
    reg_value = set_bit(*test_reg32, A2_BIT_MASK);
  }

  *test_reg32 = reg_value;

  msp_printf("\r\n",0);
  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 9: Use an IF statement to test it MD is 0
  //            if MD = 0 then
  //                print "Bit MD=0, setting mode=10"
  //                set MODE to 10
  //            else
  //                print "Bit MD=1, setting mode=11"
  //                set MODE to 11
  // ***************************************************************************
  msp_printf("PROBLEM 9: Testing bit MD & setting mode bits\r\n", 0);

  if (check_bit(*test_reg32, MD_BIT_MASK)) {
    msp_printf("Bit MD=0, setting mode=10", 0);
    reg_value = set_bit(*test_reg32, MODE_BIT_MASK);
    reg_value = clear_bit(reg_value, MODE_LSB_BIT_MASK);
  } else {
    msp_printf("Bit MD=1, setting mode=11", 0);
    reg_value = set_bit(*test_reg32, MODE_BIT_MASK);
  }


  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 10: Clear all bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 10: Clearing all bits\r\n", 0);

  *test_reg32 = 0x0;

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);

  msp_printf(" *** PROGRAM TERMINATED ***\r\n",0);
  
  // loop here forever to prevent program from terminating
  for(;;);

} /* main */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function sets the specified bit(s) in a 32-bit register value using
//  the provided bit mask. It performs a bitwise OR operation to set the bit(s).
//
// INPUT PARAMETERS:
//  reg_value - The original 32-bit register value.
//  bit_mask  - The 32-bit mask indicating which bit(s) to clear.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  uint32_t - The modified register value with the specified bit(s) set.
// -----------------------------------------------------------------------------

uint32_t set_bit(uint32_t reg_value, uint32_t bit_mask){
    return reg_value | bit_mask;
} /* set bit */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function clears the specified bit(s) in a 32-bit register value using
//  the provided bit mask. It performs a bitwise AND operation with the
//  complement of the bit mask to clear the bit(s).
//
// INPUT PARAMETERS:
//  reg_value - The original 32-bit register value.
//  bit_mask  - The 32-bit mask indicating which bit(s) to clear.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  uint32_t - The modified register value with the specified bit(s) cleared.
// -----------------------------------------------------------------------------

uint32_t clear_bit(uint32_t reg_value, uint32_t bit_mask){
    return reg_value & (~bit_mask);
} /* set bit */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function checks if the specified bit(s) in a 32-bit register value are
//  set using the provided bit mask. It performs a bitwise AND operation to 
//  verify if the bit(s) are set.
//
// INPUT PARAMETERS:
//  reg_value - a 32-bit register value to check.
//  bit_mask  - a 32-bit mask indicating which bit(s) to check.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  bool - true if the specified bit(s) are set, false otherwise.
// -----------------------------------------------------------------------------

bool check_bit(uint32_t reg_value, uint32_t bit_mask){
    return (reg_value & bit_mask) == bit_mask;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  A print function that prints to the serial console
//
// INPUT PARAMETERS:
//  buffer - Test that will be output 
//  value  - a numeric value that can be included in the output
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// -----------------------------------------------------------------------------

void msp_printf(char* buffer, unsigned int value)
{
    unsigned int i = 0; 
    unsigned int len = 0;
    char string[80];

    len = sprintf(string, buffer, value);

    // Walk through array to send each character to serial port
    for ( i = 0 ; i< len; i++)
    {
        UART_out_char(string[i]);
    } /* for */
} /* msp printf */