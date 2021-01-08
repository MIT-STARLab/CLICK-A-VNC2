/*
** Filename: dev_conf.c
** Author: Ondrej
** 
** Handle device configuration and low-level access
*/

#include "dev_conf.h"
#include "uart_handler.h"
#include "usb_handler.h"

/* Device globals */
VOS_HANDLE bus_spi = NULL;
VOS_HANDLE payload_spi = NULL;
VOS_HANDLE uart = NULL;
VOS_HANDLE usb = NULL;
VOS_HANDLE boms = NULL;

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
    vos_iomux_define_bidi(34, IOMUX_IN_GPIO_PORT_A_7, IOMUX_OUT_GPIO_PORT_A_7);
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
void dev_conf_uart(uint32 baud)
{
    common_ioctl_cb_t iocb;

    iocb.ioctl_code = VOS_IOCTL_UART_SET_BAUD_RATE;
    iocb.set.uart_baud_rate = baud;
    vos_dev_ioctl(uart, &iocb);

    iocb.ioctl_code = VOS_IOCTL_UART_SET_FLOW_CONTROL;
    iocb.set.param = UART_FLOW_NONE;
    vos_dev_ioctl(uart, &iocb);
}

/* Prepare the USB stack */
void dev_usb_start()
{
    usbhost_context_t usb_conf;
    if (usb == NULL)
    {
        usb_conf.if_count = 2;
        usb_conf.ep_count = 4;
        usb_conf.xfer_count = 2;
        usb_conf.iso_xfer_count = 0;
        usbhost_init(VOS_DEV_USBHOST_1, -1, &usb_conf);
        usb = vos_dev_open(VOS_DEV_USBHOST_1);
    }
}

/* Get USB port status */
uint8 dev_usb_status()
{
    usbhost_ioctl_cb_t iocb;
    uint8 state = PORT_STATE_DISCONNECTED;
    iocb.ioctl_code = VOS_IOCTL_USBHOST_GET_CONNECT_STATE;
    iocb.get = &state;
    vos_dev_ioctl(usb, &iocb);
    return state;
}

/* Reset a halted endpoint */
void dev_usb_reset_ep(usbhost_ep_handle ep)
{
    uint8 status = 0;
    usbhost_ioctl_cb_t iocb;

    iocb.handle.ep = ep;
    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_CLEAR_ENDPOINT_TRANSFER;
    status = vos_dev_ioctl(usb, &iocb);

    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_CLEAR_ENDPOINT_CARRY;
    status = vos_dev_ioctl(usb, &iocb);

    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_CLEAR_HOST_HALT;
    status = vos_dev_ioctl(usb, &iocb);
}

/* Wait for USB enumeration until timeout */
uint8 dev_usb_wait(uint32 timeout_ms)
{
    uint8 state = PORT_STATE_DISCONNECTED;
    usbhost_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_USBHOST_GET_CONNECT_STATE;
    iocb.get = &state;

    /* Poll state until timeout */
    for(; timeout_ms > 250; timeout_ms -= 250)
    {
        vos_dev_ioctl(usb, &iocb);
        if (state == PORT_STATE_ENUMERATED)
        {
            iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_COUNT;
            vos_dev_ioctl(usb, &iocb);
            return state;
        }
        else vos_delay_msecs(250);
    }

    return FALSE;
}

