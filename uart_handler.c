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

