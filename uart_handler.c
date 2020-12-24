/*
** Filename: uart_handler.c
** Author: Ondrej
** 
** Handles UART logic
*/

#include "uart_handler.h"
#include "usb_handler.h"
#include "packets.h"
#include "dev_conf.h"

#include "string.h"
#include "stdio.h"

/* Private variables */
static uint8 uart_buf[PACKET_IMAGE_MAX_LEN];

/* Pre-defined UART flow control packets (little endian) */
static const uint32 UART_REPLY_PROCESSING[] = { PACKET_SYNC_MARKER_LE, 0x1502, 0x15BB0100 };
static const uint32 UART_REPLY_RETRANSMIT[] = { PACKET_SYNC_MARKER_LE, 0x3002, 0xC6BB0100 };
static const uint32 UART_REPLY_HEARTBEAT[]  = { PACKET_SYNC_MARKER_LE, 0x2502, 0x8D5C0100 };
static const uint32 UART_REPLY_READY[]      = { PACKET_SYNC_MARKER_LE, 0x2002, 0x83E40100 };

/* Debugging print */
void uart_dbg(char *msg, uint16 number1, uint16 number2)
{
    char buf[256];
    uint16 msg_len = 0;
    packet_header_t *hdr = (packet_header_t*) (buf + PACKET_SYNC_LEN);
    vos_memset(buf, 0, 256);
    sprintf(buf + PACKET_OVERHEAD, "%s: %d 0x%X", msg, number1, number2);
    msg_len = strlen(buf + PACKET_OVERHEAD);
    PACKET_ADD_SYNC(buf);
    hdr->len_msb = (msg_len - 1) >> 8;
    hdr->len_lsb = (msg_len - 1) & 0xFF;
    vos_dev_write(uart, (uint8*) buf, msg_len + PACKET_OVERHEAD, NULL);
}

/* UART flow control reply */
static void uart_reply(const uint32 *pkt)
{
    vos_dev_write(uart, (uint8*) pkt, PACKET_UART_REPLY_LEN, NULL);
}

/* Starts the reprogramming sequence */
void uart_run_sequence()
{
    uint16 pkt_len = 0;
    uint8 *pkt_start = NULL;
    dev_usb_boot_t dev_first, dev_second;
    
    /* Wait for first blob before doing anything */
    if ((pkt_len = packet_process_blocking(uart, uart_buf, PACKET_IMAGE_MAX_LEN, &pkt_start, 1000)))
    {
        /* Signal that we are processing */
        uart_reply(UART_REPLY_PROCESSING);

        /* Drive EMMC_DISABLE low by setting Select high */
        vos_gpio_write_pin(GPIO_RPI_EMMC, 1);

        /* Reset RPi CPU
        ** Needs to be reset twice with about 2 sec delay for some reason
        ** Afterward, USB enumeration takes about 8 sec */
        vos_gpio_write_pin(GPIO_RPI_RESET, 0);
        vos_delay_msecs(1);
        vos_gpio_write_pin(GPIO_RPI_RESET, 1);
        vos_delay_msecs(2000);
        vos_gpio_write_pin(GPIO_RPI_RESET, 0);
        vos_delay_msecs(1);
        vos_gpio_write_pin(GPIO_RPI_RESET, 1);

        vos_delay_msecs(8000);

        /* Begin 1st USB stage */
        usb = vos_dev_open(VOS_DEV_USBHOST_1);
        if(dev_usb_boot_wait(0, &dev_first, 10000))
        {

        }

        vos_dev_close(usb);
        
    }
    else uart_dbg("timeout", 0, 0);
}

/* Test thread */
void uart_test()
{
    dev_dma_acquire(uart);
    for(;;)
    {
        uart_run_sequence();
    }
}
