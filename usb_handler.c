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
** Used to signal how much data will be transmitted and to receive replies
** See ep_write()/ep_read(): https://github.com/raspberrypi/usbboot/blob/master/main.c */
static uint8 usb_ctrl_xfer(usbhost_ep_handle ctrl, uint16 len, uint8 *buf)
{
    usbhost_ioctl_cb_t iocb;
    usb_deviceRequest_t request;

    /* Prepare xfer structures */
    request.bmRequestType = USB_BMREQUESTTYPE_VENDOR;
    request.bRequest = 0;
    request.wValue = len;
    request.wIndex = 0;
    request.wLength = 0;
    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_SETUP_TRANSFER;
    iocb.handle.ep = ctrl;
    iocb.set = &request;
    iocb.get = NULL;

    /* Check if rx data is expected */
    if (buf)
    {
        request.bmRequestType |= USB_BMREQUESTTYPE_DEV_TO_HOST;
        request.wLength = len;
        iocb.get = buf;
    }

    /* Perform transfer */
    if (vos_dev_ioctl(usb, &iocb) == USBHOST_OK)
    {
        return TRUE;
    }

    return FALSE;
}

/* Bulk write during usb boot stage */
static uint8 usb_bulk_write(dev_usb_boot_t *dev, uint16 len, uint8 *buf)
{
    /* Prepare write structure */
    usbhost_xfer_t tx;
    uint16 written = 0;
    tx.ep = dev->bulk;
    tx.buf = buf;
    tx.len = len;
    tx.zero = 0;

    /* Notify device how many bytes are about to be written */
    if (usb_ctrl_xfer(dev->ctrl, len, NULL))
    {
        /* Perform bulk write */
        if (vos_dev_write(usb, (uint8*) (&tx), sizeof(usbhost_xfer_t), &written) == USBHOST_OK)
        {
            uart_dbg("written", written, 0);
            return TRUE;
        }
    }

    return FALSE;
}

/* First stage - send the boot message and the embedded bootloader code */
uint8 usb_first_stage(dev_usb_boot_t *dev)
{
    int usb_reply = -1;
    usb_boot_msg_t boot_msg;
    boot_msg.length = bootcode_bin_len;
    vos_memset(boot_msg.signature, 0, 20);

    /* Send the init boot message */
    if (usb_bulk_write(dev, sizeof(usb_boot_msg_t), (uint8*) (&boot_msg)))
    {
        /* Send the embedded bootloader binary */
        if (usb_bulk_write(dev, bootcode_bin_len, bootcode_bin))
        {
            /* Get reply */
            if (usb_ctrl_xfer(dev->ctrl, sizeof(usb_reply), (uint8*) (&usb_reply)))
            {
                uart_dbg("read", (uint16) usb_reply, 0);
                if (usb_reply == 0) return TRUE;
            }
        }
    }

    return FALSE;
}
