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
#define USB_PID 0x2764
#define USB_SECTOR_LEN 512

/* Stage 1 block limited by telemetry RAM buffer size */
#define USB_STAGE1_CLUSTER_LEN 8
#define USB_STAGE1_BLOCK_LEN (USB_STAGE1_CLUSTER_LEN * USB_SECTOR_LEN)

/* Stage 2/3 block slightly smaller to prevent buffer overrun due to UART */
#define USB_STAGE2_3_CLUSTER_LEN 7
#define USB_STAGE2_3_BLOCK_LEN (USB_STAGE2_3_CLUSTER_LEN * USB_SECTOR_LEN)

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
