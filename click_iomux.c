/*
** Filename: click_iomux.c
**
** Automatically created by Application Wizard 2.0.2
** 
** Part of solution click in project click
**
** Comments: 
**
** Important: Sections between markers "FTDI:S*" and "FTDI:E*" will be overwritten by
** the Application Wizard
*/
#include "vos.h"

void iomux_setup(void)
{
	/* FTDI:SIO IOMux Functions */
	unsigned char packageType;
	
	packageType = vos_get_package_type();
	if (packageType == VINCULUM_II_48_PIN)
	{
		// SPI_Slave_0_CLK to pin 15 as Input.
		vos_iomux_define_input(15, IOMUX_IN_SPI_SLAVE_0_CLK);
		// SPI_Slave_0_MOSI to pin 16 as Input.
		vos_iomux_define_input(16, IOMUX_IN_SPI_SLAVE_0_MOSI);
		// SPI_Slave_0_MISO to pin 18 as Output.
		vos_iomux_define_output(18, IOMUX_OUT_SPI_SLAVE_0_MISO);
		// SPI_Slave_0_CS to pin 19 as Input.
		vos_iomux_define_input(19, IOMUX_IN_SPI_SLAVE_0_CS);
		// SPI_Slave_1_CLK to pin 20 as Input.
		vos_iomux_define_input(20, IOMUX_IN_SPI_SLAVE_1_CLK);
		// SPI_Slave_1_MOSI to pin 21 as Input.
		vos_iomux_define_input(21, IOMUX_IN_SPI_SLAVE_1_MOSI);
		// SPI_Slave_1_MISO to pin 22 as Output.
		vos_iomux_define_output(22, IOMUX_OUT_SPI_SLAVE_1_MISO);
		// SPI_Slave_1_CS to pin 23 as Input.
		vos_iomux_define_input(23, IOMUX_IN_SPI_SLAVE_1_CS);
		// UART_TXD to pin 31 as Output.
		vos_iomux_define_output(31, IOMUX_OUT_UART_TXD);
		// UART_RXD to pin 32 as Input.
		vos_iomux_define_input(32, IOMUX_IN_UART_RXD);
		// GPIO_Port_A_2 to pin 33 as Output.
		vos_iomux_define_output(33, IOMUX_OUT_GPIO_PORT_A_2);
		// GPIO_Port_A_7 to pin 34 as Output.
		vos_iomux_define_output(34, IOMUX_OUT_GPIO_PORT_A_7);
	}
	
	/* FTDI:EIO */
}
