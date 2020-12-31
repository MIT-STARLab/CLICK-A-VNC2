/*
** Filename: dev_conf.c
** Author: Ondrej
** 
** Handle device configuration
*/

#include "dev_conf.h"

#include "uart_handler.h"

/* Configure IOMUX connections */
void dev_conf_iomux()
{
    vos_iomux_define_input(15, IOMUX_IN_SPI_SLAVE_0_CLK);
    vos_iomux_define_input(16, IOMUX_IN_SPI_SLAVE_0_MOSI);
    vos_iomux_define_output(18, IOMUX_OUT_SPI_SLAVE_0_MISO);
    vos_iomux_define_input(19, IOMUX_IN_SPI_SLAVE_0_CS);
    vos_iomux_define_input(20, IOMUX_IN_SPI_SLAVE_1_CLK);
    vos_iomux_define_input(21, IOMUX_IN_SPI_SLAVE_1_MOSI);
    vos_iomux_define_output(22, IOMUX_OUT_SPI_SLAVE_1_MISO);
    vos_iomux_define_input(23, IOMUX_IN_SPI_SLAVE_1_CS);
    vos_iomux_define_output(31, IOMUX_OUT_UART_TXD);
    vos_iomux_define_input(32, IOMUX_IN_UART_RXD);
    vos_iomux_define_output(33, IOMUX_OUT_GPIO_PORT_A_2);
    vos_iomux_define_output(34, IOMUX_OUT_GPIO_PORT_A_7);
    vos_iomux_define_output(45, IOMUX_OUT_GPIO_PORT_A_4);
}

/* Configure SPI slave */
void dev_conf_spi(VOS_HANDLE spi, uint8 polarity, uint8 phase)
{
    common_ioctl_cb_t iocb;

    iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_MODE;
    iocb.set.param = SPI_SLAVE_MODE_UNMANAGED;
    vos_dev_ioctl(spi, &iocb);

    iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPHA;
    iocb.set.param = phase;
    vos_dev_ioctl(spi, &iocb);

    iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPOL;
    iocb.set.param = polarity;
    vos_dev_ioctl(spi, &iocb);
}

/* Configure UART */
void dev_conf_uart(VOS_HANDLE uart, uint32 baud)
{
    common_ioctl_cb_t iocb;

    iocb.ioctl_code = VOS_IOCTL_UART_SET_BAUD_RATE;
    iocb.set.uart_baud_rate = baud;
    vos_dev_ioctl(uart, &iocb);

    iocb.ioctl_code = VOS_IOCTL_UART_SET_FLOW_CONTROL;
    iocb.set.param = UART_FLOW_NONE;
    vos_dev_ioctl(uart, &iocb);
}

/* Wait and acquire a USB boot device with a specific serial number */
uint8 dev_usb_boot_wait(uint8 serial_num, dev_usb_boot_t *dev, uint32 timeout_ms)
{
    usbhost_ioctl_cb_t iocb;
    usb_deviceRequest_t request;
    usb_deviceDescriptor_t descriptor;
    usbhost_device_handle_ex if_query = 0, if_boot = 0;
    usbhost_ioctl_cb_vid_pid_t vid_pid = { RPI_USB_VID, RPI_BOOT_PID };

    /* Prepare descriptor request */
    request.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;
    request.bmRequestType = USB_BMREQUESTTYPE_DEV_TO_HOST |
        USB_BMREQUESTTYPE_STANDARD |
        USB_BMREQUESTTYPE_DEVICE;
    request.wValue = (USB_DESCRIPTOR_TYPE_DEVICE << 8) | 0x00;
    request.wIndex = 0x0000;
    request.wLength = 0x0012;

    uart_dbg("Waiting for device...", 1, 1);

    /* Start querying the USB bus */
    for(; timeout_ms > 250; timeout_ms -= 250)
    {
        /* Try finding the device interface by VID and PID */
        iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_FIND_HANDLE_BY_VID_PID;
        iocb.handle.dif = NULL;
        iocb.set = &vid_pid;
        iocb.get = &if_query;
        if (vos_dev_ioctl(usb, &iocb) == USBHOST_OK && if_query != NULL)
        {
            uart_dbg("Found vid_pid", 1, 1);
            /* If found, try to get the 2nd interface (the 1st might be mass storage instead) 
            ** See Initialize_Device: https://github.com/raspberrypi/usbboot/blob/master/main.c */
            if_boot = if_query;
            iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_NEXT_HANDLE;
            iocb.handle.dif = if_query;
            iocb.get = &if_query;
            vos_dev_ioctl(usb, &iocb);
            
            /* If found, set it as primary interface, otherwise we should be okay */
            if (if_query)
            {
                uart_dbg("Found 2nd if", 1, 1);
                if_boot = if_query;
            }

            /* Next, try get the control endpoint */
            iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_CONTROL_ENDPOINT_HANDLE;
            iocb.handle.dif = if_boot;
            iocb.get = &dev->ctrl;
            if (vos_dev_ioctl(usb, &iocb) == USBHOST_OK)
            {
                uart_dbg("Acq ctrl", 1, 1);
                /* Now get the descriptor and verify the serial number */
                iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_SETUP_TRANSFER;
                iocb.handle.ep = dev->ctrl;
                iocb.set = &request;
                iocb.get = &descriptor;
                if (vos_dev_ioctl(usb, &iocb) == USBHOST_OK && descriptor.iSerialNumber == serial_num)
                {
                    uart_dbg("Serial matches", 1, 1);
                    /* Finally, get the bulk transfer endpoint */
                    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_BULK_OUT_ENDPOINT_HANDLE;
                    iocb.handle.dif = if_boot;
                    iocb.get = &dev->bulk;
                    if (vos_dev_ioctl(usb, &iocb) == USBHOST_OK)
                    {
                        uart_dbg("Acq bulk", 1, 1);
                        return TRUE;
                    }
                }
            }
        }
        else uart_dbg("USB error", 1, 1);
        vos_delay_msecs(250);
    }

    uart_dbg("USB timeout", 1, 1);

    return FALSE;
}

/* Check status of the Rx queue */
uint16 dev_rx_avail(VOS_HANDLE dev)
{
    common_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
    vos_dev_ioctl(dev, &iocb);
    return iocb.get.queue_stat;
}

/* Acquire exclusive DMA access for device
** Decreases overhead for read/write operations */
void dev_dma_acquire(VOS_HANDLE dev)
{
    common_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
    iocb.set.param = DMA_ACQUIRE_AND_RETAIN;
    vos_dev_ioctl(dev, &iocb);
}

/* Release exclusive DMA access for device */
void dev_dma_release(VOS_HANDLE dev)
{
    common_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_COMMON_DISABLE_DMA;
    vos_dev_ioctl(dev, &iocb);
}
