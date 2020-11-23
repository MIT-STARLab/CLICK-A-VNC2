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

// Devices
static VOS_HANDLE bus_spi;
static VOS_HANDLE payload_spi;
VOS_HANDLE uart;

// Thread prototypes
static void bus_to_payload();
static void payload_to_bus();
static void reprogramming();
static void watchdog();

// Read buffers
static uint8 bus_buf[PACKET_TC_MAX_LEN];
static uint8 payload_buf[PACKET_TM_MAX_LEN];
static uint8 uart_buf[PACKET_IMAGE_MAX_LEN];

// Thread synchronization
static vos_mutex_t interrupt_lock;
static uint8 interrupt_bit = 0;
static uint32 payload_tx_counter = 0;

void main()
{
    uart_context_t uart_conf;
    spislave_context_t spi0_conf;
    spislave_context_t spi1_conf;
    gpio_context_t gpio_conf;
    usbhost_context_t usb_conf;

    // Initialize packet buffers with sync markers
    PACKET_ADD_SYNC(bus_buf);
    PACKET_ADD_SYNC(payload_buf);
    PACKET_ADD_SYNC(uart_buf);
    
    // Kernel & IO init
    vos_init(50, VOS_TICK_INTERVAL, VOS_NUMBER_DEVICES);
    vos_set_clock_frequency(VOS_48MHZ_CLOCK_FREQUENCY);
    vos_set_idle_thread_tcb_size(IDLE_THREAD_STACK);
    vos_init_mutex(&interrupt_lock, VOS_MUTEX_UNLOCKED);
    dev_conf_iomux();

    // Watchdog init, bit 31 is highest counter
    vos_wdt_enable(31);

    // Driver basic configuration
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

    // Driver init
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

    // Start threads, with increasing priority
    vos_create_thread_ex(15, WD_THREAD_STACK, watchdog, "watchdog", 0);
    vos_create_thread_ex(20, SPI_THREAD_STACK, bus_to_payload, "bus", 0);
    vos_create_thread_ex(20, SPI_THREAD_STACK, payload_to_bus, "payload", 0);
    vos_create_thread_ex(25, UART_THREAD_STACK, reprogramming, "reprogramming", 0);
    vos_start_scheduler();

    // Never reached
    for(;;);
}

// Bus SPI thread entry point
static void bus_to_payload()
{
    spi_pipe_conf_t config;
    config.src = bus_spi;
    config.dest = payload_spi;
    config.buf = bus_buf;
    config.max_data = PACKET_TC_MAX_LEN - PACKET_OVERHEAD;
    config.interrupts = TRUE;
    config.interrupt_lock = &interrupt_lock;
    config.interrupt_bit = &interrupt_bit;
    config.tx_counter = &payload_tx_counter;
    spi_handler_pipe(&config);
}

// Payload SPI thread entry point
static void payload_to_bus()
{
    spi_pipe_conf_t config;
    config.src = payload_spi;
    config.dest = bus_spi;
    config.buf = payload_buf;
    config.max_data = PACKET_TM_MAX_LEN - PACKET_OVERHEAD;
    config.interrupts = FALSE;
    config.interrupt_lock = &interrupt_lock;
    config.interrupt_bit = NULL;
    config.tx_counter = NULL;
    spi_handler_pipe(&config);
}

// Reprogramming thread entry point
static void reprogramming()
{

}

// Watchdog thread entry point
static void watchdog()
{
    uint32 previous_counter = 0, count_on_same = 0;
    for(;;)
    {
        vos_delay_msecs(1000);
        VOS_ENTER_CRITICAL_SECTION
        vos_wdt_clear();
        if (payload_tx_counter != previous_counter)
        {
            previous_counter = payload_tx_counter;
            count_on_same = 0;
        }
        else if(++count_on_same >= 2)
        {
            interrupt_bit ^= 1;
            vos_gpio_write_pin(GPIO_A_7, interrupt_bit);
        }
        VOS_EXIT_CRITICAL_SECTION
    }
}