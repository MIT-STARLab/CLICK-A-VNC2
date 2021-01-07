/*
** Filename: usb_handler.h
** Author: Ondrej
** 
** Handles USB logic
*/

#ifndef _usb_handler_H_
#define _usb_handler_H_

#include "vos.h"

/* USB hardware info */
#define USB_VID 0x0A5C
#define USB_BOOT_PID 0x2764
#define USB_MSD_PID 0x0001
#define USB_EMMC_BLOCK_LEN 512
#define USB_STAGE1_BLOCK_LEN 4096

/* First stage bootcode info */
#define USB_BOOTCODE_LEN 16702
#define USB_BOOTCODE_CRC 0xB1A4

/* Second stage MSD bootloader info */
#define USB_MSD_ELF_LEN 428476
#define USB_MSD_ELF_CRC 0xCC3B

/* USB boot message format */
typedef struct {
    int msg;
    uint8 signature[20];
} usb_boot_msg_t;

void usb_run_sequence();

#endif /* _usb_handler_H_ */
