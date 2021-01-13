/*
** Filename: main.c
** Author: Ondrej
** 
** Firmware entry point
*/

#include "dev_conf.h"
#include "spi_handler.h"
#include "crc.h"

#define IDLE_THREAD_STACK 512
#define BUS_THREAD_STACK 2048
#define PAYLOAD_THREAD_STACK 1024
#define HELPER_THREAD_STACK 512

void main()
{
    spislave_context_t spi0_conf;
    spislave_context_t spi1_conf;
    
    /* Kernel & basic init */
    vos_init(50, VOS_TICK_INTERVAL, VOS_NUMBER_DEVICES);
    vos_set_clock_frequency(VOS_48MHZ_CLOCK_FREQUENCY);
    vos_set_idle_thread_tcb_size(IDLE_THREAD_STACK);
    dev_conf_iomux();
    crc_16_load_table();

    /* Driver basic configuration */
    spi0_conf.slavenumber = SPI_SLAVE_0;
    spi0_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    spi1_conf.slavenumber = SPI_SLAVE_1;
    spi1_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;

    /* Driver init */
    spislave_init(VOS_DEV_SPI_SLAVE_0, &spi0_conf);
    spislave_init(VOS_DEV_SPI_SLAVE_1, &spi1_conf);

    /* Configure Select and interrupt GPIO as output and low on boot */
    vos_gpio_set_pin_mode(GPIO_RPI_IRQ, 1);
    vos_gpio_set_pin_mode(GPIO_RPI_EMMC, 1);
    vos_gpio_write_pin(GPIO_RPI_IRQ, 0);
    vos_gpio_write_pin(GPIO_RPI_EMMC, 0);

    /* Configure priority and start threads */
    vos_create_thread(20, BUS_THREAD_STACK, spi_handler_bus, 0);
    vos_create_thread(15, PAYLOAD_THREAD_STACK, spi_handler_payload, 0);
    vos_create_thread(10, HELPER_THREAD_STACK, spi_handler_watchdog, 0);
    vos_start_scheduler();

    /* Never reached */
    for(;;);
}
