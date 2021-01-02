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

#include "string.h"
#include "stdio.h"

/* Private variables */
static uint8 uart_buf[PACKET_IMAGE_MAX_LEN];

/* Pre-defined UART flow control packets (little endian) */
static const uint32 UART_REPLY_PROCESSING[] = { PACKET_SYNC_MARKER_LE, 0x1502, 0x15BB0100 };
static const uint32 UART_REPLY_RETRANSMIT[] = { PACKET_SYNC_MARKER_LE, 0x3002, 0xC6BB0100 };
static const uint32 UART_REPLY_HEARTBEAT[]  = { PACKET_SYNC_MARKER_LE, 0x2502, 0x8D5C0100 };
static const uint32 UART_REPLY_READY[]      = { PACKET_SYNC_MARKER_LE, 0x2002, 0x83E40100 };

/* Debugging print */
void uart_dbg(char *msg, uint16 number1, uint16 number2)
{
    char buf[256];
    uint16 msg_len = 0;
    packet_header_t *hdr = (packet_header_t*) (buf + PACKET_SYNC_LEN);
    vos_memset(buf, 0, 256);
    sprintf(buf + PACKET_OVERHEAD, "%s: %d 0x%X", msg, number1, number2);
    msg_len = strlen(buf + PACKET_OVERHEAD);
    PACKET_ADD_SYNC(buf);
    hdr->len_msb = (msg_len - 1) >> 8;
    hdr->len_lsb = (msg_len - 1) & 0xFF;
    vos_dev_write(uart, (uint8*) buf, msg_len + PACKET_OVERHEAD, NULL);
}

/* UART flow control reply */
static void uart_reply(const uint32 *pkt)
{
    vos_dev_write(uart, (uint8*) pkt, PACKET_UART_REPLY_LEN, NULL);
}

/* Test thread */
void uart_test()
{
    uint8 *pkt_start = NULL;
    while(packet_process_blocking(uart, uart_buf, PACKET_IMAGE_MAX_LEN, &pkt_start, 100))
    {
        uart_reply(UART_REPLY_PROCESSING);
        usb_run_sequence();
    }
}
