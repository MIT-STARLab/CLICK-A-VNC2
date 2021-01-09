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

/* Packet-wrapped debugging print */
void uart_dbg(char *msg, uint16 number1, uint16 number2)
{
    char buf[128];
    uint16 msg_len = 0;
    packet_header_t *hdr = (packet_header_t*) (buf + PACKET_SYNC_LEN);
    vos_memset(buf, 0, 128);
    sprintf(buf + PACKET_OVERHEAD, "%s: %d 0x%X", msg, number1, number2);
    msg_len = strlen(buf + PACKET_OVERHEAD);
    PACKET_ADD_SYNC(buf);
    hdr->len_msb = (msg_len - 1) >> 8;
    hdr->len_lsb = (msg_len - 1) & 0xFF;
    vos_dev_write(uart, (uint8*) buf, msg_len + PACKET_OVERHEAD, NULL);
}

/* UART reprogramming flow control reply */
uint8 uart_reply(uint8 apid_lsb, uint16 sequence, uint16 crc)
{
    /* Uses the smaller global command buffer */
    packet_header_t *hdr = (packet_header_t*) (cmd_buffer + PACKET_SYNC_LEN);
    vos_memset(hdr, 0, sizeof(packet_header_t));
    PACKET_ADD_SYNC(cmd_buffer);
    
    /* Write header and CRC */
    hdr->apid_msb = 2;
    hdr->apid_lsb = apid_lsb;
    hdr->seq_msb = (sequence << 8) & 0x3F;
    hdr->seq_lsb = sequence & 0xFF;
    hdr->len_lsb = 1;
    cmd_buffer[PACKET_UART_REPLY_LEN - 2] = crc >> 8;
    cmd_buffer[PACKET_UART_REPLY_LEN - 1] = crc & 0xFF;

    return (vos_dev_write(uart, cmd_buffer, PACKET_UART_REPLY_LEN, NULL) == UART_OK);
}

/* Wait for a new block of data from UART */
uint8 uart_get_block(uart_proc_t *proc, uint32 initial_timeout_ms)
{
    uint8 res = TRUE, failed = FALSE, retries = UART_MAX_RETRY + 1;
    uint16 pkt_len = 0, blob_size = 0, apid = 0, i = 0;
    uint8 *pkt_start = NULL;
    packet_header_t *header = NULL;

    /* If there is data remaining from previous run, shift it left.
    ** The global telemetry buffer is used to store USB blocks.
    ** The global command buffer is used to store UART blobs */
    if (proc->data_len > 0)
    {
        for (i = 0; i < proc->data_len; i++)
        {
            tlm_buffer[i] = tlm_buffer[proc->data_offset + i];
        }
    }

    /* Start blob processing loop */
    while (res && retries > 0 && proc->data_len < proc->block_len)
    {
        /* Get next blob from the bus */
        if (proc->blob_num > 0)
        {
            /* Notify we are ready for next blob (unless failed previously) */
            if (!failed) res = uart_reply(UART_READY_APID_LSB, proc->blob_seq, UART_READY_CRC);
            if (res) pkt_len = packet_process_timeout(uart, cmd_buffer,
                PACKET_TC_MAX_LEN, &pkt_start, UART_TIMEOUT_MS);
            else break;
        }

        /* Wait for first blob from the bus */
        else pkt_len = packet_process_timeout(uart, cmd_buffer,
            PACKET_TC_MAX_LEN, &pkt_start, initial_timeout_ms);

        /* Verify blob length and APID */
        if (pkt_len)
        {
            blob_size = pkt_len - PACKET_IMAGE_OVERHEAD - PACKET_IMAGE_TRIM;
            header = (packet_header_t*) (pkt_start + PACKET_SYNC_LEN);
            proc->blob_seq = (header->seq_msb << 8) | header->seq_lsb;
            apid = (header->apid_msb << 8) | header->apid_lsb;
            failed = (apid != UART_BLOB_APID || blob_size > USB_SECTOR_LEN);
        }
        else failed = TRUE;

        /* If failed, retry up to 3 times per ICD */
        if (failed)
        {
            retries--;
            vos_delay_msecs(1000);
            res = uart_reply(UART_RETRANSMIT_APID_LSB, proc->blob_seq, UART_RETRANSMIT_CRC);
        }

        /* Otherwise, process new data
        ** TODO: Fix blob len after BCT FSW update */
        else
        {
            retries = UART_MAX_RETRY + 1;
            vos_memcpy(tlm_buffer + proc->data_len, pkt_start + PACKET_IMAGE_OVERHEAD, blob_size);
            proc->data_len += blob_size;
            proc->blob_num++;
        }
    }

    /* Check for success */
    if (proc->data_len >= proc->block_len)
    {
        proc->data_offset = proc->block_len;
        proc->data_len -= proc->block_len;
        return TRUE;
    }

    return FALSE;
}
