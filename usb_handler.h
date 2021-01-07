/*
** Filename: usb_handler.h
** Author: Ondrej
** 
** Handles USB logic
*/

#ifndef _usb_handler_H_
#define _usb_handler_H_

#include "vos.h"

#define RPI_USB_VID 0x0A5C
#define RPI_BOOT_PID 0x2764
#define RPI_MSD_PID 0x0001
#define RPI_DEFAULT_BLOCK_LEN 512
#define RPI_STAGE1_BLOCK_LEN 4096

/* USB boot message format */
typedef struct {
    int msg;
    uint8 signature[20];
} usb_boot_msg_t;

void usb_run_sequence();

#endif /* _usb_handler_H_ */
