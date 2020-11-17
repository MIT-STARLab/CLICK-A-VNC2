/*
** Filename: uart_packets.c
** Author: Ondrej
** 
** Handles UART packets
*/

#include "uart_packets.h"

static uart_packet_t packet;

void uart_packet_reset()
{
    packet.currentLength = 0;
}

uart_packet_status_t uart_packet_process(unsigned char *data, unsigned int len, uart_packet_t *packet)
{
    return UART_PACKET_ERROR;
}