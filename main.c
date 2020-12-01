/*
** Filename: main.c
** Author: Ondrej
** 
** Firmware entry point
*/

#include "packets.h"
#include "dev_conf.h"
#include "spi_handler.h"

#define IDLE_THREAD_STACK 256
#define SPI_THREAD_STACK 1024
#define UART_THREAD_STACK 512
#define WD_THREAD_STACK 256

VOS_HANDLE bus_spi;
VOS_HANDLE payload_spi;
VOS_HANDLE uart;

void main()
{
    uart_context_t uart_conf;
    spislave_context_t spi0_conf;
    spislave_context_t spi1_conf;
    gpio_context_t gpio_conf;
    usbhost_context_t usb_conf;
    
    /* Kernel & IO init */
    vos_init(50, VOS_TICK_INTERVAL, VOS_NUMBER_DEVICES);
    vos_set_clock_frequency(VOS_48MHZ_CLOCK_FREQUENCY);
    vos_set_idle_thread_tcb_size(IDLE_THREAD_STACK);
    dev_conf_iomux();

    /* Watchdog init, bit 31 corresponds to 45 sec expiration */
    vos_wdt_enable(31);

    /* Driver basic configuration */
    spi0_conf.slavenumber = SPI_SLAVE_0;
    spi0_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    spi1_conf.slavenumber = SPI_SLAVE_1;
    spi1_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    uart_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    gpio_conf.port_identifier = GPIO_PORT_A;
    usb_conf.if_count = 8;
    usb_conf.ep_count = 16;
    usb_conf.xfer_count = 2;
    usb_conf.iso_xfer_count = 2;

    /* Driver init */
    uart_init(VOS_DEV_UART, &uart_conf);
    spislave_init(VOS_DEV_SPI_SLAVE_0, &spi0_conf);
    spislave_init(VOS_DEV_SPI_SLAVE_1, &spi1_conf);
    gpio_init(VOS_DEV_GPIO_PORT_A, &gpio_conf);
    // usbhost_init(VOS_DEV_USBHOST_1, -1, &usb_conf);
    vos_gpio_set_pin_mode(GPIO_A_2, 1);
    vos_gpio_set_pin_mode(GPIO_A_7, 1);
    vos_gpio_write_pin(GPIO_A_2, 0);
    vos_gpio_write_pin(GPIO_A_7, 0);

    /* Open and configure drivers */
    bus_spi = vos_dev_open(VOS_DEV_SPI_SLAVE_0);
    payload_spi = vos_dev_open(VOS_DEV_SPI_SLAVE_1);
    uart = vos_dev_open(VOS_DEV_UART);
    dev_conf_spi(bus_spi);
    dev_conf_spi(payload_spi);
    dev_conf_uart(uart);

    /* Configure priority and start threads */
    vos_create_thread_ex(20, SPI_THREAD_STACK, spi_handler_bus, "spi_bus", 0);
    vos_create_thread_ex(20, SPI_THREAD_STACK, spi_handler_payload, "spi_payload", 0);
    vos_create_thread_ex(15, WD_THREAD_STACK, spi_handler_watchdog, "spi_watchdog", 0);
    // vos_create_thread_ex(25, UART_THREAD_STACK, reprogramming, "reprogramming", 0);
    vos_start_scheduler();

    /* Never reached */
    for(;;);
}