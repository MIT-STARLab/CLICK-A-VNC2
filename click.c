/*
** Filename: click.c
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

#include "click.h"

/* FTDI:STP Thread Prototypes */
vos_tcb_t *tcbSPI;
vos_tcb_t *tcbUART;

void spi_thread();
void uart_thread();
/* FTDI:ETP */

/* FTDI:SDH Driver Handles */
VOS_HANDLE hUSBHOST_1; // USB Host Port 1
VOS_HANDLE hUART; // UART Interface Driver
VOS_HANDLE hSPI_SLAVE_0; // SPISlave Port 0 Interface Driver
VOS_HANDLE hSPI_SLAVE_1; // SPISlave Port 1 Interface Driver
VOS_HANDLE hGPIO_PORT_A; // GPIO Port A Driver
/* FTDI:EDH */

/* Declaration for IOMUx setup function */
void iomux_setup(void);

/* Main code - entry point to firmware */
void main(void)
{
	/* FTDI:SDD Driver Declarations */
	// UART Driver configuration context
	uart_context_t uartContext;
	// SPI Slave 0 configuration context
	spislave_context_t spisContext0;
	// SPI Slave 1 configuration context
	spislave_context_t spisContext1;
	// GPIO Port A configuration context
	gpio_context_t gpioContextA;
	// USB Host configuration context
	usbhost_context_t usbhostContext;
	/* FTDI:EDD */

	/* FTDI:SKI Kernel Initialisation */
	vos_init(50, VOS_TICK_INTERVAL, VOS_NUMBER_DEVICES);
	vos_set_clock_frequency(VOS_48MHZ_CLOCK_FREQUENCY);
	vos_set_idle_thread_tcb_size(512);
	/* FTDI:EKI */

	iomux_setup();

	/* FTDI:SDI Driver Initialisation */
	// Initialise UART
	uartContext.buffer_size = VOS_BUFFER_SIZE_128_BYTES;
	uart_init(VOS_DEV_UART,&uartContext);
	
	// Initialise SPI Slave 0
	spisContext0.buffer_size = VOS_BUFFER_SIZE_128_BYTES;
	spislave_init(VOS_DEV_SPI_SLAVE_0,&spisContext0);
	
	// Initialise SPI Slave 1
	spisContext1.buffer_size = VOS_BUFFER_SIZE_128_BYTES;
	spislave_init(VOS_DEV_SPI_SLAVE_1,&spisContext1);
	
	// Initialise GPIO A
	gpioContextA.port_identifier = GPIO_PORT_A;
	gpio_init(VOS_DEV_GPIO_PORT_A,&gpioContextA);
	
	
	// Initialise USB Host
	usbhostContext.if_count = 8;
	usbhostContext.ep_count = 16;
	usbhostContext.xfer_count = 2;
	usbhostContext.iso_xfer_count = 2;
	usbhost_init(VOS_DEV_USBHOST_1, -1, &usbhostContext);
	/* FTDI:EDI */

	/* FTDI:SCT Thread Creation */
	tcbSPI = vos_create_thread_ex(20, 4096, spi_thread, "spi", 0);
	tcbUART = vos_create_thread_ex(24, 1024, uart_thread, "uart", 0);
	/* FTDI:ECT */

	vos_start_scheduler();

main_loop:
	goto main_loop;
}

/* FTDI:SSP Support Functions */

unsigned char usbhost_connect_state(VOS_HANDLE hUSB)
{
	unsigned char connectstate = PORT_STATE_DISCONNECTED;
	usbhost_ioctl_cb_t hc_iocb;

	if (hUSB)
	{
		hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_GET_CONNECT_STATE;
		hc_iocb.get        = &connectstate;
		vos_dev_ioctl(hUSB, &hc_iocb);

    	// repeat if connected to see if we move to enumerated
		if (connectstate == PORT_STATE_CONNECTED)
		{
			vos_dev_ioctl(hUSB, &hc_iocb);
		}
	}
	return connectstate;
}

/* FTDI:ESP */

void open_drivers(void)
{
        /* Code for opening and closing drivers - move to required places in Application Threads */
        /* FTDI:SDA Driver Open */
        hUSBHOST_1 = vos_dev_open(VOS_DEV_USBHOST_1);
        hUART = vos_dev_open(VOS_DEV_UART);
        hSPI_SLAVE_0 = vos_dev_open(VOS_DEV_SPI_SLAVE_0);
        hSPI_SLAVE_1 = vos_dev_open(VOS_DEV_SPI_SLAVE_1);
        hGPIO_PORT_A = vos_dev_open(VOS_DEV_GPIO_PORT_A);
        /* FTDI:EDA */
}

void attach_drivers(void)
{
        /* FTDI:SUA Layered Driver Attach Function Calls */
        // TODO attach stdio to file system and stdio interface
        //fsAttach(hFAT_FILE_SYSTEM); // VOS_HANDLE for file system (typically FAT)
        //stdioAttach(hUART); // VOS_HANDLE for stdio interface (typically UART)
        /* FTDI:EUA */
}

void close_drivers(void)
{
        /* FTDI:SDB Driver Close */
        vos_dev_close(hUSBHOST_1);
        vos_dev_close(hUART);
        vos_dev_close(hSPI_SLAVE_0);
        vos_dev_close(hSPI_SLAVE_1);
        vos_dev_close(hGPIO_PORT_A);
        /* FTDI:EDB */
}

/* Application Threads */

void spi_thread()
{
	/* Thread code to be added here */


}

void uart_thread()
{
	/* Thread code to be added here */


}

