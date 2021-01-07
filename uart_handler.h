/*
** Filename: uart_handler.h
** Author: Ondrej
** 
** Handles UART logic
*/

#ifndef _uart_handler_H_
#define _uart_handler_H_

#include "vos.h"

#define UART_TIMEOUT_MS 250
#define UART_MAX_RETRY 2

/* Reprogramming APIDs */
#define UART_BLOB_APID 0x200
#define UART_PROC_APID_LSB 0x15
#define UART_READY_APID_LSB 0x20
#define UART_HEARTBEAT_APID_LSB 0x25
#define UART_RETRANSMIT_APID_LSB 0x30

/* UART reply CRCs */
#define UART_PROC_CRC 0xB27C
#define UART_READY_CRC 0x9DC5
#define UART_HEARTBEAT_CRC 0xBE92
#define UART_RETRANSMIT_CRC 0x999F

uint8 uart_reply(uint8 apid_lsb, uint16 sequence, uint16 crc);

void uart_dbg(char *msg, uint16 number1, uint16 number2);
void uart_test();

#endif /* _uart_handler_H_ */
