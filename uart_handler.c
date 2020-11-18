/*
** Filename: uart_handler.c
** Author: Ondrej
** 
** Handles low-level UART logic with bus interface
*/

#include "uart_handler.h"
#include "uart_packets.h"
#include "vos.h"
#include "UART.h"
#include "string.h"
#include "stdio.h"
#include "click.h"
#include "crc.h"

static VOS_HANDLE hUART;

// Initialize UART
void uart_handler_init()
{
    common_ioctl_cb_t uart_iocb;

    hUART = vos_dev_open(VOS_DEV_UART);
	
	uart_iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
	uart_iocb.set.param = DMA_ACQUIRE_AS_REQUIRED; 
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_BAUD_RATE;
	uart_iocb.set.uart_baud_rate = 921600;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_FLOW_CONTROL;
	uart_iocb.set.param = UART_FLOW_NONE;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_DATA_BITS;
	uart_iocb.set.param = UART_DATA_BITS_8;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_STOP_BITS;
	uart_iocb.set.param = UART_STOP_BITS_1;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_PARITY;
	uart_iocb.set.param = UART_PARITY_NONE;
	vos_dev_ioctl(hUART, &uart_iocb);
}

// Listen to and process UART packets
void uart_handler_listen()
{
    char msg[24];
    uint8 status, buffer[128];
    uint16 crc = 0, available = 0, read = 0;
    common_ioctl_cb_t uart_iocb;
    sprintf(msg, "Ready\r\n");
    vos_dev_write(hUART, (uint8*) msg, strlen(msg), NULL);

    do
    {
        uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
		vos_dev_ioctl(hUART, &uart_iocb);
        available = uart_iocb.get.queue_stat;
        if (available > sizeof(buffer)) available = sizeof(buffer);
        if (available > 0)
        {
            status = vos_dev_read(hUART, buffer, available, &read);
            if (status != UART_OK)
            {
                sprintf(msg, "Error %d\r\n", status);
                vos_dev_write(hUART, (uint8*) msg, strlen(msg), NULL);
            }
            else
            {
                crc = crc_16_update(crc, buffer, (uint16) read);
                sprintf(msg, "New CRC: 0x%X\r\n", crc_16_finalize(crc));
                vos_dev_write(hUART, (uint8*) msg, strlen(msg), NULL);
            }
        }

    } while(1);

}

void uart_handler_reply()
{

}