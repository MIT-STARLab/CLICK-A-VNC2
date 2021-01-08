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

    return (vos_dev_ioctl(usb, &iocb) == USBHOST_OK);
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
        else if (status == USBHOST_EP_HALTED) dev_usb_reset_ep(dev->bulk);
        else break;
    }

    return (len == 0);
}

/* Write to a mass storage device */
static uint8 usb_msd_write(unsigned long sector, uint8 *buf, uint16 len)
{
    uint8 status = 0;
    msi_xfer_cb_t tx;
    vos_semaphore_t sem;

    /* Prepare the transfer block */
    vos_memset(&tx, 0, sizeof(msi_xfer_cb_t));
    vos_init_semaphore(&sem, 0);
    tx.sector = sector;
    tx.buf = buf;
    tx.total_len = len;
    tx.buf_len = len;
    tx.status = MSI_NOT_ACCESSED;
    tx.s = &sem;
    tx.do_phases = MSI_PHASE_ALL;

    status = vos_dev_write(boms, (uint8*) (&tx), sizeof(msi_xfer_cb_t), NULL);
    return (status == MSI_OK && (tx.status & BOMS_HC_CC_ERROR) == 0);
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

    /* Verify USB reply equals zero */
    if (usb_ctrl_xfer(dev->ctrl, (uint8*) (&reply), 4) && reply == 0)
    {
        /* If OK, wait for USB re-enumeration */
        vos_delay_msecs(1000);
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

        /* Temporarily load from ROM into RAM telemetry buffer */
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
static uint8 usb_second_stage(dev_usb_boot_t *dev, uart_proc_t *proc)
{
    uint8 res = FALSE;
    uint16 crc = 0xFFFF;
    uint32 sent = 0;

    uart_dbg("waiting for uart", 0, 0);

    /* Wait for the first block with longer UART timeout */
    proc->block_len = USB_STAGE2_3_BLOCK_LEN;
    res = uart_get_block(proc, 30000);

    /* Prepare bootloader transfer */
    if (res) res = usb_prepare_boot_stage(dev, USB_MSD_ELF_LEN);

    /* Start msd.elf write loop */
    while (res && sent < USB_MSD_ELF_LEN)
    {
        /* Write and update CRC for sanity check */
        res = usb_bulk_write(dev, tlm_buffer, proc->block_len);
        crc = crc_16_update(crc, tlm_buffer, proc->block_len);

        if (res)
        {
            sent += proc->block_len;

            /* Adjust block size for last write */
            if ((USB_MSD_ELF_LEN - sent) < USB_STAGE2_3_BLOCK_LEN)
            {
                proc->block_len = USB_MSD_ELF_LEN - sent;
            }

            /* Request more UART data */
            if (sent < USB_MSD_ELF_LEN)
            {
                res = uart_get_block(proc, UART_TIMEOUT_MS);
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

/* Third stage - write golden image, received through UART */
static uint8 usb_third_stage(uart_proc_t *proc)
{
    uint8 res = FALSE, cluster_len = 0;
    uint32 image_sectors = 0;
    unsigned long sector = 0;

    /* Get image size (in sectors).
    ** Encoded as a 4 byte integer after msd.elf, before the golden image starts */
    proc->block_len = 4;
    res = uart_get_block(proc, UART_TIMEOUT_MS);
    if (res) image_sectors = *((uint32*) tlm_buffer);
    res = (image_sectors > 0);

    /* Default block size */
    cluster_len = USB_STAGE2_3_CLUSTER_LEN;
    proc->block_len = USB_STAGE2_3_BLOCK_LEN;

    /* Start golden image write loop */
    while (res && sector < image_sectors)
    {
        /* Adjust block size for last write */
        if ((image_sectors - sector) < USB_STAGE2_3_CLUSTER_LEN)
        {
            cluster_len = image_sectors - sector;
            proc->block_len = cluster_len * USB_EMMC_SECTOR_LEN;
        }

        /* Get UART data and write to USB MSD */
        res = uart_get_block(proc, UART_TIMEOUT_MS);
        if (res) res = usb_msd_write(sector, tlm_buffer, proc->block_len);
        if (res) sector += cluster_len;

        if (sector % 2051 == 0)
        {
            uart_dbg("MB sent", (uint16) (sector / 2048), 0);
        }
    }

    return (res && sector == image_sectors);
}

/* Enter the reprogramming sequence */
void usb_run_sequence()
{
    uint8 res = FALSE;
    dev_usb_boot_t dev = { 0, 0, 0 };
    uart_proc_t proc = { 0, 0, 0, 0, 0 };

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
        dev_rpi_bootloader_reset();

        /* Just wait for death... */
        vos_delay_msecs(20000);
    }

    /* Begin USB sequence */
    else
    {
        /* Wait and acquire the first USB boot device */
        res = dev_usb_wait(10000);
        if (res) res = dev_usb_boot_acquire(&dev);

        /* If serial numbers equals zero, run first stage */
        if (res && dev.sn == 0) res = usb_first_stage(&dev);
        else res = FALSE;

        /* Repeat for second-stage boot device */
        if (res) res = dev_usb_wait(5000);
        if (res) res = dev_usb_boot_acquire(&dev);

        /* If serial numbers equals one, run second stage */
        if (res && dev.sn == 1) res = usb_second_stage(&dev, &proc);
        else res = FALSE;

        /* Repeat for third-stage, the mass storage device */
        if (res) res = dev_usb_wait(5000);
        if (res) res = dev_usb_boms_acquire();
        if (res) res = usb_third_stage(&proc);

        /* Close USB driver */
        dev_usb_cleanup();

        uart_dbg("result", res, res);

        /* Drive EMMC_DISABLE back high and reset RPi */
        vos_gpio_write_pin(GPIO_RPI_EMMC, 0);
        dev_rpi_common_reset();

        /* Reboot VNC2 after completion */
        vos_reset_vnc2();
    }
}
