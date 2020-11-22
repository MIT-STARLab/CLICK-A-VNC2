/*
** Filename: packets.c
** Author: Ondrej
** 
** Packet definitions and helpers
*/

#include "packets.h"

// Pre-defined UART flow control packets (little endian)
const uint32 UART_REPLY_PROCESSING[] = { PACKET_SYNC_MARKER_LE, 0x1502, 0x15BB0100 };
const uint32 UART_REPLY_RETRANSMIT[] = { PACKET_SYNC_MARKER_LE, 0x3002, 0xC6BB0100 };
const uint32 UART_REPLY_HEARTBEAT[]  = { PACKET_SYNC_MARKER_LE, 0x2502, 0x8D5C0100 };
const uint32 UART_REPLY_READY[]      = { PACKET_SYNC_MARKER_LE, 0x2002, 0x83E40100 };

// Wait for sync marker
void packet_wait_for_sync(VOS_HANDLE dev)
{
    uint32 sync = 0;
    for(;;)
    {
        // &sync points to LSB on little endian
        vos_dev_read(dev, (uint8*) (&sync), 1, NULL);
        if (sync == PACKET_SYNC_MARKER)
        {
            break;
        }
        sync <<= 8;
    }
}
