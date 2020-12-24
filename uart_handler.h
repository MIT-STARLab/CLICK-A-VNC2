/*
** Filename: uart_handler.h
** Author: Ondrej
** 
** Handles UART logic
*/

#ifndef _uart_handler_H_
#define _uart_handler_H_

#include "vos.h"

#define UART_REPROG_APID 0x200
#define UART_TIMEOUT_MS 250
#define UART_MAX_RETRY 2

void uart_dbg(char *msg, uint16 number1, uint16 number2);
void uart_run_sequence();

void uart_test();

#endif /* _uart_handler_H_ */
