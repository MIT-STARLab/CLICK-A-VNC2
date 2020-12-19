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

static uint8 usb_wait_for_dev();

/* First stage - init USB, wait for dev, and send bootloader code */
uint8 usb_first_stage()
{
    
    

    return FALSE;
}
