/*
** Filename: spi_handler.h
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#ifndef _spi_handler_H_
#define _spi_handler_H_

#include "vos.h"

// SPI pipe configuration
typedef struct {
    VOS_HANDLE src;
    VOS_HANDLE dest;
    uint8 *buf;
    uint16 max_data;
    uint8 interrupts;
    vos_mutex_t *interrupt_lock;
    uint8 *interrupt_bit;
    uint32 *tx_counter;
} spi_pipe_conf_t;

// Main SPI handler
void spi_handler_pipe(spi_pipe_conf_t *conf);

#endif /* _spi_handler_H_ */