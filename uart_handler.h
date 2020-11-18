/*
** Filename: uart_handler.h
** Author: Ondrej
** 
** Handles low-level UART logic with bus interface
*/

#ifndef _uart_handler_H_
#define _uart_handler_H_

void uart_handler_init();
void uart_handler_listen();
void uart_handler_reply();

#endif /* _uart_handler_H_ */