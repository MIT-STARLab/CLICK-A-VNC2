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

/* Reprogramming active flag managed by UART handler */
extern volatile uint8 reprogramming;

/* Process an incoming packet with DMA already enabled */
uint16 packet_process_dma(VOS_HANDLE dev, uint8 *buf, uint16 bufsize, uint16 *offset)
{
    uint8 *read_ptr = buf;
    uint32 sync = 0, no_data_count = 0;
    uint16 avail = 0, read = 0, total_read = 0, packet_read = 0, packet_len = 0, crc = 0;
    packet_header_t *header = NULL;

    /* Begin read operations
    ** Read until at least one byte is transferred
    ** Abort read if no packet found in data and queue was repeatedly empty after 5 queries
    ** Abort read if the total amount of bytes would exceed our buffer size
    ** Abort read if dev is not UART and reprogramming is active */
    while (
        (total_read == 0 ||
        (no_data_count < 5 &&
        (total_read + avail) <= bufsize)) &&
        (dev == uart || reprogramming == FALSE))
    {
        if (avail > 0 && vos_dev_read(dev, read_ptr, avail, &read) == 0)
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

        /* Exit if packet is already read successfully */
        if (packet_len > 0 && packet_read >= packet_len) break;

        /* Check Rx queue and count no data occurences */
        avail = dev_get_rx_avail(dev);
        if (avail == 0) no_data_count++;
        else no_data_count = 0;
    }

    /* Check if we got enough data, if not, return zero */
    if (packet_read < packet_len) packet_len = 0;

    if (packet_len == 0)
    {
        if (bufsize > 2000)
        {
            if (PACKET_SYNC_VALID(sync)) spi_uart_dbg("[payload] noop w sync", packet_read, total_read - packet_read);
            else spi_uart_dbg("[payload] noop no sync", total_read, 0);
        }
        else
        {
            if (PACKET_SYNC_VALID(sync)) spi_uart_dbg("[bus] noop w sync", packet_read, total_read - packet_read);
            else spi_uart_dbg("[bus] noop no sync", total_read, 0);
        }
    }
    else
    {
        crc = (buf[*offset + packet_len - 2] << 8) | buf[*offset + packet_len - 1];
        if (bufsize > 2000) spi_uart_dbg("[payload] done len", packet_len, crc);
        else spi_uart_dbg("[bus] done len", packet_len, crc);
    }

    return packet_len;
}