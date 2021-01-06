/*
** Filename: usb_handler.h
** Author: Ondrej
** 
** Handles USB logic
*/

#ifndef _usb_handler_H_
#define _usb_handler_H_

#define RPI_USB_VID 0x0A5C
#define RPI_BOOT_PID 0x2764
#define RPI_MSD_PID 0x0001
#define RPI_BLOCK_LEN 512

extern unsigned char image_buf[];

void usb_run_sequence();

#endif /* _usb_handler_H_ */
