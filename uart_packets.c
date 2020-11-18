/*
** Filename: uart_packets.c
** Author: Ondrej
** 
** Handles UART packets
*/

#include "uart_packets.h"
#include "crc.h"

static size_t current_length = 0;
static uint16 current_crc = 0;

// Reprogramming flow control packets
const uint8 UART_REPLY_PROCESSING[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x15, 0, 0, 0, 1, 0xBB, 0x15};
const uint8 UART_REPLY_RETRANSMIT[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x30, 0, 0, 0, 1, 0xBB, 0xC6};
const uint8 UART_REPLY_HEARTBEAT[]  = {UART_PACKET_SYNC_MARKER, 0x02, 0x25, 0, 0, 0, 1, 0x5C, 0x8D};
const uint8 UART_REPLY_READY[]      = {UART_PACKET_SYNC_MARKER, 0x02, 0x20, 0, 0, 0, 1, 0xE4, 0x83};

// Reset to zero position
void uart_packet_reset()
{
    current_length = 0;
    current_crc = 0;
}

// Handle copying of chunks for a block of data
size_t uart_packet_handle_block(void *block_ptr, size_t block_start, size_t block_size, uint8 **data, size_t *len)
{
    size_t block_position = current_length - block_start;
    size_t remaining = block_size - block_position;
    size_t amount = (*len >= remaining) ? remaining : *len;
    if (amount > 0)
    {
        memcpy((uint8*) block_ptr + block_position, *data, amount);
        current_crc = crc_16_update(current_crc, *data, amount);
        current_length += amount;
        *data += amount;
        *len -= amount;
    }
    return (remaining - amount);
}

// Process new UART data
size_t uart_packet_process_data(uint8 *data, size_t len, uart_packet_t *packet)
{
    // Handle CCSDS header
    size_t remaining = uart_packet_handle_block(&packet->header, 0, UART_PACKET_HEADER_LEN, &data, &len);
    if (remaining == 0)
    {
        // Blob length = CCSDS packet length + 1 - 6 (2nd header) - 2 (CRC-16)
        size_t blob_len = packet->header.length - 7;
        if (blob_len <= 0 || blob_len > UART_PACKET_BLOB_MAX_LEN)
        {
            uart_packet_reset();
            return -1;
        }
        packet->blob_len = blob_len;
    }
    else return remaining;

    // Handle image blob
    remaining = uart_packet_handle_block(&packet->blob, UART_PACKET_HEADER_LEN, packet->blob_len, &data, &len);
    if (remaining > 0) return remaining;

    // Handle CRC
    remaining = uart_packet_handle_block(&packet->crc, UART_PACKET_HEADER_LEN + packet->blob_len, 2, &data, &len);
    if (remaining > 0) return remaining;

    // Check CRC
    if(crc_16_finalize(current_crc) != packet->crc)
    {
        uart_packet_reset();
        return -1;
    }
    else return 0;
}