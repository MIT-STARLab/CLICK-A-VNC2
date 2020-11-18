/*
** Filename: uart_packets.c
** Author: Ondrej
** 
** Handles UART packets
*/

#include "uart_packets.h"
#include "crc.h"

static uart_packet_t packet;
static unsigned int current_length;
static unsigned short current_crc;

// Reprogramming flow control packets
const unsigned char UART_REPLY_PROCESSING[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x15, 0, 0, 0, 1, 0xBB, 0x15};
const unsigned char UART_REPLY_RETRANSMIT[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x30, 0, 0, 0, 1, 0xBB, 0xC6};
const unsigned char UART_REPLY_HEARTBEAT[]  = {UART_PACKET_SYNC_MARKER, 0x02, 0x25, 0, 0, 0, 1, 0x5C, 0x8D};
const unsigned char UART_REPLY_READY[]      = {UART_PACKET_SYNC_MARKER, 0x02, 0x20, 0, 0, 0, 1, 0xE4, 0x83};

// Reset packet to zero position
void uart_packet_reset()
{
    current_length = 0;
    current_crc = 0;
}

// Sanity check on packet header
int uart_packet_verify_header()
{
    // Blob len = CCSDS packet length + 1 - 6 (2nd header) - 2 (CRC-16)
    int blob_len = packet.header.length - 7;

    if (blob_len <= 0 || blob_len > UART_PACKET_BLOB_MAX_LEN)
    {
        // TODO: error
        return 0;
    }

    if (packet.header.preamble.field.type != 1)
    {
        // TODO: warning
    }

    if (packet.header.preamble.field.APID != 0x200)
    {
        // TODO: warning
    }

    packet.blob_len = blob_len;
    return 1;
}

// Update running values
void uart_packet_update_values(unsigned char *data, unsigned int len)
{
    current_crc = crc_16(current_crc, data, len);
    current_length += len;
}

// Process new UART data
int uart_packet_process_data(unsigned char *data, unsigned int len, uart_packet_t *ptr)
{
    // Take care of header parsing
    if (current_length < UART_PACKET_HEADER_LEN)
    {
        size_t remaining = UART_PACKET_HEADER_LEN - current_length;
        if (len >= remaining)
        {
            memcpy(&packet.header + current_length, data, remaining);
            if (uart_packet_verify_header())
            {
                uart_packet_update_values(data, len);
                data += remaining;
                len -= remaining;
            }
            else
            {
                uart_packet_reset();
                return UART_PACKET_ERROR;
            }
        }
        else
        {
            memcpy(&packet.header + current_length, data, (size_t)len);
            uart_packet_update_values(data, len);
            return UART_PACKET_WAIT_DATA;
        }
    }

    // Calculate remaining image data
    // size_t remaining =

    return UART_PACKET_ERROR;
}