/*
** Filename: uart_handler.h
** Author: Ondrej
** 
** Handles UART logic
*/

#ifndef _uart_handler_H_
#define _uart_handler_H_

#include "vos.h"

#define UART_TIMEOUT_MS 1000
#define UART_MAX_RETRY 3

/* Reprogramming APIDs */
#define UART_BLOB_APID 0x200
#define UART_PROC_APID_LSB 0x15
#define UART_READY_APID_LSB 0x20
#define UART_HEARTBEAT_APID_LSB 0x25
#define UART_RETRANSMIT_APID_LSB 0x30

/* UART processing data */
typedef struct {
    uint32 blob_num;
    uint16 blob_seq;
    uint16 block_len;
    uint16 data_len;
    uint16 data_offset;
} uart_proc_t;

uint8 uart_reply(uint8 apid_lsb, uint16 sequence);
uint8 uart_get_block(uart_proc_t *proc, uint32 initial_timeout_ms);
void uart_dbg(char *msg, uint16 number1, uint16 number2);

#endif /* _uart_handler_H_ */
