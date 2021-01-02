/*
** Filename: crc.h
** Author: Ondrej
** 
** CRC functions
*/

#ifndef _crc_H_
#define _crc_H_

#include "vos.h"

void crc_16_load_table();
uint16 crc_16_update(uint16 crc, uint8 *data, uint16 len);

#endif /* _crc_H_ */