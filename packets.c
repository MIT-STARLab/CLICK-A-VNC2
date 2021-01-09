/*
** Filename: packets.c
** Author: Ondrej
** 
** Packet definitions and helpers
*/

#include "packets.h"
#include "dev_conf.h"
#include "crc.h"

/* Global buffers */
uint8 cmd_buffer[PACKET_TC_MAX_LEN];
uint8 tlm_buffer[PACKET_TM_MAX_LEN];

/* Process new packet data */
static void packet_process_data(uint8 *data, uint16 len, packet_proc_t *proc)
{
    proc->total_read += len;
    while (len)
    {
        /* Waiting for sync marker */
        if (!PACKET_SYNC_VALID(proc->sync))
        {
            len--;
            PACKET_SYNC_UPDATE(proc->sync, data++);
            if (PACKET_SYNC_VALID(proc->sync))
            {
                /* Save packet header pointer */
                proc->header = (packet_header_t*) data;
                proc->pkt_read += PACKET_SYNC_LEN;
            }
        }
        /* Read all available packet data */
        else
        {
            proc->pkt_read += len;
            /* If available already, read packet length */
            if (proc->pkt_len == 0 && proc->pkt_read >= PACKET_OVERHEAD)
            {
                proc->pkt_len = PACKET_OVERHEAD;
                proc->pkt_len += ((proc->header->len_msb << 8) | proc->header->len_lsb) + 1;
            }
            break;
        }
    }
}

/* Check CRC and finalize packet processing */
static uint16 packet_finalize(packet_proc_t *proc, uint8 **pkt_start)
{
    uint16 pkt_crc = 0, calc_crc = 0;

    /* Check if we got enough data, if not, return zero */
    if (proc->pkt_read < proc->pkt_len) proc->pkt_len = 0;

    /* If successful so far, save packet start pointer and check CRC */
    else if (proc->pkt_len > 0)
    {
        *pkt_start = (uint8*) proc->header - PACKET_SYNC_LEN;
        pkt_crc = PACKET_GET_CRC(proc->header, proc->pkt_len - PACKET_SYNC_LEN);
        calc_crc = crc_16_update(0xFFFF, (uint8*) (proc->header), proc->pkt_len - PACKET_SYNC_LEN - 2);

        /* If CRC check fails, return zero */
        if (pkt_crc != calc_crc)
        {
            proc->pkt_len = 0;
        }
    }

    return proc->pkt_len;
}

/* Process an incoming packet in blocking mode (DMA must be enabled already) */
uint16 packet_process_blocking(VOS_HANDLE dev, uint8 *buf, uint16 bufsize, uint8 **start, uint16 no_data_limit)
{
    packet_proc_t proc = { 0, 0, 0, 0, 0, 0 };
    uint16 read = 0, no_data_cnt = 0;

    /* We know at least one byte will be transferred */
    proc.avail = 1;

    /* Begin read operations */
    do
    {
        /* The first read will be blocking */
        if (proc.avail > 0 && vos_dev_read(dev, buf, proc.avail, &read) == 0)
        {
            packet_process_data(buf, read, &proc);
            buf += read;
        }

        /* Exit if packet is already read successfully */
        if (proc.pkt_len > 0 && proc.pkt_read >= proc.pkt_len) break;

        /* Check Rx queue status, give up if it is repeatedly empty after a few queries */
        proc.avail = dev_rx_avail(dev);
        if (proc.avail == 0) no_data_cnt++;
        else no_data_cnt = 0;
    }
    while (no_data_cnt < no_data_limit && (proc.total_read + proc.avail) <= bufsize);

    return packet_finalize(&proc, start);
}

/* Process an incoming packet in a dumb timeout mode (DMA must be enabled already) */
uint16 packet_process_timeout(VOS_HANDLE dev, uint8 *buf, uint16 bufsize, uint8 **start, uint32 timeout_ms)
{
    packet_proc_t proc = { 0, 0, 0, 0, 0, 0 };
    uint16 read = 0;

    /* 1 ms seems to be about 25 no-data loops */
    timeout_ms *= 25;

    /* Read until timeout is reached or buffer overflows */
    while (timeout_ms-- && (proc.total_read + proc.avail) <= bufsize)
    {
        if (proc.avail > 0 && vos_dev_read(dev, buf, proc.avail, &read) == 0)
        {
            packet_process_data(buf, read, &proc);
            buf += read;
        }

        /* Exit if packet is already read successfully */
        if (proc.pkt_len > 0 && proc.pkt_read >= proc.pkt_len) break;

        /* Check Rx queue status */
        proc.avail = dev_rx_avail(dev);
    }

    return packet_finalize(&proc, start);
}
