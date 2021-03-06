/*
** Filename: dev_conf.h
** Author: Ondrej
** 
** Handle device configuration and low-level access
*/

#ifndef _dev_conf_H_
#define _dev_conf_H_

#define VOS_DEV_USBHOST_1 0
#define VOS_DEV_UART 1
#define VOS_DEV_SPI_SLAVE_0 2
#define VOS_DEV_SPI_SLAVE_1 3
#define VOS_DEV_BOMS_DRV 4
#define VOS_NUMBER_DEVICES 5

#define GPIO_RPI_IRQ GPIO_A_2
#define GPIO_RPI_EMMC GPIO_A_4
#define GPIO_RPI_RESET GPIO_A_7
#define GPIO_RPI_RESET_ACTIVE 1
#define GPIO_RPI_RESET_INACTIVE 0

#include "vos.h"
#include "SPISlave.h"
#include "UART.h"
#include "USB.h"
#include "USBHost.h"
#include "IOCTL.h"
#include "BOMS.h"

/* Device globals */
extern VOS_HANDLE bus_spi;
extern VOS_HANDLE payload_spi;
extern VOS_HANDLE uart;
extern VOS_HANDLE usb;
extern VOS_HANDLE boms;

/* USB boot-stage device */
typedef struct {
    uint8 sn;
    usbhost_ep_handle ctrl;
    usbhost_ep_handle bulk;
} dev_usb_boot_t;

/* Device initialization */
void dev_conf_iomux();
void dev_conf_spi(VOS_HANDLE spi, uint8 polarity, uint8 phase);
uint8 dev_uart_start();
uint8 dev_usb_start();

/* Helpers */
void dev_dma_acquire(VOS_HANDLE dev);
void dev_dma_release(VOS_HANDLE dev);
uint16 dev_rx_avail(VOS_HANDLE dev);
uint8 dev_usb_status();
uint8 dev_usb_wait(uint32 timeout_ms);
uint8 dev_usb_boot_acquire(dev_usb_boot_t *dev);
uint8 dev_usb_boms_acquire();
void dev_usb_reset_ep(usbhost_ep_handle ep);
void dev_reprogramming_cleanup();
void dev_rpi_common_reset();
void dev_rpi_bootloader_reset();

#endif /* _dev_conf_H_ */
