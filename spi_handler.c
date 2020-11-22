/*
** Filename: spi_handler.c
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#include "spi_handler.h"
#include "packets.h"
#include "MemMgmt.h"

extern VOS_HANDLE uart;
#include "string.h"
#include "stdio.h"

static void spi_uart_dbg(char *msg, uint16 number, uint8 thread)
{
    char buf[128];
    sprintf(buf, "%s %s: %d\r\n", thread ? "[bus]" : "[payload]", msg, number);
    vos_dev_write(uart, (uint8*) buf, strlen(buf), NULL);
}

// Attempts to lock the read mutex
// For some odd reason vos_trylock_mutex does not work... good job FTDI
// Work-around with classic lock and atomic guard
static inline uint8 spi_read_ready(vos_mutex_t *lock)
{
    static uint8 ready = VOS_MUTEX_UNLOCKED;
    VOS_ENTER_CRITICAL_SECTION
    ready = LOCKED(lock);
    if(ready == VOS_MUTEX_UNLOCKED)
    {
        vos_lock_mutex(lock);
    }
    VOS_EXIT_CRITICAL_SECTION
    return ready;
}

// Handle reading and signaling write thread
void spi_read_handler(VOS_HANDLE spi,
                      uint8 *buf,
                      uint16 max_data,
                      vos_mutex_t *read_lock,
                      vos_mutex_t *write_lock,
                      uint8 send_interrupt)
{
    uint8 interrupt = 0;
    uint16 available = max_data;
    packet_header_t *header = (packet_header_t*) (buf + PACKET_SYNC_LEN);

    spi_uart_dbg("read started", 1, send_interrupt);

    for(;;)
    {
        // Wait for sync marker
        packet_wait_for_sync(spi);

        // Read into memory only if there is no pending write
        if(spi_read_ready(read_lock) == VOS_MUTEX_UNLOCKED)
        {
            // Read packet length from header
            vos_dev_read(spi, (uint8*) header, PACKET_HEADER_LEN, NULL);
            available = (header->len_msb << 8) | header->len_lsb;
            if(available >= 0)
            {
                // Avoid buffer overflow
                available = (available + 1) > max_data ? max_data : (available + 1);
                vos_dev_read(spi, buf + PACKET_OVERHEAD, available, NULL);
                
                // Signal thread and send interrupt
                VOS_ENTER_CRITICAL_SECTION
                vos_unlock_mutex(read_lock);
                vos_unlock_mutex(write_lock);
                if (send_interrupt)
                {
                    vos_gpio_write_pin(GPIO_A_7, interrupt);
                    interrupt ^= 1;
                }
                VOS_EXIT_CRITICAL_SECTION
            }
            else
            {
                // Ignore corrupt packet
                vos_unlock_mutex(read_lock);
            }

            spi_uart_dbg("processed packet len", PACKET_OVERHEAD + available, send_interrupt);
        }
        // If there is a pending write and we missed a packet, at least send an interrupt
        else if (send_interrupt)
        {
            spi_uart_dbg("read mutex is locked", 0, send_interrupt);
            vos_gpio_write_pin(GPIO_A_7, interrupt);
            interrupt ^= 1;
        }
    }
}

// Handle writing on the other bus
void spi_write_handler(VOS_HANDLE spi,
                       uint8 *buf,
                       uint16 max_data,
                       vos_mutex_t *read_lock,
                       vos_mutex_t *write_lock)
{
    uint16 available = max_data;
    packet_header_t *header = (packet_header_t*) (buf + PACKET_SYNC_LEN);
    for(;;)
    {
        vos_lock_mutex(write_lock);
        vos_lock_mutex(read_lock);
        available = (header->len_msb << 8) | header->len_lsb;
        available = (available + 1) > max_data ? max_data : (available + 1);
        vos_dev_write(spi, buf, PACKET_OVERHEAD + available, NULL);
        vos_unlock_mutex(read_lock);
    }
}
