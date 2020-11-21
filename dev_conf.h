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
#define VOS_NUMBER_DEVICES 5

#include "vos.h"
#include "SPISlave.h"
#include "UART.h"
#include "GPIO.h"
#include "USBHost.h"

void dev_conf_iomux();
void dev_conf_spi(VOS_HANDLE spi);
void dev_conf_uart(VOS_HANDLE uart);
void dev_conf_usb(VOS_HANDLE usb);

#endif /* _dev_conf_H_ */