/*
** Filename: uart_handler.c
** Author: Ondrej
** 
** Handles UART logic
*/

#include "uart_handler.h"
#include "usb_handler.h"
#include "packets.h"
#include "dev_conf.h"
#include "crc.h"

/* Pre-defined UART flow control packets (little endian) */
static const uint32 UART_REPLY_PROCESSING[] = { PACKET_SYNC_MARKER_LE, 0x1502, 0x15BB0100 };
static const uint32 UART_REPLY_RETRANSMIT[] = { PACKET_SYNC_MARKER_LE, 0x3002, 0xC6BB0100 };
static const uint32 UART_REPLY_HEARTBEAT[]  = { PACKET_SYNC_MARKER_LE, 0x2502, 0x8D5C0100 };
static const uint32 UART_REPLY_READY[]      = { PACKET_SYNC_MARKER_LE, 0x2002, 0x83E40100 };

/* Debugging print */
void uart_dbg(char *msg, uint16 number1, uint16 number2)
{
    char buf[128];
    sprintf(buf, "%s: %d 0x%X\r\n", msg, number1, number2);
    vos_dev_write(uart, (uint8*) buf, strlen(buf), NULL);
}

/* Starts the reprogramming sequence */
void uart_run_sequence()
{
    uart_dbg("[uart] starting reprogramming", 1, 1);
}
