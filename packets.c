/*
** Filename: packets.c
** Author: Ondrej
** 
** Packet definitions and helpers
*/

#include "packets.h"
#include "dev_conf.h"

/* Pre-defined UART flow control packets (little endian) */
const uint32 UART_REPLY_PROCESSING[] = { PACKET_SYNC_MARKER_LE, 0x1502, 0x15BB0100 };
const uint32 UART_REPLY_RETRANSMIT[] = { PACKET_SYNC_MARKER_LE, 0x3002, 0xC6BB0100 };
const uint32 UART_REPLY_HEARTBEAT[]  = { PACKET_SYNC_MARKER_LE, 0x2502, 0x8D5C0100 };
const uint32 UART_REPLY_READY[]      = { PACKET_SYNC_MARKER_LE, 0x2002, 0x83E40100 };

/* Process an incoming packet with DMA already enabled */
uint16 packet_process_dma(VOS_HANDLE dev, uint8 *buffer, uint16 bufsize)
{
    uint8 res = 0;
    uint32 sync = 0;
    uint16 available = 0, processed = 0, num_read = 0, packet_len = 0;
    packet_header_t *header = (packet_header_t*) (buffer + PACKET_SYNC_LEN);
    common_ioctl_cb_t iocb;

    /* Begin; the first read is blocking */
    do
    {
        /* Waiting for sync marker */
        if (sync != PACKET_SYNC_MARKER)
        {
            res = vos_dev_read(dev, (uint8*) (&sync), 1, &num_read);
            if (res == SPISLAVE_OK && num_read == 1)
            {
                if (sync == PACKET_SYNC_MARKER)
                {
                    processed += PACKET_SYNC_LEN;
                    buffer += PACKET_SYNC_LEN;
                }
                else sync <<= 8;
            }
        }

        /* Read all available data */
        else
        {
            res = vos_dev_read(dev, buffer, available, &num_read);
            if (res == SPISLAVE_OK)
            {
                processed += num_read;
                buffer += num_read;
                if (packet_len == 0 && processed >= PACKET_OVERHEAD)
                {
                    packet_len = PACKET_OVERHEAD;
                    packet_len += ((header->len_msb << 8) | header->len_lsb) + 1;
                }
            }
        }

        /* Check Rx queue status */
        iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
        vos_dev_ioctl(dev, &iocb);
        available = iocb.get.queue_stat;
    }
    while (available > 0 && (processed + available) <= bufsize);

    /* Check if we got enough data, if not, return zero */
    if (processed < packet_len) packet_len = 0;

    return packet_len;
}