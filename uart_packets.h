/*
** Filename: uart_packets.h
** Author: Ondrej
** 
** Handles UART packets
*/

#ifndef _uart_packets_H_
#define _uart_packets_H_

#include "string.h"

// Basic definitions
#define UART_PACKET_HEADER_LEN 12
#define UART_PACKET_BLOB_MAX_LEN 524
#define UART_PACKET_SYNC_MARKER 0x35, 0x2E, 0xF8, 0x53

// Reprogramming flow control packets
extern const unsigned char *UART_REPLY_PROCESSING;
extern const unsigned char *UART_REPLY_RETRANSMIT;
extern const unsigned char *UART_REPLY_HEARTBEAT;
extern const unsigned char *UART_REPLY_READY;

// Packet parsing status
typedef enum {
    UART_PACKET_WAIT_DATA,
    UART_PACKET_SUCCESS,
    UART_PACKET_ERROR
} uart_packet_status_t;

// Reprogramming packet definition (ref. ICD)
typedef struct {
    struct {
        unsigned short APID : 11;
        unsigned short secondary_header : 1;
        unsigned short type : 1;
        unsigned short version : 3;
        unsigned short sequence : 14;
        unsigned short grouping : 2;
        unsigned short length;
        unsigned short ts_msb;
        unsigned short ts_lsb;
        unsigned char ts_sub;
        unsigned char reserved;
    } header;
    unsigned short crc;
    unsigned short blob_len;
    unsigned char blob[UART_PACKET_BLOB_MAX_LEN];
} uart_packet_t;

// Processing functions
void uart_packet_reset();
int uart_packet_verify_header(uart_packet_t *packet);
int uart_packet_process_data(unsigned char *data, size_t len, uart_packet_t *packet);
void uart_packet_update_values(unsigned char *data, size_t len);

#endif /* _uart_packets_H_ */