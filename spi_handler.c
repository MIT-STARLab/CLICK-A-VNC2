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
                      uint16 max_data,
                      vos_mutex_t *read_lock,
                      vos_mutex_t *write_lock,
                      uint8 send_interrupt)
{
    uint8 interrupt = 0;
    uint16 available = max_data;
    packet_header_t *header = (packet_header_t*) (read_buf + PACKET_SYNC_LEN);
    PACKET_ASSIGN_SYNC(read_buf);
    for(;;)
    {
        packet_wait_for_sync(spi);
        vos_dev_read(spi, (uint8*) header, PACKET_HEADER_LEN, NULL);
        available = (header->len_msb << 8) | header->len_lsb;
        if(available >= 0 && (available + 1) <= PACKET_MAX_DATA)
        {
            available = (available + 1) > max_data ? max_data : (available + 1);
            vos_dev_read(spi, read_buf + PACKET_OVERHEAD, available, NULL);
            VOS_ENTER_CRITICAL_SECTION
            if(vos_trylock_mutex(read_lock) == VOS_MUTEX_UNLOCKED)
            {
                vos_memcpy(write_buf, read_buf, PACKET_OVERHEAD + available);
                vos_unlock_mutex(read_lock);
                vos_unlock_mutex(write_lock);
            }
            if (send_interrupt)
            {
                vos_gpio_write_pin(GPIO_A_7, interrupt);
                interrupt ^= 1;
            }
            VOS_EXIT_CRITICAL_SECTION
        }
    }
}

// Handle writing
void spi_write_handler(VOS_HANDLE spi,
                       uint8 *write_buf,
                       uint16 max_data,
                       vos_mutex_t *read_lock,
                       vos_mutex_t *write_lock)
{
    uint16 available = max_data;
    packet_header_t *header = (packet_header_t*) write_buf + PACKET_SYNC_LEN;
    for(;;)
    {
        vos_lock_mutex(write_lock);
        vos_lock_mutex(read_lock);
        available = (header->len_msb << 8) | header->len_lsb;
        available = (available + 1) > max_data ? max_data : (available + 1);
        vos_dev_write(spi, write_buf, PACKET_OVERHEAD + available, NULL);
        vos_unlock_mutex(read_lock);
    }
}