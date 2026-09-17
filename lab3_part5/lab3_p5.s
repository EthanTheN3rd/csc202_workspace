.syntax unified
.cpu cortex-m0plus
.thumb


.global my_asm_bitset
.global my_asm_bitclr
.global my_asm_bitcheck

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

.thumb_func
my_asm_bitset:
    ORRS R0, R0, R1
    BX LR

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

.thumb_func
my_asm_bitclr:
    MVNS R2, R1
    ANDS R0, R0, R2
    BX LR

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

.thumb_func
my_asm_bitcheck:
    ANDS R0, R1
    CMP R0, R1
    BEQ set_bit
    MOVS R0, #0x0
    BX LR

set_bit:
    MOVS R0, #0x1
    BX LR

.end