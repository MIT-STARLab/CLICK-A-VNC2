/*
** Filename: uart_packets.c
** Author: Ondrej
** 
** Handles UART packets
*/

#include "uart_packets.h"

static uart_packet_t packet;

// Reprogramming flow control packets
const unsigned char UART_REPLY_PROCESSING[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x15, 0, 0, 0, 1, 0xBB, 0x15};
const unsigned char UART_REPLY_RETRANSMIT[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x30, 0, 0, 0, 1, 0xBB, 0xC6};
const unsigned char UART_REPLY_HEARTBEAT[]  = {UART_PACKET_SYNC_MARKER, 0x02, 0x25, 0, 0, 0, 1, 0x5C, 0x8D};
const unsigned char UART_REPLY_READY[]      = {UART_PACKET_SYNC_MARKER, 0x02, 0x20, 0, 0, 0, 1, 0xE4, 0x83};

// Reset packet to zero position
void uart_packet_reset()
{
    packet.currentLength = 0;
}

// Process new UART data
int uart_packet_process(unsigned char *data, unsigned short len, uart_packet_t *ptr)
{
    memcpy(&packet.header, data, (size_t) len);
    return UART_PACKET_ERROR;
}