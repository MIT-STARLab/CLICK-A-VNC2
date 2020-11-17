/*
** Filename: uart_handler.c
** Author: Ondrej
** 
** Handles low-level UART logic with bus interface
*/

#include "uart_handler.h"

void uart_handler_init()
{
    common_ioctl_cb_t uart_iocb;
	
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

void uart_handler_listen()
{

}

void uart_handler_reply()
{

}