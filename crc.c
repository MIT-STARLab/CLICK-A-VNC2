/*
** Filename: crc.c
** Author: Ondrej
** 
** CRC functions
*/

// Ref: BCT XB1 User Guide pg. 66
unsigned short crc_16(unsigned short crc, unsigned char *data, unsigned int len)
{
    unsigned int byte = 0;
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
