/*
** Filename: spi_handler.c
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#include "spi_handler.h"
#include "packets.h"

extern VOS_HANDLE uart;
#include "string.h"
#include "stdio.h"

static void spi_uart_dbg(char *msg, uint16 number, uint8 thread)
{
    char buf[128];
    sprintf(buf, "%s %s: %d\r\n", thread ? "[bus]" : "[payload]", msg, number);
    vos_dev_write(uart, (uint8*) buf, strlen(buf), NULL);
}

// Main SPI handler
void spi_handler_pipe(spi_pipe_conf_t *conf)
{
    uint16 available = conf->max_data;
    packet_header_t *header = (packet_header_t*) (conf->buf + PACKET_SYNC_LEN);
    for(;;)
    {
        // Wait for sync marker
        packet_wait_for_sync(conf->src);

        // Read packet length from header
        vos_dev_read(conf->src, (uint8*) header, PACKET_HEADER_LEN, NULL);

        available = ((header->len_msb << 8) | header->len_lsb) + 1;
        if(available > 1)
        {
            // Avoid buffer overflow
            available = available > conf->max_data ? conf->max_data : available;

            // Read the rest of the packet
            vos_dev_read(conf->src, conf->buf + PACKET_OVERHEAD, available, NULL);

            // If we received a bus packet, wait for bus to stop clocking data from payload
            // If we received a payload packet, signal the other thread that the bus write is pending 
            vos_lock_mutex(conf->interrupt_lock);

            // Now, send interrupt to payload since bus read & write is finished
            if (conf->interrupts)
            {
                VOS_ENTER_CRITICAL_SECTION
                *(conf->interrupt_bit) ^= 1;
                vos_gpio_write_pin(GPIO_A_7, *(conf->interrupt_bit));
                VOS_EXIT_CRITICAL_SECTION

                // Unlock the payload thread to be able to write again
                vos_unlock_mutex(conf->interrupt_lock);
            }

            // Start blocking write on the opposite SPI bus
            vos_dev_write(conf->dest, conf->buf, PACKET_OVERHEAD + available, NULL);

            // If successfully written to payload, increase watchdog counter
            if (conf->interrupts)
            {
                VOS_ENTER_CRITICAL_SECTION
                *(conf->tx_counter)++;
                VOS_EXIT_CRITICAL_SECTION
            }
            // If successfully written to bus, signal interrupt handler
            else
            {
                vos_unlock_mutex(conf->interrupt_lock);
            }
        }
    }
}