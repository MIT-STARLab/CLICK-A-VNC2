/*
** Filename: uart_packets.c
** Author: Ondrej
** 
** Handles UART packets
*/

#include "uart_packets.h"
#include "crc.h"

static size_t current_length = 0;
static unsigned short current_crc = 0;

// Reprogramming flow control packets
const unsigned char UART_REPLY_PROCESSING[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x15, 0, 0, 0, 1, 0xBB, 0x15};
const unsigned char UART_REPLY_RETRANSMIT[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x30, 0, 0, 0, 1, 0xBB, 0xC6};
const unsigned char UART_REPLY_HEARTBEAT[]  = {UART_PACKET_SYNC_MARKER, 0x02, 0x25, 0, 0, 0, 1, 0x5C, 0x8D};
const unsigned char UART_REPLY_READY[]      = {UART_PACKET_SYNC_MARKER, 0x02, 0x20, 0, 0, 0, 1, 0xE4, 0x83};

// Reset to zero position
void uart_packet_reset()
{
    current_length = 0;
    current_crc = 0;
}

// Sanity check on packet header
int uart_packet_verify_header(uart_packet_t *packet)
{
    // Blob len = CCSDS packet length + 1 - 6 (2nd header) - 2 (CRC-16)
    size_t blob_len = packet->header.length - 7;

    if (blob_len <= 0 || blob_len > UART_PACKET_BLOB_MAX_LEN)
    {
        // TODO: error
        return 0;
    }

    if (packet->header.type != 1)
    {
        // TODO: warning
    }

    if (packet->header.APID != 0x200)
    {
        // TODO: warning
    }

    packet->blob_len = blob_len;
    return 1;
}

// Update running values
void uart_packet_update_values(unsigned char **data, size_t amount, size_t *len, size_t *pos)
{
    current_length += amount;
    current_crc = crc_16_update(current_crc, *data, amount);
    *data += amount;
    *pos += amount;
    *len -= amount;
}

// Process new UART data
int uart_packet_process_data(unsigned char *data, size_t len, uart_packet_t *packet)
{
    // Take care of header parsing
    size_t position = current_length;
    size_t remaining = UART_PACKET_HEADER_LEN - position;
    size_t amount = (len >= remaining) ? remaining : len;
    if (amount > 0)
    {
        memcpy(&packet->header + position, data, amount);
        uart_packet_update_values(&data, amount, &len, &position);
        if (amount == remaining)
        {
            if (!uart_packet_verify_header(packet))
            {
                uart_packet_reset();
                return UART_PACKET_ERROR;
            }
        }
        else return UART_PACKET_WAIT_DATA;
    }

    // Handle image blob copy
    position -= UART_PACKET_HEADER_LEN;
    remaining = packet->blob_len - position;
    amount = (len >= remaining) ? remaining : len;
    if (amount > 0)
    {
        memcpy(&packet->blob + position, data, amount);
        uart_packet_update_values(&data, amount, &len, &position);
        if (remaining > amount) return UART_PACKET_WAIT_DATA;
    }

    // Handle CRC copy
    position -= packet->blob_len;
    remaining = 2 - position;
    amount = (len >= remaining) ? remaining : len;
    if (amount > 0)
    {
        memcpy(&packet->crc + position, data, amount);
        uart_packet_update_values(&data, amount, &len, &position);
        if (remaining > amount) return UART_PACKET_WAIT_DATA;
    }

    // Too much data
    if (len > 0)
    {
        // TODO: warning
    }

    // Check CRC
    if(crc_16_finalize(current_crc) != packet->crc)
    {
        uart_packet_reset();
        return UART_PACKET_ERROR;
    }
    else return UART_PACKET_SUCCESS;
}