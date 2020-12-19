/*
** Filename: dev_conf.c
** Author: Ondrej
** 
** Handle device configuration
*/

#include "dev_conf.h"

/* Configure IOMUX connections */
void dev_conf_iomux()
{
    vos_iomux_define_input(15, IOMUX_IN_SPI_SLAVE_0_CLK);
    vos_iomux_define_input(16, IOMUX_IN_SPI_SLAVE_0_MOSI);
    vos_iomux_define_output(18, IOMUX_OUT_SPI_SLAVE_0_MISO);
    vos_iomux_define_input(19, IOMUX_IN_SPI_SLAVE_0_CS);
    vos_iomux_define_input(20, IOMUX_IN_SPI_SLAVE_1_CLK);
    vos_iomux_define_input(21, IOMUX_IN_SPI_SLAVE_1_MOSI);
    vos_iomux_define_output(22, IOMUX_OUT_SPI_SLAVE_1_MISO);
    vos_iomux_define_input(23, IOMUX_IN_SPI_SLAVE_1_CS);
    vos_iomux_define_output(31, IOMUX_OUT_UART_TXD);
    vos_iomux_define_input(32, IOMUX_IN_UART_RXD);
    vos_iomux_define_output(33, IOMUX_OUT_GPIO_PORT_A_2);
    vos_iomux_define_output(34, IOMUX_OUT_GPIO_PORT_A_7);
    vos_iomux_define_output(45, IOMUX_OUT_GPIO_PORT_A_4);
}

/* Configure SPI slave */
void dev_conf_spi(VOS_HANDLE spi, uint8 polarity, uint8 phase)
{
    common_ioctl_cb_t spi_iocb;

    spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_MODE;
    spi_iocb.set.param = SPI_SLAVE_MODE_UNMANAGED;
    vos_dev_ioctl(spi, &spi_iocb);

    spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPHA;
	spi_iocb.set.param = phase;
	vos_dev_ioctl(spi, &spi_iocb);

	spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPOL;
	spi_iocb.set.param = polarity;
	vos_dev_ioctl(spi, &spi_iocb);

    spi_iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
    spi_iocb.set.param = DMA_ACQUIRE_AS_REQUIRED;
    vos_dev_ioctl(spi, &spi_iocb);
}

/* Configure UART */
void dev_conf_uart(VOS_HANDLE uart, uint32 baud)
{
    common_ioctl_cb_t uart_iocb;

    uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_BAUD_RATE;
    uart_iocb.set.uart_baud_rate = baud;
    vos_dev_ioctl(uart, &uart_iocb);

    uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_FLOW_CONTROL;
    uart_iocb.set.param = UART_FLOW_NONE;
    vos_dev_ioctl(uart, &uart_iocb);
}

/* Configure USB host */
void dev_conf_usb(VOS_HANDLE usb)
{
    
}

/* Initialize timer */
void dev_conf_timer(VOS_HANDLE timer, uint8 mode)
{
    tmr_ioctl_cb_t iocb;

    iocb.ioctl_code = VOS_IOCTL_TIMER_SET_TICK_SIZE;
    iocb.param = TIMER_TICK_MS;
    vos_dev_ioctl(timer, &iocb);

    iocb.ioctl_code = VOS_IOCTL_TIMER_SET_DIRECTION;
    iocb.param = TIMER_COUNT_DOWN;
    vos_dev_ioctl(timer, &iocb);

    iocb.ioctl_code = VOS_IOCTL_TIMER_SET_MODE;
    iocb.param = mode;
    vos_dev_ioctl(timer, &iocb);
}

/* Start timer */
void dev_timer_start(VOS_HANDLE timer, uint16 timeout)
{
    tmr_ioctl_cb_t iocb;

    iocb.ioctl_code = VOS_IOCTL_TIMER_SET_COUNT;
    iocb.param = timeout;
    vos_dev_ioctl(timer, &iocb);

    iocb.ioctl_code = VOS_IOCTL_TIMER_START;
    vos_dev_ioctl(timer, &iocb);
}

/* Stop timer */
void dev_timer_stop(VOS_HANDLE timer)
{
    tmr_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_TIMER_STOP;
    vos_dev_ioctl(timer, &iocb);
}

/* Check timer status */
uint16 dev_timer_status(VOS_HANDLE timer)
{
    tmr_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_TIMER_GET_CURRENT_COUNT;
    vos_dev_ioctl(timer, &iocb);
    return iocb.param;
}

/* Wait for timer to expire */
void dev_timer_wait(VOS_HANDLE timer)
{
    tmr_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_TIMER_WAIT_ON_COMPLETE;
    vos_dev_ioctl(timer, &iocb);
}

/* Check status of the Rx queue */
uint16 dev_rx_avail(VOS_HANDLE dev)
{
    common_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
    vos_dev_ioctl(dev, &iocb);
    return iocb.get.queue_stat;
}

/* Acquire exclusive DMA access for device
** Decreases overhead for read/write operations */
void dev_dma_acquire(VOS_HANDLE dev)
{
    common_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
    iocb.set.param = DMA_ACQUIRE_AND_RETAIN;
    vos_dev_ioctl(dev, &iocb);
}

/* Release exclusive DMA access for device */
void dev_dma_release(VOS_HANDLE dev)
{
    common_ioctl_cb_t iocb;
    iocb.ioctl_code = VOS_IOCTL_COMMON_DISABLE_DMA;
    vos_dev_ioctl(dev, &iocb);
}
