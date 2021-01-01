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

/* Control endpoint transfer during usb boot stage
** See ep_write()/ep_read(): https://github.com/raspberrypi/usbboot/blob/master/main.c */
static uint8 usb_ctrl_xfer(usbhost_ep_handle ctrl, uint32 len, uint8 *buf)
{
    usbhost_ioctl_cb_t iocb;
    usb_deviceRequest_t request;

    /* Prepare request structure */
    request.bmRequestType = USB_BMREQUESTTYPE_VENDOR;
	request.bRequest = 0;
	request.wValue = len & 0xFFFF;
	request.wIndex = len >> 16;
	request.wLength = 0;

    /* Check if data out is expected */
    if (buf)
    {
        request.bmRequestType |= USB_BMREQUESTTYPE_DEV_TO_HOST;
        request.wLength = len & 0xFFFF;
    }

    return FALSE;
}

/* Bulk write during usb boot stage */
static uint8 usb_bulk_write(dev_usb_boot_t *dev, uint16 len, uint8 *buf)
{

    return FALSE;
}

/* First stage - send embedded bootloader code */
uint8 usb_first_stage(dev_usb_boot_t *dev)
{
    return TRUE;
}
