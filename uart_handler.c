/*
** Filename: uart_handler.c
** Author: Ondrej
** 
** Handles UART logic
*/

#include "uart_handler.h"
#include "packets.h"
#include "dev_conf.h"
#include "crc.h"

volatile uint8 reprogramming = FALSE;
static uint8 uart_buf[PACKET_IMAGE_MAX_LEN];

void uart_handler()
{
    uint8 val = 0;
    uint16 read = 0, test = 0;
    for(;;)
    {
        if(vos_dev_read(uart, &val, 1, &read) == UART_OK)
        {
            dev_dma_release(bus_spi);
            dev_dma_release(payload_spi);
            dev_dma_acquire(uart);
            test = dev_get_rx_avail(uart);
            val = val;
        }
    }
}