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

/* Debugging print */
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

/* Test thread */
void uart_test()
{
    uint8 *pkt_start = NULL;
    uart = vos_dev_open(VOS_DEV_UART);
    dev_conf_uart(921600);
    dev_dma_acquire(uart);
    uart_dbg("boot...", 1, 1);
    for(;;)
    {
        uart_dbg("waiting for packet...", 1, 1);
        if (packet_process_blocking(uart, cmd_buffer, PACKET_IMAGE_MAX_LEN, &pkt_start, 10))
        {
            uart_reply(UART_PROC_APID_LSB, 0, UART_PROC_CRC);
            usb_run_sequence();
        }
    }
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
    cmd_buffer[PACKET_UART_REPLY_LEN - 2] = crc << 8;
    cmd_buffer[PACKET_UART_REPLY_LEN - 1] = crc & 0xFF;

    return (vos_dev_write(uart, cmd_buffer, PACKET_UART_REPLY_LEN, NULL) == UART_OK);
}

/* Wait for a new block of data from UART */
uint8 uart_new_block(uart_proc_t *proc, uint32 initial_timeout_ms)
{
    uint8 res = TRUE, failed = FALSE, retries = UART_MAX_RETRY;
    uint16 pkt_len = 0, apid = 0;
    uint8 *pkt_start = NULL;
    packet_header_t *header = NULL;

    while (res && retries > 0 && proc->data_len < proc->block_len)
    {
        /* Get next blob from the bus */
        if (proc->blob_num > 0)
        {
            /* Notify we are ready for next blob (unless failed previously) */
            if (!failed) res = uart_reply(UART_READY_APID_LSB, proc->blob_seq, UART_READY_CRC);
            if (res) pkt_len = packet_process_timeout(uart, cmd_buffer,
                PACKET_IMAGE_MAX_LEN, &pkt_start, UART_TIMEOUT_MS);
            else break;
        }

        /* Wait for first blob from the bus */
        else pkt_len = packet_process_timeout(uart, cmd_buffer,
            PACKET_IMAGE_MAX_LEN, &pkt_start, initial_timeout_ms);

        /* Verify packet length and APID */
        if (pkt_len)
        {
            header = (packet_header_t*) (pkt_start + PACKET_SYNC_LEN);
            apid = (header->apid_msb << 8) | header->apid_lsb;
            failed = (apid != UART_BLOB_APID);
        }
        else failed = TRUE;

        /* If failed, retry up to 3 times per ICD */
        if (failed)
        {
            retries--;
            res = uart_reply(UART_RETRANSMIT_APID_LSB, proc->blob_seq, UART_RETRANSMIT_CRC);
        }

        /* Otherwise, process new data */
        else
        {            
            proc->blob_num++;
            // ...
        }
    }

    return res;
}
