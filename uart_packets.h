/*
** Filename: uart_packets.h
** Author: Ondrej
** 
** Handles UART packets
*/

#ifndef _uart_packets_H_
#define _uart_packets_H_

#include "vos.h"
#include "string.h"

// Basic definitions
#define UART_PACKET_HEADER_LEN 12
#define UART_PACKET_BLOB_MAX_LEN 524
#define UART_PACKET_SYNC_MARKER 0x35, 0x2E, 0xF8, 0x53

// Reprogramming flow control packets
extern const uint8 *UART_REPLY_PROCESSING;
extern const uint8 *UART_REPLY_RETRANSMIT;
extern const uint8 *UART_REPLY_HEARTBEAT;
extern const uint8 *UART_REPLY_READY;

// Reprogramming packet definition (ref. ICD)
typedef struct {
    struct {
        uint16 APID : 11;
        uint16 secondary : 1;
        uint16 type : 1;
        uint16 version : 3;
        uint16 sequence : 14;
        uint16 grouping : 2;
        uint16 length;
        uint32 timestamp;
        uint8 ts_sub;
        uint8 reserved;
    } header;
    uint16 crc;
    uint16 blob_len;
    uint8 blob[UART_PACKET_BLOB_MAX_LEN];
} uart_packet_t;

// Processing functions
void uart_packet_reset();
size_t uart_packet_process_data(uint8 *data, size_t len, uart_packet_t *packet);

#endif /* _uart_packets_H_ */