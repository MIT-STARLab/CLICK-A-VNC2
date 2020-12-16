/*
** Filename: spi_handler.c
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#include "spi_handler.h"
#include "uart_handler.h"
#include "packets.h"
#include "dev_conf.h"
#include "crc.h"
#include "string.h"
#include "stdio.h"

/* Private variables */
static uint8 bus_buf[PACKET_TC_MAX_LEN];
static uint8 payload_buf[PACKET_TM_MAX_LEN];
static vos_mutex_t bus_write_busy;
static vos_mutex_t payload_read_busy;
static vos_mutex_t payload_read_block;
static volatile uint8 payload_response_pending = FALSE;
static volatile uint32 payload_tx_cnt = 0;
static volatile uint8 interrupt_bit = 0;

/* Check if we received a reprogramming initialization command */
static uint8 spi_is_reprog_command(uint8 *pkt, uint16 pkt_len)
{
    packet_header_t *header = NULL;
    uint16 apid = 0, crc_pkt = 0, crc_calc = 0;
    if (pkt_len == SPI_REPROG_PKT_LEN)
    {
        /* Read header and check APID */
        pkt += PACKET_SYNC_LEN;
        pkt_len -= PACKET_SYNC_LEN;
        header = (packet_header_t*) pkt;
        apid = (header->apid_msb << 8) | header->apid_lsb;
        if (apid == SPI_REPROG_APID)
        {
            /* Check CRC */
            crc_calc = crc_16_update(0xFFFF, pkt, pkt_len - 2);
            pkt += PACKET_HEADER_LEN;
            crc_pkt = (pkt[0] << 8) | pkt[1];
            if (crc_pkt == crc_calc)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* Bus to payload SPI handler */
void spi_handler_bus()
{
    uint16 pkt_len = 0;
    uint8 *pkt_start = NULL;

    /* Bus handler starts unblocked and with DMA */
    vos_init_mutex(&bus_write_busy, VOS_MUTEX_UNLOCKED);
    dev_dma_acquire(bus_spi);

    for(;;)
    {
        /* Wait for packet from bus */
        if((pkt_len = packet_process_blocking(bus_spi, bus_buf,
            PACKET_TC_MAX_LEN, &pkt_start, SPI_NO_DATA_LIMIT)))
        {
            /* Wait for a bus write to finish on other thread (if any), then release DMA */
            vos_lock_mutex(&bus_write_busy);
            dev_dma_release(bus_spi);

            /* Initialize and run reprogramming sequence if requested */
            if (spi_is_reprog_command(pkt_start, pkt_len))
            {
                dev_dma_acquire(uart);
                uart_run_sequence();
                dev_dma_release(uart);
            }

            /* Otherwise start a regular read-write on payload SPI */
            else
            {
                /* Acquire DMA and unblock payload read operation on other thread */
                dev_dma_acquire(payload_spi);
                vos_lock_mutex(&payload_read_busy);
                vos_unlock_mutex(&payload_read_block);

                /* Begin payload write operation */
                payload_response_pending = TRUE;
                vos_dev_write(payload_spi, pkt_start, pkt_len, NULL);

                /* Wait for payload read to finish, then release DMA */
                vos_lock_mutex(&payload_read_busy);
                vos_unlock_mutex(&payload_read_busy);
                dev_dma_release(payload_spi);

                /* Update payload flags for watchdog thread */
                payload_tx_cnt++;
                payload_response_pending = FALSE;
            }

            /* Switch DMA back to bus and unblock a write on other thread */
            dev_dma_acquire(bus_spi);
            vos_unlock_mutex(&bus_write_busy);
        }
    }
}

/* Payload to bus SPI handler */
void spi_handler_payload()
{
    uint16 pkt_len = 0;
    uint8 *pkt_start = NULL;

    /* Payload handler starts blocked without DMA */
    vos_init_mutex(&payload_read_block, VOS_MUTEX_LOCKED);
    vos_init_mutex(&payload_read_busy, VOS_MUTEX_UNLOCKED);

    for(;;)
    {
        /* Wait to be unblocked by bus thread */
        vos_lock_mutex(&payload_read_block);

        /* Signal payload that we are ready */
        interrupt_bit ^= 1;
        vos_gpio_write_pin(GPIO_RPI_IRQ, interrupt_bit);

        /* Wait for packet from payload */
        pkt_len = packet_process_blocking(payload_spi, payload_buf,
            PACKET_TM_MAX_LEN, &pkt_start, SPI_NO_DATA_LIMIT);
        vos_unlock_mutex(&payload_read_busy);

        /* If a valid packet is received, send it to the bus */
        if(pkt_len)
        {
            vos_lock_mutex(&bus_write_busy);
            vos_dev_write(bus_spi, pkt_start, pkt_len, NULL);
            vos_unlock_mutex(&bus_write_busy);
        }
    }
}

/* The SPI watchdog sends interrupts if the payload did not respond to primary interrupt
** (e.g. because RPI was still booting up...). Runs at 1 Hz. */
void spi_handler_watchdog()
{
    uint32 prev_cnt = 0, count_on_same = 0;
    dev_timer_start(timer_wd, 1000);
    for(;;)
    {
        dev_timer_wait(timer_wd);
        #ifndef __INTELLISENSE__
        VOS_ENTER_CRITICAL_SECTION
        #endif
        if (payload_tx_cnt != prev_cnt)
        {
            prev_cnt = payload_tx_cnt;
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
