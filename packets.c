/*
** Filename: packets.c
** Author: Ondrej
** 
** Packet definitions and helpers
*/

#include "packets.h"
#include "dev_conf.h"
#include "spi_handler.h"

/* Pre-defined UART flow control packets (little endian) */
const uint32 UART_REPLY_PROCESSING[] = { PACKET_SYNC_MARKER_LE, 0x1502, 0x15BB0100 };
const uint32 UART_REPLY_RETRANSMIT[] = { PACKET_SYNC_MARKER_LE, 0x3002, 0xC6BB0100 };
const uint32 UART_REPLY_HEARTBEAT[]  = { PACKET_SYNC_MARKER_LE, 0x2502, 0x8D5C0100 };
const uint32 UART_REPLY_READY[]      = { PACKET_SYNC_MARKER_LE, 0x2002, 0x83E40100 };

/* Process an incoming packet with DMA already enabled */
uint16 packet_process_dma(VOS_HANDLE dev, uint8 *buf, uint16 bufsize, uint16 *offset)
{
    uint32 sync = 0;
    uint8 *read_ptr = buf, no_data_count = 0;
    uint16 avail = 1, read = 0, total_read = 0, packet_read = 0, packet_len = 0, crc = 0;
    packet_header_t *header = NULL;
    common_ioctl_cb_t iocb;

    /* Begin read operations */
    do
    {
        /* The first read will be blocking */
        if (avail > 0 && vos_dev_read(dev, read_ptr, avail, &read) == SPISLAVE_OK)
        {
            total_read += read;
            while (read)
            {
                /* Waiting for sync marker */
                if (!PACKET_SYNC_VALID(sync))
                {
                    read--;
                    PACKET_SYNC_UPDATE(sync, read_ptr++);
                    if (PACKET_SYNC_VALID(sync))
                    {
                        /* Save packet start offset */
                        *offset = read_ptr - buf - PACKET_SYNC_LEN;
                        header = (packet_header_t*) read_ptr;
                        packet_read += PACKET_SYNC_LEN;
                    }
                }
                /* Read all available packet data */
                else
                {
                    packet_read += read;
                    read_ptr += read;
                    read = 0;
                    if (packet_len == 0 && packet_read >= PACKET_OVERHEAD)
                    {
                        packet_len = PACKET_OVERHEAD;
                        packet_len += ((header->len_msb << 8) | header->len_lsb) + 1;
                    }
                }
            }
        }

        /* End if packet is already read successfully */
        if (packet_len > 0 && packet_read >= packet_len) break;

        /* Check Rx queue status */
        iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
        vos_dev_ioctl(dev, &iocb);
        avail = iocb.get.queue_stat;

        /* Give up if queue is repeatedly empty after a few queries */
        if (avail == 0) no_data_count++;
        else no_data_count = 0;
    }
    while (no_data_count < 5 && (total_read + avail) <= bufsize);

    /* Check if we got enough data, if not, return zero */
    if (packet_read < packet_len) packet_len = 0;

    if (packet_len == 0)
    {
        spi_uart_dbg("[packet] noop, read", total_read);
    }
    else
    {
        crc = (buf[*offset + packet_len - 2] << 8) | buf[*offset + packet_len - 1];
        spi_uart_dbg("[packet] done, crc", crc);
    }

    return packet_len;
}