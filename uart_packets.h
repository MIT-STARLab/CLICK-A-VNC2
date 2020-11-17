/*
** Filename: uart_packets.h
** Author: Ondrej
** 
** Handles UART packets
*/

#ifndef _uart_packets_H_
#define _uart_packets_H_

#define UART_PACKET_HEADER_LEN 
#define UART_PACKET_BLOB_MAX_LEN 528
#define UART_PACKET_SYNC_MARKER 0x35, 0x2E, 0xF8, 0x53

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
    } header;
    unsigned short crc;
    unsigned char blob[UART_PACKET_BLOB_MAX_LEN];
    unsigned int currentLength;
} uart_packet_t;

// Reprogramming flow control packets
const unsigned char UART_REPLY_PROCESSING[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x15, 0, 0, 0, 1, 0xBB, 0x15};
const unsigned char UART_REPLY_RETRANSMIT[] = {UART_PACKET_SYNC_MARKER, 0x02, 0x30, 0, 0, 0, 1, 0xBB, 0xC6};
const unsigned char UART_REPLY_HEARTBEAT[]  = {UART_PACKET_SYNC_MARKER, 0x02, 0x25, 0, 0, 0, 1, 0x5C, 0x8D};
const unsigned char UART_REPLY_READY[]      = {UART_PACKET_SYNC_MARKER, 0x02, 0x20, 0, 0, 0, 1, 0xE4, 0x83};

// Processing functions
void uart_packet_reset();
uart_packet_status_t uart_packet_process(unsigned char *data, unsigned int len, uart_packet_t *packet);

#endif /* _uart_packets_H_ */