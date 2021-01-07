/*
** Filename: usb_handler.c
** Author: Ondrej
** 
** Handles USB logic
*/

#include "usb_handler.h"
#include "bootloader.h"
#include "dev_conf.h"
#include "packets.h"
#include "uart_handler.h"
#include "crc.h"

/* Control endpoint transfer during usb boot stage
** Used to signal how much data will be transmitted and to receive replies
** See ep_write()/ep_read(): https://github.com/raspberrypi/usbboot/blob/master/main.c */
static uint8 usb_ctrl_xfer(usbhost_ep_handle ctrl, uint8 *buf, uint32 len)
{
    usbhost_ioctl_cb_t iocb;
    usb_deviceRequest_t request;

    /* Prepare xfer structures */
    request.bmRequestType = USB_BMREQUESTTYPE_VENDOR;
    request.bRequest = 0;
    request.wValue = len & 0xFFFF;
    request.wIndex = len >> 16;
    request.wLength = 0;
    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_SETUP_TRANSFER;
    iocb.handle.ep = ctrl;
    iocb.set = &request;
    iocb.get = NULL;

    /* Check if rx data is expected */
    if (buf)
    {
        request.bmRequestType |= USB_BMREQUESTTYPE_DEV_TO_HOST;
        request.wLength = len & 0xFFFF;
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
static uint8 usb_bulk_write(dev_usb_boot_t *dev, uint8 *buf, uint16 len)
{
    uint8 status = 0;
    usbhost_xfer_t tx;
    vos_semaphore_t sem;

    /* Prepare the transfer block */
    vos_memset(&tx, 0, sizeof(usbhost_xfer_t));
    vos_init_semaphore(&sem, 0);
    tx.s = &sem;
    tx.buf = buf;
    tx.ep = dev->bulk;
    tx.flags = USBHOST_XFER_FLAG_START_BULK_ENDPOINT_LIST;

    /* Perform bulk write */
    while (len)
    {
        tx.len = len;
        tx.cond_code = USBHOST_CC_NOTACCESSED;
        status = vos_dev_write(usb, (uint8*) (&tx), sizeof(usbhost_xfer_t), NULL);

        /* Advance if transfer was successful */
        if (status == USBHOST_OK && tx.cond_code == USBHOST_CC_NOERROR)
        {
            len -= tx.len;
            tx.buf += tx.len;
        }
        else if (status == USBHOST_EP_HALTED)
        {
            uart_dbg("USB clearing halt", status, status);
            dev_usb_reset_ep(dev->bulk);
        }
        else
        {
            uart_dbg("USB write error", tx.cond_code, status);
            break;
        }
    }

    return (len == 0);
}

/* First stage - send the boot message and the embedded bootloader code */
static uint8 usb_first_stage(dev_usb_boot_t *dev)
{
    uint8 res = FALSE;
    uint16 i = 0, pos = 0, avail = 0, crc = 0xFFFF;
    usb_boot_msg_t boot_msg = { BOOTCODE_LEN };

    /* Notify that boot message will be sent */
    res = usb_ctrl_xfer(dev->ctrl, NULL, sizeof(boot_msg));

    /* Send the init boot message */
    res = res ? usb_bulk_write(dev, (uint8*) (&boot_msg), sizeof(boot_msg)) : res;

    /* Notify that bootloader will be sent */
    res = res ? usb_ctrl_xfer(dev->ctrl, NULL, BOOTCODE_LEN) : res;

    /* Send the embedded bootloader binary */
    while (res && pos < BOOTCODE_LEN)
    {
        avail = RPI_STAGE1_BLOCK_LEN;
        if ((BOOTCODE_LEN - pos) < RPI_STAGE1_BLOCK_LEN)
        {
            avail = BOOTCODE_LEN - pos;
        }

        /* Temporarily load from ROM into telemetry buffer */
        for (i = 0; i < avail; i++, pos++)
        {
            tlm_buffer[i] = bootcode_bin[pos];
        }

        /* Write and update CRC for sanity check */
        res = usb_bulk_write(dev, tlm_buffer, avail);
        crc = crc_16_update(crc, tlm_buffer, avail);
    }

    /* Verify bootloader write */
    res = (pos == BOOTCODE_LEN && crc == BOOTCODE_CRC);

    /* Verify USB reply */
    if (res)
    {
        vos_delay_msecs(1000);
        res = usb_ctrl_xfer(dev->ctrl, (uint8*) (&boot_msg.msg), 4);
        if (boot_msg.msg == 0) return TRUE;
    }

    return FALSE;
}

/* Enter the reprogramming sequence */
void usb_run_sequence()
{
    uint8 success = FALSE, stage = 0;
    dev_usb_boot_t dev = { 0 };

    /* Drive EMMC_DISABLE low by setting Select high
    ** This also connects the USB hub to the RPi USB slave port */
    vos_gpio_write_pin(GPIO_RPI_EMMC, 1);

    /* Start the USB stack and wait for connection */
    dev_usb_start();
    vos_delay_msecs(250);

    /* Check port state */
    if (dev_usb_status() == PORT_STATE_DISCONNECTED)
    {
        uart_dbg("resetting RPi", 0, 0);

        /* Reset the RPi into USB bootloader mode.
        ** Following the reset, the USB enumeration happens after roughly 8-10 sec.
        ** However, due to some internal USB bug, the USB stack crashes the system when it happens.
        ** This always happens during the first USB enumeration of the first RPi boot device.
        ** The only workaround is to let the crash happen and get reset by the internal watchdog...
        ** The second enumaration seems to always be successful. WTF.
        ** The internal watchdog counter resets the VNC2L if it's not cleared periodically.
        ** The expiration time is 2^bitPos / 48e6; bitPos is given as argument below.
        ** bitPos of 29 results in a reset after about 11 sec */
        vos_wdt_enable(29);
        dev_rpi_reset();

        /* Just wait for death... */
        vos_delay_msecs(20000);
    }

    /* Begin USB bootloader stage */
    else
    {
        while (dev.sn < 1)
        {
            /* Wait for USB enumeration to complete */
            success = dev_usb_wait(10000);
            // uart_dbg("enumeration cnt", success, 1);

            if (success) success = dev_usb_boot_acquire(&dev);
            else uart_dbg("enumeration timeout", 1, 1);

            if (success)
            {
                uart_dbg("device with serial", dev.sn, 0);
                if (dev.sn == 0 && stage == 0)
                {
                    success = usb_first_stage(&dev);
                }
            }

            if (success)
            {
                if (dev.sn == 0) stage = 1;
                uart_dbg("stage finished", 1, 1);
                vos_delay_msecs(1000);
                dev_usb_force_enumeration();
            }
        }
    }
}
