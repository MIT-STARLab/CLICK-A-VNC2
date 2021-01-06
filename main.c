/*
** Filename: main.c
** Author: Ondrej
** 
** Firmware entry point
*/

#include "packets.h"
#include "dev_conf.h"
#include "spi_handler.h"
#include "uart_handler.h"
#include "crc.h"

#define IDLE_THREAD_STACK 512
#define BUS_THREAD_STACK 4096
#define PAYLOAD_THREAD_STACK 1024
#define HELPER_THREAD_STACK 512

void main()
{
    uart_context_t uart_conf;
    spislave_context_t spi0_conf;
    spislave_context_t spi1_conf;
    usbhost_context_t usb_conf;
    
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
    uart_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    usb_conf.if_count = 8;
    usb_conf.ep_count = 16;
    usb_conf.xfer_count = 4;
    usb_conf.iso_xfer_count = 4;

    /* Driver init */
    uart_init(VOS_DEV_UART, &uart_conf);
    spislave_init(VOS_DEV_SPI_SLAVE_0, &spi0_conf);
    spislave_init(VOS_DEV_SPI_SLAVE_1, &spi1_conf);
    usbhost_init(VOS_DEV_USBHOST_1, -1, &usb_conf);

    /* Configure EMMC and interrupt GPIO as output and low */
    vos_gpio_set_pin_mode(GPIO_RPI_IRQ, 1);
    vos_gpio_set_pin_mode(GPIO_RPI_EMMC, 1);
    vos_gpio_write_pin(GPIO_RPI_IRQ, 0);
    vos_gpio_write_pin(GPIO_RPI_EMMC, 0);

    /* Configure priority and start threads */
    // vos_create_thread_ex(20, BUS_THREAD_STACK, spi_handler_bus, "bus", 0);
    // vos_create_thread_ex(15, PAYLOAD_THREAD_STACK, spi_handler_payload, "payload", 0);
    // vos_create_thread_ex(10, HELPER_THREAD_STACK, spi_handler_watchdog, "wd", 0);
    vos_create_thread_ex(15, BUS_THREAD_STACK, uart_test, "ut", 0);
    vos_start_scheduler();

    /* Never reached */
    for(;;);
}
