/*
** Filename: packets.h
** Author: Ondrej
** 
** Packet definitions
*/

#ifndef _packets_H_
#define _packets_H_

#include "vos.h"

// ICD definitions
#define PACKET_SYNC_MARKER 0x352EF853
#define PACKET_HEADER_LEN 6
#define PACKET_TC_MAX_LEN 1024
#define PACKET_TM_MAX_LEN 4100
#define PACKET_IMAGE_MAX_LEN 532
#define PACKET_UART_HEADER_LEN 12
#define PACKET_UART_REPLY_LEN 12

// Reprogramming flow control packets
extern const uint8 *UART_REPLY_PROCESSING;
extern const uint8 *UART_REPLY_RETRANSMIT;
extern const uint8 *UART_REPLY_HEARTBEAT;
extern const uint8 *UART_REPLY_READY;

// CCSDS header
typedef struct {
    uint16 APID : 11;
    uint16 secondary : 1;
    uint16 type : 1;
    uint16 version : 3;
    uint16 sequence : 14;
    uint16 grouping : 2;
    uint16 len;
} packet_header_t;

#endif /* _packets_H_ */