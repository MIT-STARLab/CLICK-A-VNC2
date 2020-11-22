/*
** Filename: spi_handler.h
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#ifndef _spi_handler_H_
#define _spi_handler_H_

#include "vos.h"

// Handle reading and signaling write thread
void spi_read_handler(VOS_HANDLE spi,
                      uint8 *buf,
                      uint16 max_data,
                      vos_mutex_t *read_lock,
                      vos_mutex_t *write_lock,
                      uint8 send_interrupt);

// Handle writing
void spi_write_handler(VOS_HANDLE spi,
                       uint8 *buf,
                       uint16 max_data,
                       vos_mutex_t *read_lock,
                       vos_mutex_t *write_lock);

#endif /* _spi_handler_H_ */