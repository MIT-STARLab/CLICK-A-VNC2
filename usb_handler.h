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

uint8 usb_first_stage(dev_usb_boot_t *dev);

#endif /* _usb_handler_H_ */
