/*
** Filename: crc.c
** Author: Ondrej
** 
** CRC functions
*/

// Ref: BCT XB1 User Guide pg. 66
unsigned short crc_16_update(unsigned short crc, unsigned char *data, short len)
{
    unsigned short byte = 0;
    unsigned char bit = 8, msb = 0;
    while (byte < len)
    {
        msb = crc >> 15;
        crc <<= 1;
        crc |= (data[byte] >> --bit) & 1;
        if (msb) crc ^= 0x8005;
        if (bit < 1)
        {
            bit = 8;
            byte++;
        }
    }
    return crc;
}

// Ref: BCT XB1 User Guide pg. 66
unsigned short crc_16_finalize(unsigned short crc)
{
    unsigned char push[] = {0, 0};
    return crc_16_update(crc, push, 2);
}