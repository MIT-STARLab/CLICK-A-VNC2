/*
** Filename: click.c
** Author: Ondrej
** 
** Firmware entry point
*/

#include "packets.h"
#include "dev_conf.h"
#include "spi_handler.h"

// Devices
static VOS_HANDLE bus_spi;
static VOS_HANDLE payload_spi;
static VOS_HANDLE uart;

// Thread prototypes
static void bus_read();
static void bus_write();
static void payload_read();
static void payload_write();
static void uart_handler();

// Read and write buffers
static uint8 bus_read_buf[PACKET_TC_MAX_LEN];
static uint8 bus_write_buf[PACKET_TM_MAX_LEN];
static uint8 payload_read_buf[PACKET_TM_MAX_LEN];
static uint8 payload_write_buf[PACKET_TC_MAX_LEN];
static uint8 uart_read_buf[PACKET_IMAGE_MAX_LEN];

// Thread synchronization
static vos_mutex_t bus_read_lock;
static vos_mutex_t bus_write_lock;
static vos_mutex_t payload_read_lock;
static vos_mutex_t payload_write_lock;

void main()
{
    uart_context_t uart_conf;
    spislave_context_t spi0_conf;
    spislave_context_t spi1_conf;
    gpio_context_t gpio_conf;
    usbhost_context_t usb_conf;
    
    // Kernel & IO initialization
    vos_init(50, VOS_TICK_INTERVAL, VOS_NUMBER_DEVICES);
    vos_set_clock_frequency(VOS_48MHZ_CLOCK_FREQUENCY);
    vos_set_idle_thread_tcb_size(256);
    vos_init_mutex(&bus_read_lock, 0);
    vos_init_mutex(&bus_write_lock, 1);
    vos_init_mutex(&payload_read_lock, 0);
    vos_init_mutex(&payload_write_lock, 1);
    dev_conf_iomux();

    // Driver basic configuration
    uart_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    spi0_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    spi1_conf.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
    gpio_conf.port_identifier = GPIO_PORT_A;
    usb_conf.if_count = 8;
    usb_conf.ep_count = 16;
    usb_conf.xfer_count = 2;
    usb_conf.iso_xfer_count = 2;

    // Driver initialization
    uart_init(VOS_DEV_UART, &uart_conf);
    spislave_init(VOS_DEV_SPI_SLAVE_0, &spi0_conf);
    spislave_init(VOS_DEV_SPI_SLAVE_1, &spi1_conf);
    gpio_init(VOS_DEV_GPIO_PORT_A, &gpio_conf);
    // usbhost_init(VOS_DEV_USBHOST_1, -1, &usb_conf);
    vos_gpio_set_pin_mode(GPIO_A_2, 1);
    vos_gpio_set_pin_mode(GPIO_A_7, 1);
    vos_gpio_write_pin(GPIO_A_7, 1);

    // Open and configure drivers
    bus_spi = vos_dev_open(VOS_DEV_SPI_SLAVE_0);
    payload_spi = vos_dev_open(VOS_DEV_SPI_SLAVE_1);
    uart = vos_dev_open(VOS_DEV_UART);
    dev_conf_spi(bus_spi);
    dev_conf_spi(payload_spi);
    dev_conf_uart(uart);

    // Start threads
    vos_create_thread_ex(20, 384, bus_read, "bus_read", 0);
    // vos_create_thread_ex(20, 384, bus_write, "bus_write", 0);
    // vos_create_thread_ex(20, 384, payload_read, "payload_read", 0);
    // vos_create_thread_ex(20, 1024, payload_write, "payload_write", 0);
    vos_start_scheduler();

    // Never reached
    for(;;);
}

void bus_read()
{
    spi_read_handler(bus_spi,
                     bus_read_buf,
                     payload_write_buf,
                     PACKET_TC_MAX_LEN - PACKET_OVERHEAD,
                     &bus_read_lock,
                     &bus_write_lock,
                     TRUE);
}

void bus_write()
{
    spi_write_handler(bus_spi,
                      bus_write_buf,
                      PACKET_TM_MAX_LEN - PACKET_OVERHEAD,
                      &payload_read_lock,
                      &payload_write_lock);
}

void payload_read()
{
    spi_read_handler(payload_spi,
                     payload_read_buf,
                     bus_write_buf,
                     PACKET_TM_MAX_LEN - PACKET_OVERHEAD,
                     &payload_read_lock,
                     &payload_write_lock,
                     FALSE);
}

void payload_write()
{
    spi_write_handler(payload_spi,
                      payload_write_buf,
                      PACKET_TC_MAX_LEN - PACKET_OVERHEAD,
                      &bus_read_lock,
                      &bus_write_lock);
}
