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
void spi_uart_dbg(char *msg, uint16 number1, uint16 number2)
{
    char buf[128];
    sprintf(buf, "%s: %d 0x%X\r\n", msg, number1, number2);
    vos_dev_write(uart, (uint8*) buf, strlen(buf), NULL);
}

/* Bus to payload SPI handler */
void spi_handler_bus()
{
    uint16 packet_len = 0;
    uint8 *packet_start = NULL;
    vos_init_mutex(&bus_write_busy, VOS_MUTEX_UNLOCKED);
    dev_dma_acquire(bus_spi);

    for(;;)
    {
        /* Wait for packet from bus */
        if((packet_len = packet_process_blocking(bus_spi, bus_buf,
            PACKET_TC_MAX_LEN, &packet_start, SPI_NO_DATA_LIMIT)))
        {
            /* Wait for a bus write operation to finish on other thread (if any) */
            vos_lock_mutex(&bus_write_busy);

            /* Switch DMA to payload SPI */
            dev_dma_release(bus_spi);


            // Reprogramming check here

            dev_dma_acquire(payload_spi);

            /* Unblock payload read operation on other thread */
            vos_lock_mutex(&payload_read_busy);
            vos_unlock_mutex(&payload_read_block);

            /* Begin payload write operation */
            payload_response_pending = TRUE;
            vos_dev_write(payload_spi, packet_start, packet_len, NULL);

            /* Wait for payload read to finish on other thread */
            vos_lock_mutex(&payload_read_busy);
            vos_unlock_mutex(&payload_read_busy);

            /* Update payload flags for watchdog thread */
            payload_tx_counter++;
            payload_response_pending = FALSE;

            /* Switch DMA back to bus SPI */
            dev_dma_release(payload_spi);
            dev_dma_acquire(bus_spi);

            /* Unblock a bus write operation on other thread */
            vos_unlock_mutex(&bus_write_busy);
        }
    }
}

/* Payload to bus SPI handler */
void spi_handler_payload()
{
    uint16 packet_len = 0;
    uint8 *packet_start = NULL;
    vos_init_mutex(&payload_read_busy, VOS_MUTEX_UNLOCKED);
    vos_init_mutex(&payload_read_block, VOS_MUTEX_LOCKED);

    for(;;)
    {
        /* Wait to be unblocked by bus thread */
        vos_lock_mutex(&payload_read_block);

        /* Signal payload that we are ready */
        interrupt_bit ^= 1;
        vos_gpio_write_pin(GPIO_RPI_IRQ, interrupt_bit);

        /* Wait for packet from payload */
        packet_len = packet_process_blocking(payload_spi, payload_buf,
            PACKET_TM_MAX_LEN, &packet_start, SPI_NO_DATA_LIMIT);
        vos_unlock_mutex(&payload_read_busy);

        /* If a valid packet is received, send it to bus */
        if(packet_len)
        {
            vos_lock_mutex(&bus_write_busy);
            vos_dev_write(bus_spi, packet_start, packet_len, NULL);
            vos_unlock_mutex(&bus_write_busy);
        }
    }
}

/* The SPI watchdog sends interrupts if payload did not respond to primary interrupt
** (e.g. because RPI was still booting up...). Runs at 1 Hz. */
void spi_handler_watchdog()
{
    tmr_ioctl_cb_t tmr_iocb;

    uint32 previous_counter = 0, count_on_same = 0;
    spi_uart_dbg("[wd] running", 0, 0);
    dev_conf_timer_start(timer, 5000);
    for(;;)
    {
        spi_uart_dbg("[wd] timer status", dev_timer_status(timer), 0);
        vos_delay_msecs(1000);
        #ifndef __INTELLISENSE__
        VOS_ENTER_CRITICAL_SECTION
        #endif
        if (payload_tx_counter != previous_counter)
        {
            previous_counter = payload_tx_counter;
            count_on_same = 0;
        }
        else if(payload_response_pending && ++count_on_same > 1)
        {
            interrupt_bit ^= 1;
            vos_gpio_write_pin(GPIO_RPI_IRQ, interrupt_bit);
        }
        #ifndef __INTELLISENSE__
        VOS_EXIT_CRITICAL_SECTION
        #endif
    }
}
