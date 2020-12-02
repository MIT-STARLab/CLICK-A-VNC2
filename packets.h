/*
** Filename: packets.h
** Author: Ondrej
** 
** Packet definitions and helpers
*/

#ifndef _packets_H_
#define _packets_H_

#include "vos.h"

/* High-level definitions */
#define PACKET_SYNC_MARKER 0x352EF853
#define PACKET_SYNC_MARKER_LE 0x53F82E35
#define PACKET_SYNC_LEN 4
#define PACKET_HEADER_LEN 6
#define PACKET_OVERHEAD (PACKET_SYNC_LEN + PACKET_HEADER_LEN)
#define PACKET_TC_MAX_LEN 1024
#define PACKET_TM_MAX_LEN 4100
#define PACKET_IMAGE_MAX_LEN 532
#define PACKET_UART_HEADER_LEN 12
#define PACKET_UART_REPLY_LEN 12
// #define PACKET_ADD_SYNC(buf) *((uint32*) buf) = PACKET_SYNC_MARKER_LE
#define PACKET_SYNC_VALID(sync) (sync == PACKET_SYNC_MARKER)
#define PACKET_SYNC_UPDATE(sync, ptr) sync = ((sync << 8) | ((*(ptr)) & 0xFF))

/* Pre-defined UART flow control packets */
extern const uint32 *UART_REPLY_PROCESSING;
extern const uint32 *UART_REPLY_RETRANSMIT;
extern const uint32 *UART_REPLY_HEARTBEAT;
extern const uint32 *UART_REPLY_READY;

/* CCSDS header in little endian */
typedef struct {
    uint8 apid_msb : 3;
    uint8 secondary : 1;
    uint8 type : 1;
    uint8 version : 3;
    uint8 apid_lsb;
    uint8 seq_msb : 6;
    uint8 grouping : 2;
    uint8 seq_lsb;
    uint8 len_msb;
    uint8 len_lsb;
} packet_header_t;

/* Helper packet functions */
uint16 packet_process_dma(VOS_HANDLE dev, uint8 *buffer, uint16 bufsize, uint16 *offset);

#endif /* _packets_H_ */