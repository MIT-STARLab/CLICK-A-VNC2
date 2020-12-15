/*
** Filename: spi_handler.h
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#ifndef _spi_handler_H_
#define _spi_handler_H_

#include "vos.h"

#define SPI_NO_DATA_LIMIT 5
#define SPI_REPROGRAM_APID 0x7EF
#define SPI_REPROGRAM_LEN 2

/* Device globals */
extern VOS_HANDLE bus_spi;
extern VOS_HANDLE payload_spi;
extern VOS_HANDLE uart;
extern VOS_HANDLE timer;

/* SPI handlers */
void spi_handler_bus();
void spi_handler_payload();
void spi_handler_watchdog();
void spi_uart_dbg(char *msg, uint16 number1, uint16 number2);

#endif /* _spi_handler_H_ */