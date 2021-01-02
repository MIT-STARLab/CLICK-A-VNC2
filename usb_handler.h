/*
** Filename: usb_handler.h
** Author: Ondrej
** 
** Handles USB logic
*/

#ifndef _usb_handler_H_
#define _usb_handler_H_

#include "dev_conf.h"

/* First bootloader init message */
typedef struct {
    uint32 length;
    uint8 signature[20];
} usb_boot_msg_t;

void usb_run_sequence();

#endif /* _usb_handler_H_ */
