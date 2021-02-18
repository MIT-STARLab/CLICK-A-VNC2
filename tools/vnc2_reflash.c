/* Reflashes VNC2L program memory through UART on a Raspberry Pi
** Tested working on Raspbian 10 lite */
#ifndef __INTELLISENSE__
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <wiringPi.h>

#define RST_PIN 4
#define PROG_PIN 5
#define TIMEOUT 0x20000
#define INIT_TIMEOUT 0x200000
#define READ_TIMEOUT 1000
#define SLOW 0

int init()
{
    int uart = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(uart == -1)
    {
        printf("Could not open UART\n");
        return uart;
    }

    struct termios options;

    tcgetattr(uart, &options);
    options.c_cflag = B115200 | CS8 | CRTSCTS | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart, TCIFLUSH);
    tcsetattr(uart, TCSANOW, &options);
    
    return uart;
}

int uartDataAvail(int uart)
{
    int result = 0;

    if (ioctl(uart, FIONREAD, &result) == -1)
    {
        return -1;
    }
    return result;
}

int write_flash(int uart, unsigned char addrL, unsigned char addrH, char *data)
{
    int i = 0;
    unsigned char cmd[3] = {0x03, addrL, addrH};
    unsigned char cmd_success = 0, wr_success = 0;
    unsigned char endianData[128]; 
    
    for(i = 0; i<128; i+=2)
    {
        endianData[i] = data[i+1];
        endianData[i+1] = data[i];
    }

    i = 0;
    write(uart, cmd, 3);

    while (i < TIMEOUT)
    {
        if (uartDataAvail(uart) > 0)
        {
            read(uart, cmd, 1);
            if(cmd[0] != 0x02)
            {
                printf("\nIncorrect command response in write flash: %0X\n", cmd[0]);
                return -1;
            }
            else cmd_success = 1;
            break;
        }
        else i++;
    }

    if(cmd_success)
    {
        i = 0;
        write(uart, endianData, 128);
        while (i < TIMEOUT)
        {
            if (uartDataAvail(uart) > 0)
            {
                read(uart, cmd, 1);
                if(cmd[0] != 0x02)
                {
                    printf("\nIncorrect write response in write flash: %0X\n", cmd[0]);
                    return -1;
                }
                else wr_success = 1;
                break;
            }
            else i++;
        }

        if (!wr_success)
        {
            printf("\nData write timeout\n");
            return -1;
        }
    }
    else
    {
        printf("\nCmd write timeout\n");
        return -1;
    }

    return 0;
}

int init_flash(int uart)
{
    int i = 0;
    unsigned char success = 0;
  #ifdef SLOW
    char cmd[2] = {0x01, 0x01};
  #else
    char cmd[2] = {0x01, 0x02};
    struct termios old, new;
    tcgetattr(uart, &old);
    tcgetattr(uart, &new);
    new.c_cflag = B1000000 | CS8 | CRTSCTS | CLOCAL | CREAD;
    new.c_iflag = IGNPAR;
    new.c_oflag = 0;
    new.c_lflag = 0;
  #endif
    do
    {
        i = 0;
        write(uart, cmd, 2);
        usleep(50000);
      #ifndef SLOW
        tcsetattr(uart, TCSAFLUSH, &new);
      #endif
        while (i < INIT_TIMEOUT)
        {
            if (uartDataAvail(uart) > 0)
            {
                read(uart, cmd, 1);
                if(cmd[0] != 0x02)
                {
                    printf("Could not set flash controls\n");
                    return -1;
                }
                else success = 1;
                break;
            }
            else i++;
        }
        if (!success)
        {
            printf("Init flash cmd timeout\n");
          #ifndef SLOW
            tcsetattr(uart, TCSAFLUSH, &old);
          #endif
            usleep(50000);
        }
    } while(!success);

    return 0;
}

