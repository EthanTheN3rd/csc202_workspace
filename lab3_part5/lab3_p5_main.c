//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//
//  DESIGNER NAME:  Ethan Callahan 
//
//       LAB NAME:  Lab 3 Part 5
//
//      FILE NAME:  lab3_p5_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program runs on the Texas Instruments MSPM0 LaunchPad 
//    (p/n LP-MSPM0G3507) and also serves as hands-on exercise for practicing
//    bitwise operations in Assmebly within an embedded systems context using 
//    the MSPM0 (ARM Cortex-M0+).
//
//    Students will manipulate a simulated 32-bit register and implement
//    bit-level operations using assembly routines. The goal is to reinforce
//    how high-level C bitwise operations map to low-level processor
//    instructions.
//
//    Key concepts reinforced in this lab include:
//      - Bit masking using symbolic constants
//      - Setting, clearing, and testing individual bits
//      - Read-modify-write operations at the register level
//      - Interaction between C code and assembly routines
//
//    This lab emphasizes how C-style bitwise operations are implemented
//    using ARM assembly instructions such as ORR, AND, and BIC.
//
//    Students are required to:
//      - Define symbolic constants for bit masks
//      - Implement the following assembly functions:
//          * my_asm_bitset()
//          * my_asm_bitclr()
//          * my_asm_bitcheck()
//      - Apply these functions to complete a sequence of problems
//
//    Each problem requires modifying the register using a single
//    read-modify-write operation. After each step, the updated register
//    value or result should be displayed using the msp_printf() function
//    via the UART serial terminal.
//
//    The program output allows students to observe how assembly-level
//    bitwise operations affect the register value.
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
    }
} /* msp printf */

// TODO: Enter the prototype for your functions here

uint32_t   my_asm_bitset(uint32_t reg_value, uint32_t bit_mask);
uint32_t   my_asm_bitclr(uint32_t reg_value, uint32_t bit_mask);
uint32_t my_asm_bitcheck(uint32_t reg_value, uint32_t bit_mask);

//-----------------------------------------------------------------------------
// Define symbolic constants used by program
//-----------------------------------------------------------------------------
#define BAUD_RATE                                                       (115200)

// Symbolic constants to the bit fields with the register
// Shift left operator creates the mask automatically based on bit position 
#define PIE_BIT_MASK                                                   (1U << 0)
#define EME_BIT_MASK                                                   (1U << 1)
#define RD_BIT_MASK                                                    (1U << 2)
#define MD_BIT_MASK                                                    (1U << 3)
#define CRS_BIT_MASK                                                   (7U << 4)
#define MODE_01_BIT_VALUE                                              (1U << 7)
#define MODE_10_BIT_VALUE                                              (2U << 7)
#define MODE_BIT_MASK                                                  (3U << 7)
#define PRS_BIT_MASK                                                   (7U << 9)
#define A0_BIT_MASK                                                   (1U << 12)
#define A1_BIT_MASK                                                   (1U << 13)
#define A2_BIT_MASK                                                   (1U << 14)
#define A3_BIT_MASK                                                   (1U << 15)

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

  // Set register  
  *test_reg32  = 0xC000;
  
  clock_init_40mhz();
  launchpad_gpio_init();
  lp_leds_init();

  UART_init(BAUD_RATE);

  msp_printf("******* PROGRAM RUNNING *******\r\n\r\n",0);
  msp_printf("      CSC202 FALL 2026\r\n",0);
  msp_printf("ARM M0+ Assembly Bitwise Test Program\r\n",0);
  msp_printf("--------------------------------------------------------\r\n",0);

  // Display the size of the test register
  msp_printf("The size of the test reg is 0x%X bytes\r\n", sizeof(*test_reg32));

  // Display the value of the test register
  msp_printf("The starting value of test reg is 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 1: Set the PIE bit in test register (*test_reg32)
  // ***************************************************************************
  msp_printf("PROBLEM 1: Setting PIE bit\r\n", 0);
  *test_reg32 = my_asm_bitset(*test_reg32, PIE_BIT_MASK);
  
  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 2: Set the RD bit in test register
  // ***************************************************************************
  msp_printf("PROBLEM 2: Setting RD bit\r\n", 0);

  *test_reg32 = my_asm_bitset(*test_reg32, RD_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 3: Set the CRS bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 3: Setting CRS bits\r\n", 0);

  *test_reg32 = my_asm_bitset(*test_reg32, CRS_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 4: Set the A[3:0] bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 4: Setting A[3:0] bits\r\n", 0);

  *test_reg32 = my_asm_bitset(*test_reg32, A3_BIT_MASK);
  *test_reg32 = my_asm_bitset(*test_reg32, A2_BIT_MASK);
  *test_reg32 = my_asm_bitset(*test_reg32, A1_BIT_MASK);
  *test_reg32 = my_asm_bitset(*test_reg32, A0_BIT_MASK);
  
  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 5: Use an IF statement to test it A2 bit is set
  //            if A2 = 1 then print "Bit A2 is 1"
  //            else print "The bit A2 is 0"
  // ***************************************************************************
  msp_printf("PROBLEM 5: Testing bit A2\r\n", 0);

  bool test_bit = my_asm_bitcheck(*test_reg32, A2_BIT_MASK);
  if (test_bit) {
    msp_printf("Bit A2 is 1\r\n", 0);
  } else {
    msp_printf("Bit A2 is 0\r\n", 0);
  }

  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 6: Clear A2 bit in test register
  // ***************************************************************************
  msp_printf("PROBLEM 6: Clearing A[2] bit\r\n", 0);

  *test_reg32 = my_asm_bitclr(*test_reg32, A2_BIT_MASK);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 7: Clear CRS bits and set PRS bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 7: Clear CRS bits and set PRS bits\r\n", 0);

  *test_reg32 = my_asm_bitclr(*test_reg32, CRS_BIT_MASK);
  *test_reg32 = my_asm_bitset(*test_reg32, PRS_BIT_MASK);

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

  test_bit = my_asm_bitcheck(*test_reg32, A2_BIT_MASK);
  if (test_bit) {
    msp_printf("Bit A2=1, clearing it\r\n", 0);
    *test_reg32 = my_asm_bitclr(*test_reg32, A2_BIT_MASK);
  } else {
    msp_printf("Bit A2=0, setting it\r\n", 0);
    *test_reg32 = my_asm_bitset(*test_reg32, A2_BIT_MASK);
  }

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

  test_bit = my_asm_bitcheck(*test_reg32, MD_BIT_MASK);
  if (test_bit) {
    msp_printf("Bit MD=1, setting mode=11\r\n", 0);
    *test_reg32 = my_asm_bitset(*test_reg32, MODE_10_BIT_VALUE);
    *test_reg32 = my_asm_bitset(*test_reg32, MODE_01_BIT_VALUE);
  } else {
    msp_printf("Bit MD=0, setting mode=10\r\n", 0);
    *test_reg32 = my_asm_bitset(*test_reg32, MODE_10_BIT_VALUE);
  }

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);


  // ***************************************************************************
  // PROBLEM 10: Clear all bits in test register
  // ***************************************************************************
  msp_printf("PROBLEM 10: Clearing all bits\r\n", 0);

  *test_reg32 = my_asm_bitclr(*test_reg32, 0xFFFF);

  msp_printf("    --> Test reg = 0x%04X\r\n", *test_reg32);
  msp_printf("\r\n",0);

  msp_printf(" *** PROGRAM TERMINATED ***\r\n",0);
  
  // loop here forever to prevent program from terminating
  for(;;);

} /* main */


