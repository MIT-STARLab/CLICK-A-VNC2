/*
** Filename: spi_handler.c
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#include "spi_handler.h"
#include "packets.h"
#include "dev_conf.h"
#include "string.h"
#include "stdio.h"

/* Private variables */
static uint8 bus_buf[PACKET_TC_MAX_LEN];
static uint8 payload_buf[PACKET_TM_MAX_LEN];
static vos_mutex_t bus_write_busy;
static vos_mutex_t payload_read_busy;
static vos_mutex_t payload_read_block;
static volatile uint8 payload_response_pending = FALSE;
static volatile uint32 payload_tx_counter = 0;
static volatile uint8 interrupt_bit = 0;

/* Debugging print */
static void spi_uart_dbg(char *msg, uint16 number)
{
    char buf[128];
    sprintf(buf, "%s: %d 0x%X\r\n", msg, number, number);
    vos_dev_write(uart, (uint8*) buf, strlen(buf), NULL);
}

/* Bus to payload SPI handler */
void spi_handler_bus()
{
    uint16 packet_len = 0, crc = 0;
    PACKET_ADD_SYNC(bus_buf);
    vos_init_mutex(&bus_write_busy, VOS_MUTEX_UNLOCKED);
    dev_dma_acquire(bus_spi);

    for(;;)
    {
        /* Wait for packet from bus */
        if((packet_len = packet_process_dma(bus_spi, bus_buf, PACKET_TC_MAX_LEN)))
        {
            crc = (bus_buf[packet_len - 2] << 8) | bus_buf[packet_len - 1];
            spi_uart_dbg("[bus] packet with crc", crc);

            /* Wait for a bus write operation finish on other thread (if any) */
            vos_lock_mutex(&bus_write_busy);

            /* Switch DMA to payload SPI */
            dev_dma_release(bus_spi);
            dev_dma_acquire(payload_spi);

            /* Unblock a payload read operation on other thread */
            vos_unlock_mutex(&payload_read_block);
            vos_unlock_mutex(&payload_read_busy);

            /* Begin payload write operation */
            payload_response_pending = TRUE;
            vos_dev_write(payload_spi, bus_buf, packet_len, NULL);

            /* Wait for payload read to finish on other thread */
            vos_lock_mutex(&payload_read_busy);

            /* Update payload flags for watchdog thread */
            payload_tx_counter++;
            payload_response_pending = FALSE;

            /* Switch DMA back to bus SPI */
            dev_dma_release(payload_spi);
            dev_dma_acquire(bus_spi);

            /* Unblock a bus write operation on other thread */
            vos_unlock_mutex(&bus_write_busy);

            spi_uart_dbg("[bus] payload write success", packet_len);
        }
    }
}

/* Payload to bus SPI handler */
void spi_handler_payload()
{
    uint16 packet_len = 0, crc = 0;
    PACKET_ADD_SYNC(payload_buf);
    vos_init_mutex(&payload_read_busy, VOS_MUTEX_UNLOCKED);
    vos_init_mutex(&payload_read_block, VOS_MUTEX_LOCKED);

    for(;;)
    {
        /* Wait to be unblocked by bus thread */
        vos_lock_mutex(&payload_read_block);
        vos_lock_mutex(&payload_read_busy);

        /* Signal payload that we are ready */
        interrupt_bit ^= 1;
        vos_gpio_write_pin(GPIO_A_2, interrupt_bit);

        /* Wait for packet from payload */
        packet_len = packet_process_dma(payload_spi, payload_buf, PACKET_TM_MAX_LEN);
        vos_unlock_mutex(&payload_read_busy);

        /* If a valid packet is received, send it to bus */
        if(packet_len)
        {
            crc = (payload_buf[packet_len - 2] << 8) | payload_buf[packet_len - 1];
            spi_uart_dbg("[payload] packet with crc", crc);

            vos_lock_mutex(&bus_write_busy);
            vos_dev_write(bus_spi, payload_buf, packet_len, NULL);
            vos_unlock_mutex(&bus_write_busy);

            spi_uart_dbg("[payload] bus write success", packet_len);
        }
    }
}

/* The SPI watchdog sends interrupts if payload did respond to primary interrupt
** (e.g. because RPI was still booting up...)
** Runs at 1 Hz, and also clears the VNC2 internal watchdog counter */
void spi_handler_watchdog()
{
    uint32 previous_counter = 0, count_on_same = 0;
    spi_uart_dbg("[wd] running", 1);
    for(;;)
    {
        vos_delay_msecs(1000);
        #ifndef __INTELLISENSE__
        VOS_ENTER_CRITICAL_SECTION
        #endif
        vos_wdt_clear();
        if (payload_tx_counter != previous_counter)
        {
            previous_counter = payload_tx_counter;
            count_on_same = 0;
        }
        else if(payload_response_pending && ++count_on_same > 1)
        {
            interrupt_bit ^= 1;
            vos_gpio_write_pin(GPIO_A_2, interrupt_bit);
        }
        #ifndef __INTELLISENSE__
        VOS_EXIT_CRITICAL_SECTION
        #endif
    }
}