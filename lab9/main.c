//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ethan Callahan
//
//       LAB NAME:  Lab 9
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
#include "adc.h"
#include "clock.h"
#include "demo_melodies.h"
#include "lcd1602.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void    run_lab9(void);
uint8_t get_duty_cycle(void);
void    enable_active_buzzer(void);
void    enable_passive_buzzer(void);
void    disable_active_buzzer(void);
void    disable_passive_buzzer(void);

void motor0_set_pwm_freq(uint16_t frequency);

void config_pb1_interrupts(void);
void config_pb2_interrupts(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define MAX_FREQ 4000;

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool PROGRAM_RUNNING = true;
typedef enum
{
  BUZZER_IDLE,
  ACTIVE_DC_TEST,
  PASSIVE_DC_TEST,
  ACTIVE_DC_SWEEP,
  PASSIVE_FREQ_SWEEP,
  PLAY_MELODY
} fsm_state_t;

fsm_state_t CURRENT_STATE = BUZZER_IDLE;
fsm_state_t NEXT_STATE    = BUZZER_IDLE;

int main(void)
{
  // Configure the LaunchPad board
  clock_init_40mhz();
  launchpad_gpio_init();

  config_pb1_interrupts();
  config_pb2_interrupts();

  keypad_init();
  I2C_mstr_init();
  lcd1602_init();
  dipsw_init();
  leds_init();
  leds_enable();
  leds_on(0x6);

  motor0_init();
  motor0_pwm_init(0xff, 0xff);

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  run_lab9();

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void run_lab9(void)
{

  while (PROGRAM_RUNNING)
  {
    // Check PB2 to advance state
    // if (is_pb_down(PB2_IDX))
    // {
    //   CURRENT_STATE = NEXT_STATE;

    //   // debounce
    //   msec_delay(5);
    //   while (is_pb_down(PB2_IDX))
    //     ;
    // }

    uint8_t  duty_cycle = get_duty_cycle();
    uint16_t frequency  = duty_cycle * 40;

    switch (CURRENT_STATE)
    {
      case BUZZER_IDLE:
        msec_delay(10);
        NEXT_STATE = ACTIVE_DC_TEST;

        disable_passive_buzzer();
        disable_active_buzzer();

        motor0_set_pwm_freq(frequency);
        motor0_set_pwm_dc(duty_cycle);

        motor0_pwm_enable();

        // Display on LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("Buzzers Disabled");
        break;

      case ACTIVE_DC_TEST:
        msec_delay(10);
        NEXT_STATE = PASSIVE_DC_TEST;

        motor0_set_pwm_freq(frequency);
        motor0_set_pwm_dc(duty_cycle);

        enable_active_buzzer();
        motor0_pwm_enable();

        // Display on LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string(" Active DC Test ");
        break;

      case PASSIVE_DC_TEST:
        msec_delay(10);
        NEXT_STATE = ACTIVE_DC_SWEEP;

        motor0_set_pwm_freq(frequency);
        motor0_set_pwm_dc(duty_cycle);

        disable_active_buzzer();
        enable_passive_buzzer();
        motor0_pwm_enable();

        // Display on LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("Passive DC Test ");
        break;
      case ACTIVE_DC_SWEEP:
        msec_delay(10);
        NEXT_STATE = PASSIVE_FREQ_SWEEP;

        uint16_t adc_value = ADC0_in(7);

        duty_cycle = adc_value / 41;
        frequency  = adc_value;

        motor0_set_pwm_freq(frequency);
        motor0_set_pwm_dc(duty_cycle);

        disable_passive_buzzer();
        enable_active_buzzer();
        motor0_pwm_enable();

        // Display on LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("Active DC Sweep ");
        break;
      case PASSIVE_FREQ_SWEEP:
        msec_delay(10);
        NEXT_STATE = PLAY_MELODY;

        adc_value = ADC0_in(7);

        duty_cycle = adc_value / 41;
        frequency  = adc_value;

        motor0_set_pwm_freq(frequency);
        motor0_set_pwm_dc(duty_cycle);

        disable_active_buzzer();
        enable_passive_buzzer();
        motor0_pwm_enable();

        // Display on LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("Passive F Sweep");
        break;
      case PLAY_MELODY:
        msec_delay(10);
        NEXT_STATE = BUZZER_IDLE;
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("  Play Melody   ");

        disable_passive_buzzer();
        enable_active_buzzer();
        motor0_pwm_enable();

        for (uint8_t i = 0; (i < IMPERIAL_MARCH_LENGTH)&&PROGRAM_RUNNING&&(CURRENT_STATE); i++)
        {
          motor0_set_pwm_freq(imperial_march[i].freq);
          msec_delay(imperial_march[i].duration);
        }

        break;
    }
    // Display Freq/DC on LCD
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("F=");
    lcd_write_doublebyte(frequency);
    lcd_set_ddram_addr(LCD_LINE2_ADDR + 10);
    lcd_write_string("DC=");
    lcd_write_byte(duty_cycle);
  }

  lcd_clear();
  lcd_write_string("Program End");
}

uint8_t get_duty_cycle(void)
{
  uint8_t        key_value  = keypad_scan();
  static uint8_t duty_cycle = 0;

  if (key_value != NO_KEY_PRESSED)
  {
    duty_cycle = key_value * 100 / 15;
  }

  return (duty_cycle);
}

void enable_active_buzzer(void)
{
  led_off(LED_BAR_LD1_IDX);
}

void enable_passive_buzzer(void)
{
  led_off(LED_BAR_LD2_IDX);
}

void disable_active_buzzer(void)
{
  led_on(LED_BAR_LD1_IDX);
}

void disable_passive_buzzer(void)
{
  led_on(LED_BAR_LD2_IDX);
}

void motor0_set_pwm_freq(uint16_t frequency)
{
  motor0_set_pwm_count(200000 / frequency);
}

void config_pb1_interrupts(void)
{
  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;

  GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;

  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 1);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

void config_pb2_interrupts(void)
{
  GPIOA->POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;

  GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;

  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 0);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

void GROUP1_IRQHandler(void)
{
  uint32_t group_iidx_status;
  uint32_t gpio_mis = GPIOA->CPU_INT.MIS;

  do
  {
    group_iidx_status = CPUSS->INT_GROUP[1].IIDX;
    switch (group_iidx_status)
    {

      // GPIOA interrupt (PB2 Pressed)
      case (CPUSS_INT_GROUP_IIDX_STAT_INT0):
        gpio_mis = GPIOA->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) ==
            GPIO_CPU_INT_MIS_DIO15_SET)
        {
          // When PB2 pressed:
          CURRENT_STATE = NEXT_STATE;

          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        }
        break;

      // GPIOB interrupt (PB1 pressed)
      case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
        gpio_mis = GPIOB->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) ==
            GPIO_CPU_INT_MIS_DIO18_SET)
        {
          PROGRAM_RUNNING     = false;
          GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
        }
        break;

      default:
        break;
    }
  } while (PROGRAM_RUNNING && group_iidx_status != 0);
}