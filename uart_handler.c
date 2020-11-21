/*
** Filename: uart_handler.c
** Author: Ondrej
** 
** Handles UART logic
*/

#include "uart_handler.h"
#include "packets.h"
#include "UART.h"
#include "string.h"
#include "stdio.h"
#include "crc.h"

// Listen to and process UART packets
void uart_handler_listen(VOS_HANDLE uart)
{
    char msg[24];
    uint8 status, buffer[128];
    uint16 crc = 0, available = 0, read = 0;
    common_ioctl_cb_t uart_iocb;
    sprintf(msg, "Ready\r\n");
    vos_dev_write(uart, (uint8*) msg, strlen(msg), NULL);

    do
    {
        uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
        vos_dev_ioctl(uart, &uart_iocb);
        available = uart_iocb.get.queue_stat;
        if (available > sizeof(buffer)) available = sizeof(buffer);
        if (available > 0)
        {
            status = vos_dev_read(uart, buffer, available, &read);
            if (status != UART_OK)
            {
                sprintf(msg, "Error %d\r\n", status);
                vos_dev_write(uart, (uint8*) msg, strlen(msg), NULL);
            }
            else
            {
                crc = crc_16_update(crc, buffer, (uint16) read);
                sprintf(msg, "New CRC: 0x%X\r\n", crc_16_finalize(crc));
                vos_dev_write(uart, (uint8*) msg, strlen(msg), NULL);
            }
        }

    } while(1);

}

void uart_handler_reply()
{

}