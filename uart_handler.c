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

static uint8 uart_buf[PACKET_IMAGE_MAX_LEN];

void uart_handler()
{
    uint16 read = 0, test = 0;
    for(;;)
    {
        if(vos_dev_read(uart, uart_buf, 1, &read) == UART_OK)
        {
            if (read == 1)
            {
                test++;
            }
        }
    }
}