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
#define SPI_REPROG_APID 0x7EF
#define SPI_REPROG_PKT_LEN 12

/* SPI handlers */
void spi_handler_bus();
void spi_handler_payload();
void spi_handler_watchdog();

#endif /* _spi_handler_H_ */