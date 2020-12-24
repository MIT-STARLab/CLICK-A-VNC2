/*
** Filename: usb_handler.c
** Author: Ondrej
** 
** Handles USB logic
*/

#include "usb_handler.h"
#include "bootloader.h"
#include "dev_conf.h"

#include "uart_handler.h"

/* First stage - send embedded bootloader code */
uint8 usb_first_stage()
{
    return TRUE;
}