/* Acquire a USB boot device */
uint8 dev_usb_boot_acquire(dev_usb_boot_t *dev)
{
    uint8 status = 0, success = FALSE;
    usbhost_ioctl_cb_t iocb;
    usb_deviceRequest_t request;
    usb_deviceDescriptor_t descriptor;
    usbhost_device_handle interface = NULL;
    usbhost_ep_handle endpoint = NULL;
    usbhost_ioctl_cb_vid_pid_t vid_pid = { USB_VID, USB_BOOT_PID };

    /* Prepare descriptor request */
    vos_memset(dev, 0, sizeof(dev_usb_boot_t));
    vos_memset(&descriptor, 0, sizeof(usb_deviceDescriptor_t));
    request.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;
    request.bmRequestType = USB_BMREQUESTTYPE_DEV_TO_HOST |
        USB_BMREQUESTTYPE_STANDARD |
        USB_BMREQUESTTYPE_DEVICE;
    request.wValue = (USB_DESCRIPTOR_TYPE_DEVICE << 8) | 0x00;
    request.wIndex = 0x0000;
    request.wLength = 0x0012;

    /* Get interface via VID/PID */
    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_FIND_HANDLE_BY_VID_PID;
    iocb.handle.dif = NULL;
    iocb.set = &vid_pid;
    iocb.get = &interface;
    status = vos_dev_ioctl(usb, &iocb);
    status = (status == USBHOST_OK && interface != NULL);

    /* Acquire control endpoint */
    if (status)
    {
        iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_CONTROL_ENDPOINT_HANDLE;
        iocb.handle.dif = interface;
        iocb.set = NULL;
        iocb.get = &dev->ctrl;
        status = vos_dev_ioctl(usb, &iocb);
        status = (status == USBHOST_OK && dev->ctrl != NULL);
    }

    /* Save the serial number from the device descriptor */
    if (status)
    {
        iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_SETUP_TRANSFER;
        iocb.handle.ep = dev->ctrl;
        iocb.set = &request;
        iocb.get = &descriptor;
        status = vos_dev_ioctl(usb, &iocb);
        status = (status == USBHOST_OK);
        dev->sn = descriptor.iSerialNumber;
    }
    
    /* Acquire bulk-out endpoint */
    if (status)
    {
        iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_BULK_OUT_ENDPOINT_HANDLE;
        iocb.handle.dif = interface;
        iocb.set = NULL;
        iocb.get = &dev->bulk;
        status = vos_dev_ioctl(usb, &iocb);
        status = (status == USBHOST_OK && dev->bulk != NULL);
        success = status;
    }

    /* Try to acquire the second bulk-out endpoint.
    ** For some reason, if there are two, the first one doesn't work.
    ** Could be related to the comment in Initialize_Device:
    ** https://github.com/raspberrypi/usbboot/blob/master/main.c */
    if (status)
    {
        iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_NEXT_ENDPOINT_HANDLE;
        iocb.handle.ep = dev->bulk;
        iocb.get = &endpoint;
        if (vos_dev_ioctl(usb, &iocb) == USBHOST_OK && endpoint != NULL)
        {
            dev->bulk = endpoint;
        }
    }

    return success;
}

/* Acquire a mass storage USB device */
uint8 dev_usb_boms_acquire()
{
    usbhost_ioctl_cb_t iocb;
    usbhost_device_handle interface = NULL;
    usbhost_ioctl_cb_class_t class;
    msi_ioctl_cb_t boms_iocb;
    boms_ioctl_cb_attach_t boms_att;

    /* Find BOMS class device interface */
    class.dev_class = USB_CLASS_MASS_STORAGE;
    class.dev_subclass = USB_SUBCLASS_MASS_STORAGE_SCSI;
    class.dev_protocol = USB_PROTOCOL_MASS_STORAGE_BOMS;
    iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_FIND_HANDLE_BY_CLASS;
    iocb.handle.dif = NULL;
    iocb.set = &class;
    iocb.get = &interface;

    if (vos_dev_ioctl(usb, &iocb) == USBHOST_OK && interface != NULL)
    {
        /* Attach the BOMS driver to the interface */
        if (boms == NULL) boms = vos_dev_open(VOS_DEV_BOMS_DRV);
        boms_att.hc_handle = usb;
        boms_att.ifDev = interface;
        boms_iocb.ioctl_code = MSI_IOCTL_BOMS_ATTACH;
        boms_iocb.set = &boms_att;
        boms_iocb.get = NULL;
        return (vos_dev_ioctl(boms, &boms_iocb) == MSI_OK);
    }

    return FALSE;
}

/* Reset RPi CPU
** Needs to be reset twice with about 2 sec delay for some reason
** Afterward, the USB enumeration takes about 8-10 sec */
void dev_rpi_reset()
{
    /* Configure pin as output */
    vos_gpio_set_pin_mode(GPIO_RPI_RESET, 1);

    /* Run reset sequence */
    vos_gpio_write_pin(GPIO_RPI_RESET, GPIO_RPI_RESET_INACTIVE);
    vos_delay_msecs(100);
    vos_gpio_write_pin(GPIO_RPI_RESET, GPIO_RPI_RESET_ACTIVE);
    vos_delay_msecs(1);
    vos_gpio_write_pin(GPIO_RPI_RESET, GPIO_RPI_RESET_INACTIVE);
    vos_delay_msecs(2000);
    vos_gpio_write_pin(GPIO_RPI_RESET, GPIO_RPI_RESET_ACTIVE);
    vos_delay_msecs(1);
    vos_gpio_write_pin(GPIO_RPI_RESET, GPIO_RPI_RESET_INACTIVE);
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
