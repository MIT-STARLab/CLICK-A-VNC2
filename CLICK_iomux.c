/*
** Filename: CLICK_iomux.c
**
** Automatically created by Application Wizard 2.0.2
** 
** Part of solution CLICK in project CLICK
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
		// Debugger to pin 11 as Bi-Directional.
		vos_iomux_define_bidi(199, IOMUX_IN_DEBUGGER, IOMUX_OUT_DEBUGGER);
		// PWM_1 to pin 12 as Output.
		vos_iomux_define_output(12, IOMUX_OUT_PWM_1);
		// PWM_2 to pin 13 as Output.
		vos_iomux_define_output(13, IOMUX_OUT_PWM_2);
		// PWM_3 to pin 14 as Output.
		vos_iomux_define_output(14, IOMUX_OUT_PWM_3);
		// SPI_Slave_1_CLK to pin 20 as Input.
		vos_iomux_define_input(15, IOMUX_IN_SPI_SLAVE_0_CLK);
		// SPI_Slave_1_MOSI to pin 21 as Input.
		vos_iomux_define_input(16, IOMUX_IN_SPI_SLAVE_0_MOSI);
		// SPI_Slave_1_MISO to pin 22 as Output.
		vos_iomux_define_output(18, IOMUX_OUT_SPI_SLAVE_0_MISO);
		// SPI_Slave_1_CS to pin 23 as Input.
		vos_iomux_define_input(19, IOMUX_IN_SPI_SLAVE_0_CS);
		// SPI_Slave_0_CLK to pin 15 as Input.
		vos_iomux_define_input(20, IOMUX_IN_SPI_SLAVE_1_CLK);
		// SPI_Slave_0_MOSI to pin 16 as Input.
		vos_iomux_define_input(21, IOMUX_IN_SPI_SLAVE_1_MOSI);
		// SPI_Slave_0_MISO to pin 18 as Output.
		vos_iomux_define_output(22, IOMUX_OUT_SPI_SLAVE_1_MISO);
		// SPI_Slave_0_CS to pin 19 as Input.
		vos_iomux_define_input(23, IOMUX_IN_SPI_SLAVE_1_CS);
		// UART_TXD to pin 31 as Output.
		vos_iomux_define_output(31, IOMUX_OUT_UART_TXD);
		// UART_RXD to pin 32 as Input.
		vos_iomux_define_input(32, IOMUX_IN_UART_RXD);
		// UART_RTS_N to pin 33 as Output.
		vos_iomux_define_output(33, IOMUX_OUT_UART_RTS_N);
		// UART_CTS_N to pin 34 as Input.
		vos_iomux_define_input(34, IOMUX_IN_UART_CTS_N);
		// UART_DTR_N to pin 35 as Output.
		vos_iomux_define_output(35, IOMUX_OUT_UART_DTR_N);
		// UART_DSR_N to pin 36 as Input.
		vos_iomux_define_input(36, IOMUX_IN_UART_DSR_N);
		// UART_DCD to pin 37 as Input.
		vos_iomux_define_input(37, IOMUX_IN_UART_DCD);
		// UART_RI to pin 38 as Input.
		vos_iomux_define_input(38, IOMUX_IN_UART_RI);
		// UART_TX_Active to pin 41 as Output.
		vos_iomux_define_output(41, IOMUX_OUT_UART_TX_ACTIVE);
		// GPIO_Port_A_5 to pin 42 as Output.
		vos_iomux_define_output(42, IOMUX_OUT_GPIO_PORT_A_5);
		// GPIO_Port_A_6 to pin 43 as Output.
		vos_iomux_define_output(43, IOMUX_OUT_GPIO_PORT_A_6);
		// GPIO_Port_A_7 to pin 44 as Input.
		vos_iomux_define_input(44, IOMUX_IN_GPIO_PORT_A_7);
		// GPIO_Port_A_0 to pin 45 as Output.
		vos_iomux_define_output(45, IOMUX_OUT_GPIO_PORT_A_0);
		// GPIO_Port_A_1 to pin 46 as Input.
		vos_iomux_define_input(46, IOMUX_IN_GPIO_PORT_A_1);
		// GPIO_Port_A_2 to pin 47 as Input.
		vos_iomux_define_input(47, IOMUX_IN_GPIO_PORT_A_2);
		// GPIO_Port_A_3 to pin 48 as Output.
		vos_iomux_define_output(48, IOMUX_OUT_GPIO_PORT_A_3);
	
	}
	
	/* FTDI:EIO */
}
