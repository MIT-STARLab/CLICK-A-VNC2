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
        union {
            unsigned short raw;
            struct {
                unsigned short APID : 11;
                unsigned short secondary_header : 1;
                unsigned short type : 1;
                unsigned short version : 3;
            } field;
        } preamble;
        union {
            unsigned short raw;
            struct {
                unsigned short count : 14;
                unsigned short grouping : 2;
            } field;
        } sequence;
        unsigned short length;
        unsigned int timestamp;
        unsigned char subsecond;
        unsigned char reserved;
    } header;
    unsigned short crc;
    unsigned int blob_len;
    unsigned char blob[UART_PACKET_BLOB_MAX_LEN];
} uart_packet_t;

// Processing functions
void uart_packet_reset();
int uart_packet_verify_header();
int uart_packet_process_data(unsigned char *data, unsigned int len, uart_packet_t *ptr);
void uart_packet_update_values(unsigned char *data, unsigned int len);

#endif /* _uart_packets_H_ */