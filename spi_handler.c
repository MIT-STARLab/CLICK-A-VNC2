/*
** Filename: spi_handler.c
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#include "spi_handler.h"
#include "packets.h"
#include "MemMgmt.h"

// Handle reading and signaling write thread
void spi_read_handler(VOS_HANDLE spi,
                      uint8 *read_buf,
                      uint8 *write_buf,
                      uint16 buf_size,
                      vos_mutex_t *read_lock,
                      vos_mutex_t *write_lock,
                      uint8 send_interrupt)
{
    packet_header_t *header = (packet_header_t*) read_buf + 4;
    ((uint32*) read_buf)[0] = PACKET_SYNC_MARKER;
    for(;;)
    {
        packet_wait_for_sync(spi);
        vos_dev_read(spi, (uint8*) header, PACKET_HEADER_LEN, NULL);
        if(header->len > 0 && header->len < PACKET_MAX_DATA)
        {
            vos_dev_read(spi, read_buf + 4 + PACKET_HEADER_LEN, header->len + 1, NULL);
            VOS_ENTER_CRITICAL_SECTION
            if(vos_trylock_mutex(read_lock) == VOS_MUTEX_UNLOCKED)
            {
                vos_memcpy(write_buf, read_buf, 5 + PACKET_HEADER_LEN + header->len);
                vos_unlock_mutex(read_lock);
                vos_unlock_mutex(write_lock);
            }
            if (send_interrupt != NULL)
            {

            }
            VOS_EXIT_CRITICAL_SECTION
        }
    }
}

// Handle writing
void spi_write_handler(VOS_HANDLE spi,
                       uint8 *write_buf,
                       vos_mutex_t *read_lock,
                       vos_mutex_t *write_lock)
{
    packet_header_t *header = (packet_header_t*) write_buf + 4;
    for(;;)
    {
        vos_lock_mutex(write_lock);
        vos_lock_mutex(read_lock);
        vos_dev_write(spi, write_buf, 5 + PACKET_HEADER_LEN + header->len, NULL);
        vos_unlock_mutex(read_lock);
    }
}