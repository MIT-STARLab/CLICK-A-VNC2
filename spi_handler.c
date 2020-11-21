/*
** Filename: spi_handler.c
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#include "spi_handler.h"

// Handle reading and signaling write thread
void spi_read_handler(VOS_HANDLE spi,
                      uint8 *read,
                      uint8 *write,
                      uint16 buf_size,
                      vos_mutex_t *read_lock,
                      vos_mutex_t *write_lock,
                      uint8 send_interrupt)
{
    
}