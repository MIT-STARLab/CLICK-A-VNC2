/*
** Filename: uart_handler.h
** Author: Ondrej
** 
** Handles UART logic
*/

#ifndef _uart_handler_H_
#define _uart_handler_H_

#include "vos.h"

void uart_handler_listen(VOS_HANDLE uart);
void uart_handler_reply();

#endif /* _uart_handler_H_ */