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

/* Prepare bootloader transfer to device */
static uint8 usb_prepare_boot_stage(dev_usb_boot_t *dev, uint32 length)
{
    usb_boot_msg_t boot_msg;
    boot_msg.msg = length;

    /* Notify that boot init message will be sent */
    if (usb_ctrl_xfer(dev->ctrl, NULL, sizeof(boot_msg)))
    {
        /* Send the init boot message */
        if (usb_bulk_write(dev, (uint8*) (&boot_msg), sizeof(boot_msg)))
        {
            /* Notify that bootloader will be sent */
            return usb_ctrl_xfer(dev->ctrl, NULL, length);
        }
    }

    return FALSE;
}

/* Finalize bootloader transfer to device */
static uint8 usb_finalize_boot_stage(dev_usb_boot_t *dev)
{
    int reply = -1;

    /* Verify USB reply */
    vos_delay_msecs(1000);
    if (usb_ctrl_xfer(dev->ctrl, (uint8*) (&reply), 4) && reply == 0)
    {
        /* If OK, wait and force USB re-enumeration */
        vos_delay_msecs(1000);
        dev_usb_force_enumeration();
        return TRUE;
    }

    return FALSE;
}

/* First stage - send the embedded bootloader code */
static uint8 usb_first_stage(dev_usb_boot_t *dev)
{
    uint8 res = FALSE;
    uint16 i = 0, pos = 0, avail = 0, crc = 0xFFFF;

    /* Prepare bootloader transfer */
    res = usb_prepare_boot_stage(dev, USB_BOOTCODE_LEN);

    /* Send the embedded bootloader binary */
    while (res && pos < USB_BOOTCODE_LEN)
    {
        avail = USB_BOOTCODE_LEN - pos;
        avail = avail > USB_STAGE1_BLOCK_LEN ? USB_STAGE1_BLOCK_LEN : avail;

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
    if (res && pos == USB_BOOTCODE_LEN && crc == USB_BOOTCODE_CRC)
    {
        return usb_finalize_boot_stage(dev);
    }

    return FALSE;
}

/* Second stage - send msd.elf, received through UART */
static uint8 usb_second_stage(dev_usb_boot_t *dev)
{
    uint8 res = FALSE;
    uint16 crc = 0xFFFF;
    uint32 sent = 0;
    uart_proc_t proc = { 0, 0, 0, 0, 0 };

    /* Wait for the first block with longer timeout */
    proc.block_len = USB_STAGE2_BLOCK_LEN;
    res = uart_get_block(&proc, 10000);

    /* Prepare bootloader transfer */
    if (res) res = usb_prepare_boot_stage(dev, USB_MSD_ELF_LEN);

    /* Start msd.elf write loop */
    while (res && sent < USB_MSD_ELF_LEN)
    {
        /* Write and update CRC for sanity check */
        res = usb_bulk_write(dev, tlm_buffer, proc.block_len);
        crc = crc_16_update(crc, tlm_buffer, proc.block_len);

        if (res)
        {
            sent += proc.block_len;

            /* Adjust block size for last write */
            if ((USB_MSD_ELF_LEN - sent) < USB_STAGE2_BLOCK_LEN)
            {
                proc.block_len = USB_MSD_ELF_LEN - sent;
            }

            /* Request more UART data */
            if (sent < USB_MSD_ELF_LEN)
            {
                res = uart_get_block(&proc, UART_TIMEOUT_MS);
            }
        }
    }

    /* Verify bootloader write */
    if (res && sent == USB_MSD_ELF_LEN && crc == USB_MSD_ELF_CRC)
    {
        return usb_finalize_boot_stage(dev);
    }

    return FALSE;
}

/* Enter the reprogramming sequence */
void usb_run_sequence()
{
    uint8 res = FALSE;
    dev_usb_boot_t dev = { 0, 0, 0 };

    /* Drive EMMC_DISABLE low by setting Select high
    ** This also connects the USB hub to the RPi USB slave port */
    vos_gpio_write_pin(GPIO_RPI_EMMC, 1);

    /* Start the USB stack and wait for connection */
    dev_usb_start();
    vos_delay_msecs(250);

    /* Check port state */
    if (dev_usb_status() == PORT_STATE_DISCONNECTED)
    {
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
        /* Wait and acquire the first USB boot device */
        res = dev_usb_wait(10000);
        if (res) res = dev_usb_boot_acquire(&dev);

        /* If serial numbers equals zero, run first stage */
        if (res && dev.sn == 0) res = usb_first_stage(&dev);
        else res = FALSE;

        /* Repeat for second-stage boot device */
        if (res) res = dev_usb_wait(10000);
        if (res) res = dev_usb_boot_acquire(&dev);

        /* If serial numbers equals one, run second stage */
        if (res && dev.sn == 1) res = usb_second_stage(&dev);
        else res = FALSE;

        uart_dbg("result", res, res);
    }
}
