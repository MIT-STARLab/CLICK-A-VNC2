/*
** Filename: spi_handler.h
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#ifndef _spi_handler_H_
#define _spi_handler_H_

#include "vos.h"

/* Device globals */
extern VOS_HANDLE bus_spi;
extern VOS_HANDLE payload_spi;
extern VOS_HANDLE uart;

/* SPI handlers */
void spi_handler_bus();
void spi_handler_payload();
void spi_handler_watchdog();

#endif /* _spi_handler_H_ */