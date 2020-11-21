/*
** Filename: click.c
** Author: Ondrej
** 
** Firmware entry point
*/

#include "dev_conf.h"

void spi_thread();
void uart_thread();

void main()
{
    uart_context_t uartContext;
    spislave_context_t spisContext0;
    spislave_context_t spisContext1;
    gpio_context_t gpioContextA;
    usbhost_context_t usbhostContext;
    
    // Kernel & IO initialization
    vos_init(50, VOS_TICK_INTERVAL, VOS_NUMBER_DEVICES);
    vos_set_clock_frequency(VOS_48MHZ_CLOCK_FREQUENCY);
    vos_set_idle_thread_tcb_size(512);
    dev_conf_iomux();

    // Driver basic configuration
    uartContext.buffer_size = VOS_BUFFER_SIZE_128_BYTES;
    spisContext0.buffer_size = VOS_BUFFER_SIZE_128_BYTES;
    spisContext1.buffer_size = VOS_BUFFER_SIZE_128_BYTES;
    gpioContextA.port_identifier = GPIO_PORT_A;
    usbhostContext.if_count = 8;
    usbhostContext.ep_count = 16;
    usbhostContext.xfer_count = 2;
    usbhostContext.iso_xfer_count = 2;

    // Driver initialization
    uart_init(VOS_DEV_UART, &uartContext);
    spislave_init(VOS_DEV_SPI_SLAVE_0, &spisContext0);
    spislave_init(VOS_DEV_SPI_SLAVE_1, &spisContext1);
    gpio_init(VOS_DEV_GPIO_PORT_A, &gpioContextA);
    usbhost_init(VOS_DEV_USBHOST_1, -1, &usbhostContext);

    // Start threads
    vos_create_thread_ex(20, 4096, spi_thread, "spi", 0);
    vos_create_thread_ex(24, 4096, uart_thread, "uart", 0);
    vos_start_scheduler();

    // Never reached
    for(;;);
}

void spi_thread()
{
    /* Thread code to be added here */
}

void uart_thread()
{
    /* Thread code to be added here */
    uart_handler_init();
    uart_handler_listen();
}
