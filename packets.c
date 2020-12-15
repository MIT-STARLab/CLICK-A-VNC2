/*
** Filename: packets.c
** Author: Ondrej
** 
** Packet definitions and helpers
*/

#include "packets.h"
#include "dev_conf.h"

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

/* Finalize packet processing */
static uint16 packet_finalize(packet_proc_t *proc, uint8 **pkt_start)
{
    /* Check if we got enough data, if not, return zero */
    if (proc->pkt_read < proc->pkt_len) proc->pkt_len = 0;

    /* If successful, save packet start pointer */
    else if (proc->pkt_len > 0) *pkt_start = proc->header - PACKET_SYNC_LEN;

    return proc->pkt_len;
}

/* Process an incoming packet in blocking mode (DMA must be enabled already) */
uint16 packet_process_blocking(VOS_HANDLE dev, uint8 *buf, uint16 bufsize, uint8 **start, uint16 no_data_lmit)
{
    packet_proc_t proc = { 0, 0, 0, 0, 0, 0 };
    uint16 read = 0, no_data_count = 0;

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
        if (proc.avail == 0) no_data_count++;
        else no_data_count = 0;
    }
    while (no_data_count < no_data_lmit && (proc.total_read + proc.avail) <= bufsize);

    return packet_finalize(&proc, start);
}

/* Process an incoming packet in timeout mode (DMA must be enabled already) */
uint16 packet_process_timeout(VOS_HANDLE dev, uint8 *buf, uint16 bufsize, uint8 **start, uint16 timeout)
{
    packet_proc_t proc = { 0, 0, 0, 0, 0, 0 };
    uint16 read = 0;

    /* Read until timeout is reached or buffer overflows */
    while (timeout > 0 && (proc.total_read + proc.avail) <= bufsize)
    {
        if (proc.avail > 0 && vos_dev_read(dev, buf, proc.avail, &read) == 0)
        {
            packet_process_data(buf, read, &proc);
            buf += read;
        }

        /* Exit if packet is already read successfully */
        if (proc.pkt_len > 0 && proc.pkt_read >= proc.pkt_len) break;

        /* Check Rx queue status, sleep if empty until timout is reached */
        proc.avail = dev_rx_avail(dev);
        if (proc.avail == 0)
        {
            vos_delay_msecs(1);
            timeout--;
        }
    }

    return packet_finalize(&proc, start);
}
