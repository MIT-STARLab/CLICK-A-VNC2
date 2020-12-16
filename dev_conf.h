/*
** Filename: dev_conf.h
** Author: Ondrej
** 
** Handle device configuration
*/

#ifndef _dev_conf_H_
#define _dev_conf_H_

#define VOS_DEV_USBHOST_1 0
#define VOS_DEV_UART 1
#define VOS_DEV_SPI_SLAVE_0 2
#define VOS_DEV_SPI_SLAVE_1 3
#define VOS_DEV_GPIO_PORT_A 4
#define VOS_DEV_TIMER_0 5
#define VOS_DEV_TIMER_1 6
#define VOS_NUMBER_DEVICES 7

#define GPIO_RPI_IRQ GPIO_A_2
#define GPIO_RPI_RESET GPIO_A_7

#include "vos.h"
#include "SPISlave.h"
#include "UART.h"
#include "GPIO.h"
#include "USBHost.h"
#include "IOCTL.h"
#include "Timers.h"

/* Device globals */
extern VOS_HANDLE bus_spi;
extern VOS_HANDLE payload_spi;
extern VOS_HANDLE uart;
extern VOS_HANDLE timer_wd;
extern VOS_HANDLE timer_pkt;

/* Device initialization */
void dev_conf_iomux();
void dev_conf_spi(VOS_HANDLE spi, uint8 polarity, uint8 phase);
void dev_conf_uart(VOS_HANDLE uart, uint32 baud);
void dev_conf_timer(VOS_HANDLE timer, uint8 mode);
void dev_conf_usb(VOS_HANDLE usb);

/* Helpers */
uint16 dev_timer_status(VOS_HANDLE timer);
void dev_timer_start(VOS_HANDLE timer, uint16 timeout);
void dev_timer_stop(VOS_HANDLE timer);
void dev_timer_wait(VOS_HANDLE timer);
void dev_dma_acquire(VOS_HANDLE dev);
void dev_dma_release(VOS_HANDLE dev);
uint16 dev_rx_avail(VOS_HANDLE dev);

#endif /* _dev_conf_H_ */
