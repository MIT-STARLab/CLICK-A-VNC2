/*
** Filename: dev_conf.c
** Author: Ondrej
** 
** Handle device configuration
*/

#include "dev_conf.h"
#include "IOCTL.h"

// Configure IOMUX connections
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
}

// Configure SPI bus
void dev_conf_spi(VOS_HANDLE spi)
{
    common_ioctl_cb_t spi_iocb;

    spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPHA;
    spi_iocb.set.param = SPI_SLAVE_SCK_CPHA_0;
    vos_dev_ioctl(spi, &spi_iocb);

    spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPOL;
    spi_iocb.set.param = SPI_SLAVE_SCK_CPOL_0;
    vos_dev_ioctl(spi, &spi_iocb);

    spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_DATA_ORDER;
    spi_iocb.set.param = SPI_SLAVE_DATA_ORDER_MSB;
    vos_dev_ioctl(spi, &spi_iocb);

    spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_ADDRESS;
    spi_iocb.set.param = 0;
    vos_dev_ioctl(spi, &spi_iocb);

    spi_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_MODE;
    spi_iocb.set.param = SPI_SLAVE_MODE_UNMANAGED;
    vos_dev_ioctl(spi, &spi_iocb);

    spi_iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
    spi_iocb.set.param = DMA_ACQUIRE_AS_REQUIRED;
    vos_dev_ioctl(spi, &spi_iocb);
}

// Configure UART
void dev_conf_uart(VOS_HANDLE uart)
{
    common_ioctl_cb_t uart_iocb;

    uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_BAUD_RATE;
    uart_iocb.set.uart_baud_rate = 115200;
    vos_dev_ioctl(uart, &uart_iocb);

    uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_FLOW_CONTROL;
    uart_iocb.set.param = UART_FLOW_NONE;
    vos_dev_ioctl(uart, &uart_iocb);
}