/*
** Filename: crc.h
** Author: Ondrej
** 
** CRC functions
*/

#ifndef _crc_H_
#define _crc_H_

unsigned short crc_16_update(unsigned short crc, unsigned char *data, short len);
unsigned short crc_16_finalize(unsigned short crc);

#endif /* _crc_H_ */