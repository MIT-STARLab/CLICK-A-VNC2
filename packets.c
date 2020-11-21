/*
** Filename: packets.c
** Author: Ondrej
** 
** Packet definitions and helpers
*/

#include "packets.h"

// Pre-defined UART flow control packets
const uint32 UART_REPLY_PROCESSING[] = { PACKET_SYNC_MARKER, 0x02150000, 0x0001BB15 };
const uint32 UART_REPLY_RETRANSMIT[] = { PACKET_SYNC_MARKER, 0x02300000, 0x0001BBC6 };
const uint32 UART_REPLY_HEARTBEAT[]  = { PACKET_SYNC_MARKER, 0x02250000, 0x00015C8D };
const uint32 UART_REPLY_READY[]      = { PACKET_SYNC_MARKER, 0x02200000, 0x0001E483 };

// Wait for sync marker
void packet_wait_for_sync(VOS_HANDLE dev)
{
    uint32 sync = 0;
    uint8 *lsb = &sync + 3;
    for(;;)
    {
        vos_dev_read(dev, lsb, 1, NULL);
        if (sync == PACKET_SYNC_MARKER)
        {
            break;
        }
        sync <<= 8;
    }
}
