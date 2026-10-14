/*******************************************************************************
**
** @file        : LinkerScript.ld
**
** @author      : Ali Abdullkareem
**
** @brief       : Linker script for STM32F103C8Tx Device from STM32F1 series
**                      64KBytes FLASH
**                      20KBytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
*******************************************************************************/

_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;
/*End of Stack  as its end of RAM (Decending)*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/*Memory*/
MEMORY
{
    RAM     (xrw) : ORIGIN = 0x20000000, LENGTH = 20k
    FLASH   (xr)  : ORIGIN = 0x08000000, LENGTH = 64k
}

/*SECTIONS*/
SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))/* Startup code */
        . = ALIGN(4);
    } >FLASH
    /*code into FLASH*/
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH
    /*Const data into flash*/
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH
    /*Initialized data into RAM(VMA) copied from FLash(LMA)*/
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT >FLASH
    /*Uninitialized data into RAM*/
    . = ALIGN(4);
    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ebss = .;
    } >RAM
     /*User_heap_stack section, used to check that there is enough "RAM" Ram  type memory left */
     ._user_heap_stack :
     {
        . = ALIGN(8);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
     } >RAM
}