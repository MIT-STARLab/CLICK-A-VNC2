/*
** Filename: spi_handler.c
** Author: Ondrej
** 
** Handles SPI piping logic
*/

#include "spi_handler.h"
#include "usb_handler.h"
#include "packets.h"
#include "dev_conf.h"
#include "helpers.h"

#include "uart_handler.h"

/* Private variables */
static vos_mutex_t bus_write_busy;
static vos_mutex_t payload_read_busy;
static vos_mutex_t payload_read_block;
static volatile uint8 payload_response_pending = FALSE;
static volatile uint32 payload_tx_cnt = 0;
static volatile uint8 interrupt_bit = 0;

/* Check if we received a reprogramming initialization command */
static uint8 spi_is_reprog_command(uint8 *pkt, uint16 pkt_len)
{
    uint16 apid = 0;
    packet_header_t *header = NULL;
    if (pkt_len == SPI_REPROG_PKT_LEN)
    {
        /* Read header and check APID */
        header = (packet_header_t*) (pkt + PACKET_SYNC_LEN);
        apid = (header->apid_msb << 8) | header->apid_lsb;
        return (apid == SPI_REPROG_APID);
    }
    return FALSE;
}

/* Bus to payload SPI handler */
void spi_handler_bus()
{
    uint16 pkt_len = 0;
    uint8 *pkt_start = NULL;

    /* Bus handler starts unblocked and with DMA */
    bus_spi = vos_dev_open(VOS_DEV_SPI_SLAVE_0);
    dev_conf_spi(bus_spi, SPI_SLAVE_SCK_CPOL_1, SPI_SLAVE_SCK_CPHA_1);
    dev_dma_acquire(bus_spi);
    vos_init_mutex(&bus_write_busy, VOS_MUTEX_UNLOCKED);

    for(;;)
    {
        /* Wait for packet from bus */
        if((pkt_len = packet_process_blocking(bus_spi, cmd_buffer,
            PACKET_TC_MAX_LEN, &pkt_start, SPI_NO_DATA_LIMIT)))
        {
            /* Wait for a bus write to finish on other thread (if any), then release DMA */
            vos_lock_mutex(&bus_write_busy);
            dev_dma_release(bus_spi);

            /* Run reprogramming sequence if requested */
            if (spi_is_reprog_command(pkt_start, pkt_len))
            {
                usb_run_sequence();
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
    payload_spi = vos_dev_open(VOS_DEV_SPI_SLAVE_1);
    dev_conf_spi(payload_spi, SPI_SLAVE_SCK_CPOL_0, SPI_SLAVE_SCK_CPHA_0);
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
        pkt_len = packet_process_blocking(payload_spi, tlm_buffer,
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

/* A 1 Hz watchdog for SPI
** Sends interrupts if the payload did not respond to primary SPI interrupt
** (e.g. because RPI was still booting up...) */
void spi_handler_watchdog()
{
    uint32 prev_cnt = 0, count_on_same = 0;
    uart_dbg("boot", 1, 1);
    for(;;)
    {
        vos_delay_msecs(1000);
        ENTER_CRITICAL_SECTION
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
        EXIT_CRITICAL_SECTION
    }
}