int sync_flash(int uart)
{
    int i = 0;
    char sync = 0xFF;
    char synced[1] = {5};
    unsigned char one = 0, two = 0;

    do
    {
        i = 0;
        write(uart, &sync, 1);
        while (i < TIMEOUT)
        {
            if (uartDataAvail(uart) > 0)
            {
                read(uart, &synced, 1);
                if(synced[0]!=0xFF)
                {
                    printf("Failed to sync: %0X\n", synced[0]);
                }
                else
                {
                    one = 1;
                    printf("Sync: %0X\n", synced[0]);
                }
                break;
            }
            else i++;
        }
        if (one)
        {
            i = 0;
            sync = 0xFB;
            write(uart, &sync, 1);
            while (i < TIMEOUT)
            {
                if (uartDataAvail(uart) > 0) 
                {
                    read(uart, &synced, 1);
                    if(synced[0] != 0xFB)
                    {
                        printf("Failed to sync: %0X\n", synced[0]);
                    }
                    else
                    {
                        two = 1;
                        printf("Sync: %0X\n", synced[0]);
                    }
                    break;
                }
                else i++;
            }
            if (!two)
            {
                printf("Sync 0xFB timeout\n");
                one = 0;
            }
        }
        else printf("Sync 0xFF timeout\n");
    } while (!two);
    return 0;
}

int read_flash(int uart, unsigned char addrL, unsigned char addrH, unsigned char *data)
{
    int i = 0, avail = 0;
    unsigned char endianData[128];
    unsigned char cmd[3] = {0x02, addrL, addrH};
    unsigned char rd_success = 0;

    write(uart, cmd, 3);
    while (i < READ_TIMEOUT)
    {
        usleep(10);
        avail = uartDataAvail(uart);
        if (avail == 1)
        {
            read(uart, cmd, 1);
            if(cmd[0] == 0xFF)
            {
                printf("\nFlash read error %0X\n", cmd[0]);
                return -1;
            }
            avail = uartDataAvail(uart);
        }
        if (avail == 129)
        {
            read(uart, cmd, 1);
            if(cmd[0] != 0x02)
            {
                printf("\nIncorrect command response in read flash: %0X\n", cmd[0]);
                return -1;
            }
            else
            {
                read(uart, &endianData, 128);
                rd_success = 1;
                break;
            }
        }
        else i++;
    }

    if(!rd_success)
    {
        printf("\nFlash read timeout\n");
        return -1;
    }
    
    for(i = 0; i < 128; i+=2)
    {
        data[i] = endianData[i+1];
        data[i+1] = endianData[i];
    }

    return 0;
}

void reset_vnc()
{
    digitalWrite(RST_PIN, HIGH);
    digitalWrite(RST_PIN, LOW);
    usleep(50000);
    digitalWrite(RST_PIN, HIGH);
    usleep(100000);
}

int main()
{
    FILE *rom;
    int uart = 0;
    unsigned int addr = 0, result = 0;
    unsigned char addrL = 0, addrH = 0; 
    char data[128], readback[129];

    rom = fopen("CLICK.rom", "rb");
    if (rom == NULL) return -1;
    if (fread(data, sizeof(char), 128, rom) != 128) return -1;

    wiringPiSetup();
    uart = init();
    if (uart < 0) return -1;
    
    pinMode(RST_PIN, OUTPUT);
    pinMode(PROG_PIN, OUTPUT);
    digitalWrite(PROG_PIN, LOW);
    reset_vnc();
    
    printf("Syncing...\n");
    if(sync_flash(uart) == -1) return -1;
    printf("Synced!\n");
    if(init_flash(uart) == -1) return -1;
    printf("Baud rate set!\n");

    for(;;)
    {
        printf("\r0x%0X ", ((addrH)<<8)+addrL);
        fflush(stdout);
        result = write_flash(uart, addrL, addrH, data);
        if (result == 0) result = read_flash(uart, addrL, addrH, readback);
        if (result == 0)
        {
            for(int i = 0; i < 128; i++)
            {
                if(data[i] != readback[i])
                {
                    printf("Bad flash write/readback\n");
                    printf("%0X\n", (addrH<<8)+addrL+i);
                    printf("%0X\n", readback[i]);
                    printf("%0X\n", data[i]);
                    return -1;
                }
            }
            if (fread(data, sizeof(char), 128, rom) > 0)
            {
                addrL += 1;
                if(addrL==0)
                {
                    addrH += 1;
                }
            }
            else break;
        }
        else
        {
            addrL = addrH = 0; 
            close(uart);
            uart = init();
            reset_vnc();
            printf("Syncing...\n");
            if(sync_flash(uart) == -1) return -1;
            printf("Synced!\n");
            if(init_flash(uart) == -1) return -1;
            printf("Baud rate set!\n");
        }
    }

    digitalWrite(PROG_PIN, HIGH);
    digitalWrite(RST_PIN, LOW);
    usleep(50000);
    digitalWrite(RST_PIN, HIGH);
    
    printf("\nSuccess\n");
    fclose(rom); 
    close(uart);

    return 0;
}

#endif /* __INTELLISENSE__ */
