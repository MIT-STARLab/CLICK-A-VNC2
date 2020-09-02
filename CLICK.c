/*
** Filename: CLICK.c
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

#include "CLICK.h"

/* FTDI:STP Thread Prototypes */
vos_tcb_t *tcbBUS;
vos_tcb_t *tcbREPROGRAMMING;

void bus();
void reprogramming();
/* FTDI:ETP */

/* FTDI:SDH Driver Handles */
VOS_HANDLE hUSBHOST_1; // USB Host Port 1
VOS_HANDLE hUART; // UART Interface Driver
VOS_HANDLE hSPI_SLAVE_0; // SPISlave Port 0 Interface Driver
VOS_HANDLE hSPI_SLAVE_1; // SPISlave Port 1 Interface Driver
VOS_HANDLE hGPIO_PORT_A; // GPIO Port A Driver
VOS_HANDLE hFAT_FILE_SYSTEM; // FAT File System for FAT32 and FAT16
VOS_HANDLE hBOMS; // Bulk Only Mass Storage for USB disks
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
	uartContext.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
	uart_init(VOS_DEV_UART,&uartContext);
	
	// Initialise SPI Slave 0
	spisContext0.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
	spisContext0.slavenumber = SPI_SLAVE_0;
	spislave_init(VOS_DEV_SPI_SLAVE_0,&spisContext0);
	
	// Initialise SPI Slave 1
	spisContext1.buffer_size = VOS_BUFFER_SIZE_512_BYTES;
	spisContext1.slavenumber = SPI_SLAVE_1;
	spislave_init(VOS_DEV_SPI_SLAVE_1,&spisContext1);
	
	// Initialise GPIO A
	gpioContextA.port_identifier = GPIO_PORT_A;
	gpio_init(VOS_DEV_GPIO_PORT_A,&gpioContextA);
	
	// Initialise FAT File System Driver
	fatdrv_init(VOS_DEV_FAT_FILE_SYSTEM);
	
	// Initialise BOMS Device Driver
	boms_init(VOS_DEV_BOMS);
	
	
	
	
	
	
	// Initialise USB Host
	usbhostContext.if_count = 8;
	usbhostContext.ep_count = 16;
	usbhostContext.xfer_count = 2;
	usbhostContext.iso_xfer_count = 2;
	usbhost_init(VOS_DEV_USBHOST_1, -1, &usbhostContext);
	/* FTDI:EDI */

	/* FTDI:SCT Thread Creation */
	tcbBUS = vos_create_thread_ex(20, 2028, bus, "TT&C", 0);
	tcbREPROGRAMMING = vos_create_thread_ex(24, 4096, reprogramming, "Reprogramming", 0);
	/* FTDI:ECT */

	vos_start_scheduler();
	
main_loop:
	goto main_loop;
}

/* FTDI:SSP Support Functions */
/*
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


VOS_HANDLE fat_attach(VOS_HANDLE hMSI, unsigned char devFAT)
{
	fat_ioctl_cb_t           fat_ioctl;
	fatdrv_ioctl_cb_attach_t fat_att;
	VOS_HANDLE hFAT;

	// currently the MSI (BOMS or other) must be open
        // open the FAT driver
	hFAT = vos_dev_open(devFAT);

        // attach the FAT driver to the MSI driver
	fat_ioctl.ioctl_code = FAT_IOCTL_FS_ATTACH;
	fat_ioctl.set = &fat_att;
	fat_att.msi_handle = hMSI;
	fat_att.partition = 0;

	if (vos_dev_ioctl(hFAT, &fat_ioctl) != FAT_OK)
	{
                // unable to open the FAT driver
		vos_dev_close(hFAT);
		hFAT = NULL;
	}

	return hFAT;
}

void fat_detach(VOS_HANDLE hFAT)
{
	fat_ioctl_cb_t           fat_ioctl;

	if (hFAT)
	{
		fat_ioctl.ioctl_code = FAT_IOCTL_FS_DETACH;
		fat_ioctl.set = NULL;
		fat_ioctl.get = NULL;

		vos_dev_ioctl(hFAT, &fat_ioctl);
		vos_dev_close(hFAT);
	}
}


VOS_HANDLE boms_attach(VOS_HANDLE hUSB, unsigned char devBOMS)
{
	usbhost_device_handle_ex ifDisk = 0;
	usbhost_ioctl_cb_t hc_iocb;
	usbhost_ioctl_cb_class_t hc_iocb_class;
	msi_ioctl_cb_t boms_iocb;
	boms_ioctl_cb_attach_t boms_att;
	VOS_HANDLE hBOMS;

	// find BOMS class device
	hc_iocb_class.dev_class = USB_CLASS_MASS_STORAGE;
	hc_iocb_class.dev_subclass = USB_SUBCLASS_MASS_STORAGE_SCSI;
	hc_iocb_class.dev_protocol = USB_PROTOCOL_MASS_STORAGE_BOMS;

	// user ioctl to find first hub device
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_FIND_HANDLE_BY_CLASS;
	hc_iocb.handle.dif = NULL;
	hc_iocb.set = &hc_iocb_class;
	hc_iocb.get = &ifDisk;

	if (vos_dev_ioctl(hUSB, &hc_iocb) != USBHOST_OK)
	{
		return NULL;
	}

	// now we have a device, intialise a BOMS driver with it
	hBOMS = vos_dev_open(devBOMS);

	// perform attach
	boms_att.hc_handle = hUSB;
	boms_att.ifDev = ifDisk;

	boms_iocb.ioctl_code = MSI_IOCTL_BOMS_ATTACH;
	boms_iocb.set = &boms_att;
	boms_iocb.get = NULL;

	if (vos_dev_ioctl(hBOMS, &boms_iocb) != MSI_OK)
	{
		vos_dev_close(hBOMS);
		hBOMS = NULL;
	}

	return hBOMS;
}

void boms_detach(VOS_HANDLE hBOMS)
{
	msi_ioctl_cb_t boms_iocb;

	if (hBOMS)
	{
		boms_iocb.ioctl_code = MSI_IOCTL_BOMS_DETACH;
		boms_iocb.set = NULL;
		boms_iocb.get = NULL;

		vos_dev_ioctl(hBOMS, &boms_iocb);
		vos_dev_close(hBOMS);
	}
}



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

/*
VOS_HANDLE fat_attach(VOS_HANDLE hMSI, unsigned char devFAT)
{
	fat_ioctl_cb_t           fat_ioctl;
	fatdrv_ioctl_cb_attach_t fat_att;
	VOS_HANDLE hFAT;

	// currently the MSI (BOMS or other) must be open
        // open the FAT driver
	hFAT = vos_dev_open(devFAT);

        // attach the FAT driver to the MSI driver
	fat_ioctl.ioctl_code = FAT_IOCTL_FS_ATTACH;
	fat_ioctl.set = &fat_att;
	fat_att.msi_handle = hMSI;
	fat_att.partition = 0;

	if (vos_dev_ioctl(hFAT, &fat_ioctl) != FAT_OK)
	{
                // unable to open the FAT driver
		vos_dev_close(hFAT);
		hFAT = NULL;
	}

	return hFAT;
}

void fat_detach(VOS_HANDLE hFAT)
{
	fat_ioctl_cb_t           fat_ioctl;

	if (hFAT)
	{
		fat_ioctl.ioctl_code = FAT_IOCTL_FS_DETACH;
		fat_ioctl.set = NULL;
		fat_ioctl.get = NULL;

		vos_dev_ioctl(hFAT, &fat_ioctl);
		vos_dev_close(hFAT);
	}
}


VOS_HANDLE boms_attach(VOS_HANDLE hUSB, unsigned char devBOMS)
{
	usbhost_device_handle_ex ifDisk = 0;
	usbhost_ioctl_cb_t hc_iocb;
	usbhost_ioctl_cb_class_t hc_iocb_class;
	msi_ioctl_cb_t boms_iocb;
	boms_ioctl_cb_attach_t boms_att;
	VOS_HANDLE hBOMS;

	// find BOMS class device
	hc_iocb_class.dev_class = USB_CLASS_MASS_STORAGE;
	hc_iocb_class.dev_subclass = USB_SUBCLASS_MASS_STORAGE_SCSI;
	hc_iocb_class.dev_protocol = USB_PROTOCOL_MASS_STORAGE_BOMS;

	// user ioctl to find first hub device
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_FIND_HANDLE_BY_CLASS;
	hc_iocb.handle.dif = NULL;
	hc_iocb.set = &hc_iocb_class;
	hc_iocb.get = &ifDisk;

	if (vos_dev_ioctl(hUSB, &hc_iocb) != USBHOST_OK)
	{
		return NULL;
	}

	// now we have a device, intialise a BOMS driver with it
	hBOMS = vos_dev_open(devBOMS);

	// perform attach
	boms_att.hc_handle = hUSB;
	boms_att.ifDev = ifDisk;

	boms_iocb.ioctl_code = MSI_IOCTL_BOMS_ATTACH;
	boms_iocb.set = &boms_att;
	boms_iocb.get = NULL;

	if (vos_dev_ioctl(hBOMS, &boms_iocb) != MSI_OK)
	{
		vos_dev_close(hBOMS);
		hBOMS = NULL;
	}

	return hBOMS;
}

void boms_detach(VOS_HANDLE hBOMS)
{
	msi_ioctl_cb_t boms_iocb;

	if (hBOMS)
	{
		boms_iocb.ioctl_code = MSI_IOCTL_BOMS_DETACH;
		boms_iocb.set = NULL;
		boms_iocb.get = NULL;

		vos_dev_ioctl(hBOMS, &boms_iocb);
		vos_dev_close(hBOMS);
	}
}

void attach_drivers(void)
{
        hBOMS = boms_attach(hUSBHOST_1, VOS_DEV_BOMS);
        hFAT_FILE_SYSTEM = fat_attach(hBOMS, VOS_DEV_FAT_FILE_SYSTEM);
        // TODO attach stdio to file system and stdio interface
        //fsAttach(hFAT_FILE_SYSTEM); // VOS_HANDLE for file system (typically FAT)
        //stdioAttach(hUART); // VOS_HANDLE for stdio interface (typically UART)
        // TODO attach stdio to file system and stdio interface
        //fsAttach(hFAT_FILE_SYSTEM); // VOS_HANDLE for file system (typically FAT)
        //stdioAttach(hUART); // VOS_HANDLE for stdio interface (typically UART)
}
*/
/* Application Threads */

void uartsetup(VOS_HANDLE hUART)
{
	common_ioctl_cb_t uart_iocb;
	
	uart_iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
	uart_iocb.set.param = DMA_ACQUIRE_AS_REQUIRED; 
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_BAUD_RATE;
	uart_iocb.set.uart_baud_rate = 921600;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_FLOW_CONTROL;
	uart_iocb.set.param = UART_FLOW_NONE;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_DATA_BITS;
	uart_iocb.set.param = UART_DATA_BITS_8;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	// UART set stop bits
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_STOP_BITS;
	uart_iocb.set.param = UART_STOP_BITS_1;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	uart_iocb.ioctl_code = VOS_IOCTL_UART_SET_PARITY;
	uart_iocb.set.param = UART_PARITY_NONE;
	vos_dev_ioctl(hUART, &uart_iocb);
	
	return;
}
	/*
void get_next_packet(VOS_HANDLE hUART){
	
	return;
}
	*/
	
unsigned char check_sync(unsigned char *buf){
	if(buf[0]==0x35 && buf[1] == 0x2E && buf[2] == 0xF8 && buf[3] == 0x53){
		return 1;
	}
	else{return 0;}
}
	
void reprogramming()
{
	// variable declarations
	VOS_HANDLE hUART = vos_dev_open(VOS_DEV_UART);
	VOS_HANDLE hUSBHOST_1 = vos_dev_open(VOS_DEV_USBHOST_1);
	VOS_HANDLE hGPIO = vos_dev_open(VOS_DEV_GPIO_PORT_A);
	unsigned short dataAvail = 0;
	unsigned char buf[512];
	unsigned char writeBuf[8];
	unsigned short numRead = 0;
	unsigned char i = 0;
	unsigned char not_done = 1; 
	short size[2];
	char ret = 0;	
	unsigned char counter = 0;

	
	usbhost_ioctl_cb_vid_pid_t usbhost_ioctVidPid;
	usbhost_ep_handle epHandle = NULL;
	unsigned short myVid;
	unsigned short myPid; 
	usbhost_ioctl_cb_t hc_iocb;
	common_ioctl_cb_t uart_iocb;
	usb_deviceRequest_t bootCtrl; 
	usbhost_xfer_t bootBulk;
	usbhost_ep_handle hCtrl = NULL;
	usbhost_ep_handle hBulkOut = NULL;
	usbhost_device_handle hUSBRPi = NULL; // handle to the next device interface
	
	unsigned char processing[12] = {0x35, 0x2E, 0xF8, 0x53, 0x02, 0x15, 0, 0, 0, 1, 187, 21};
	unsigned char ready[12] = {0x35, 0x2E, 0xF8, 0x53, 0x02, 0x20, 0, 0, 0, 1, 228, 131};
	unsigned char heartbeat[12] = {0x35, 0x2E, 0xF8, 0x53, 0x02, 0x25, 0, 0, 0, 1, 92, 141};
	unsigned char retry[12] = {0x35, 0x2E, 0xF8, 0x53, 0x02, 0x30, 0, 0, 0, 1, 187, 198};



	// configure UART
	uartsetup(hUART);
	while(1){
		//buf[0] = 0xD8;
		uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
		vos_dev_ioctl(hUART, &uart_iocb);
		dataAvail = uart_iocb.get.queue_stat; 
		if(dataAvail>0){
			vos_dev_read(hUART, buf, dataAvail, &numRead);
			if(check_sync(buf)){
				vos_dev_write(hUART, processing, 12, NULL);
				vos_delay_msecs(1000);
				vos_dev_write(hUART, ready, 12, NULL);
			}
			else{
				vos_dev_write(hUART, retry, 12, NULL);
			}
			dataAvail = 0;
		}
		if(counter == 255){
			vos_dev_write(hUART, heartbeat, 12, NULL);
			counter = 0;
		}
		counter++; 
		vos_delay_msecs(100); 
	}
}
	/*
	vos_dev_write(hGPIO, 0x01, 1, NULL);
	
	// wait for device to be plugged in, get vid, pid
	while(dataAvail != 4){
		uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
		vos_dev_ioctl(hUART, &uart_iocb);
		dataAvail = uart_iocb.get.queue_stat; 
		vos_delay_msecs(250);
	}
	vos_dev_read(hUART, buf, dataAvail, &numRead);
	vos_dev_write(hUART, buf, dataAvail, NULL);
	dataAvail = 0;
	
	buf[0] = 0x77;
	//usbsetup(hUART, hUSBHOST_1, &hUSBRPi, buf, hCtrl, hBulkOut);
	
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_ENUMERATE;
	hc_iocb.handle.dif = NULL;
	vos_dev_ioctl(hUSBHOST_1, &hc_iocb);

	while(usbhost_connect_state(hUSBHOST_1) != PORT_STATE_ENUMERATED){vos_delay_msecs(250);}
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_NEXT_HANDLE;
	hc_iocb.handle.dif = NULL;
	hc_iocb.get = &hUSBRPi;
	vos_dev_ioctl(hUSBHOST_1, &hc_iocb);
	
	// notify bus over uart that device plugged in and ready
	
	hc_iocb.handle.dif = hUSBRPi;
	if(hUSBRPi){
		hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_VID_PID;
		hc_iocb.get = &usbhost_ioctVidPid;
		vos_dev_ioctl(hUSBHOST_1, &hc_iocb);
		vos_delay_msecs(250);
		myVid = usbhost_ioctVidPid.vid;
		myPid = usbhost_ioctVidPid.pid;  
		vos_dev_write(hUART, (unsigned char *) &usbhost_ioctVidPid, 4, NULL);
	}
	else{
		buf[0] = 0x22; 
		vos_dev_write(hUART, buf, 4, NULL); 
		return;
	}
	
	// device plugged in, need to get interface handle via VID and PID
	
	//usbhost_ioctVidPid.vid = 0x0a5c;
	//usbhost_ioctVidPid.pid = 0x2764;
	
	// now get endpoint handles (control and bulk)
	
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_CONTROL_ENDPOINT_HANDLE;
	hc_iocb.handle.dif = hUSBRPi;
	hc_iocb.get = &hCtrl;
	hc_iocb.set = NULL;
	vos_dev_ioctl(hUSBHOST_1, &hc_iocb);
	
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_BULK_OUT_ENDPOINT_HANDLE;
	hc_iocb.get = &hBulkOut;
	vos_dev_ioctl(hUSBHOST_1, &hc_iocb);
	
	// check to see if second set of endpoints
	
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_GET_NEXT_ENDPOINT_HANDLE;
	hc_iocb.handle.ep = hCtrl;
	hc_iocb.get = &epHandle; 
	vos_dev_ioctl(hUSBHOST_1, &hc_iocb);
	
	if(epHandle){
		hCtrl = epHandle;
		hc_iocb.handle.ep = hBulkOut;
		vos_dev_ioctl(hUSBHOST_1, &hc_iocb);
		
		if(epHandle){
			hBulkOut = epHandle; 
		}
		else{
			buf[1] = 0x77;
			vos_dev_write(hUART, buf, 4, NULL);
		}
	}
	else{
		vos_dev_write(hUART, buf, 4, NULL);
	}
	
	// get bootcode.bin over uart and point second_stage at it
	
	// first 2 bytes should be total length of bootcode.bin, buf[0]<<8+buf[1] = MSB, buf[2]<<8+buf[3] = LSB
	// writes to the bootloader occur as a control transfer telling it the number of bytes coming then a bulk transfer with those bytes
	while(dataAvail != 24){
		uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
		vos_dev_ioctl(hUART, &uart_iocb);
		dataAvail = uart_iocb.get.queue_stat; 
		vos_delay_msecs(250);
	}
	vos_dev_read(hUART, buf, dataAvail, &numRead);
	dataAvail = 0;
	
	size[0] = (buf[0]<<8)+buf[1]; size[1] = (buf[2]<<8)+buf[3];
	writeBuf[0] = 0x55;
	writeBuf[1] = buf[0]; writeBuf[2] = buf[1]; writeBuf[3] = buf[2]; writeBuf[4] = buf[3];
	vos_dev_write(hUART, writeBuf, 5, NULL);
	
	
	// write length of bootcode to EP, uses vendor specific control transfer so ignore table 9-4
	// first control transfer is just saying 4 bytes (size of bootcode can be an int)
	// then the actual size gets written (is called boot message)
	bootCtrl.bmRequestType = USB_BMREQUESTTYPE_VENDOR; 
	bootCtrl.bRequest = 0;
	bootCtrl.wValue = 0x18; // length low 2 bytes
	bootCtrl.wIndex = 0; // length high 2 bytes
	bootCtrl.wLength = 0;
	
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_SETUP_TRANSFER;
	hc_iocb.handle.ep = hCtrl;
	hc_iocb.set = &bootCtrl;
	hc_iocb.get = NULL;
	vos_dev_ioctl(hUSBHOST_1, &hc_iocb); 
	
	bootBulk.ep = hBulkOut;
	bootBulk.buf = buf;
	bootBulk.len = 24;
	bootBulk.zero = 0;
	vos_dev_write(hUSBHOST_1, (unsigned char *) &bootBulk, sizeof(usbhost_xfer_t), NULL);

	// write bootcode to EP, needs to be in a while loop
	
	bootCtrl.bmRequestType = USB_BMREQUESTTYPE_VENDOR; 
	bootCtrl.bRequest = 0;
	bootCtrl.wValue = size[1]; // length low 2 bytes
	bootCtrl.wIndex = size[0]; // length high 2 bytes
	bootCtrl.wLength = 0;
	
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_SETUP_TRANSFER;
	hc_iocb.handle.ep = hCtrl;
	hc_iocb.set = &bootCtrl;
	hc_iocb.get = NULL;
	vos_dev_ioctl(hUSBHOST_1, &hc_iocb);

	while(not_done){
		while(dataAvail!= 1){
			uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
			vos_dev_ioctl(hUART, &uart_iocb);
			dataAvail = uart_iocb.get.queue_stat; 
			vos_delay_msecs(250);
		}
		
		vos_dev_read(hUART, buf, dataAvail, &numRead);
		
		if(buf[0] == 0x88){
			not_done = 0;
			writeBuf[0] = 0x44;
			vos_dev_write(hUART, writeBuf, 1, NULL);
			
			bootCtrl.bmRequestType = USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_DEV_TO_HOST; 
			bootCtrl.bRequest = 0;
			bootCtrl.wValue = 4; // length low 2 bytes
			bootCtrl.wIndex = 0; // length high 2 bytes
			bootCtrl.wLength = 4;
			
			hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_SETUP_TRANSFER;
			hc_iocb.handle.ep = hCtrl;
			hc_iocb.set = &bootCtrl;
			hc_iocb.get = writeBuf;
			vos_dev_ioctl(hUSBHOST_1, &hc_iocb);
			vos_delay_msecs(1000);

			vos_dev_write(hUART, writeBuf, 4, NULL);
		}
		else{
			
			writeBuf[0] = 0x55;
			vos_dev_write(hUART, writeBuf, 1, NULL);
			
			while(dataAvail != 512){
			uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
			vos_dev_ioctl(hUART, &uart_iocb);
			dataAvail = uart_iocb.get.queue_stat; 
			vos_delay_msecs(250);
			}
			
			vos_dev_read(hUART, buf, dataAvail, &numRead);
	
			dataAvail = 0;
			bootBulk.ep = hBulkOut;
			bootBulk.buf = buf;
			bootBulk.len = 512;
			bootBulk.zero = 0;
			ret = vos_dev_write(hUSBHOST_1, &bootBulk, sizeof(usbhost_xfer_t), NULL);
			
			vos_dev_write(hUART, &ret, 1, NULL);
		}
	}
	
	fuark:
		goto fuark;
	
	
	// now load start.elf, might need to get new handles
	/*
	usbhost_xfer_t elf;
	elf.ep = hBulkOut;
	elf.buf = second_stage;
	elf.len = 0;
	elf.zero = 0;
	vos_dev_write(hUSBHOST_1, &elf, sizeof(usbhost_xfer_t), NULL);
	
}*/
	
void bus()
{
	
	common_ioctl_cb_t spi0_iocb;
	common_ioctl_cb_t spi1_iocb;
	common_ioctl_cb_t uart_iocb;
			
	unsigned short dataAvail = 0;
	unsigned short transmitting = 0;
	//uint8 error = 0;
	unsigned char bus_in[512];
	unsigned char bus_out[512];
	
	//VOS_HANDLE hUART = vos_dev_open(VOS_DEV_UART);
	VOS_HANDLE hSPI_SLAVE_0 = vos_dev_open(VOS_DEV_SPI_SLAVE_0);
	VOS_HANDLE hSPI_SLAVE_1 = vos_dev_open(VOS_DEV_SPI_SLAVE_1);
	
	//uartsetup(hUART);
	
	// Setup SPI Slave 0
	spi0_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPHA;
	spi0_iocb.set.param = SPI_SLAVE_SCK_CPHA_0;
	vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);

	spi0_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPOL;
	spi0_iocb.set.param = SPI_SLAVE_SCK_CPOL_0;
	vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);

	spi0_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_DATA_ORDER;
	spi0_iocb.set.param = SPI_SLAVE_DATA_ORDER_MSB;
	vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);

	spi0_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_ADDRESS;
	spi0_iocb.set.param = 0;
	vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);

	spi0_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_MODE;
	spi0_iocb.set.param = SPI_SLAVE_MODE_UNMANAGED;
	vos_dev_ioctl(hSPI_SLAVE_0,&spi0_iocb);
	
	spi0_iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
	spi0_iocb.set = DMA_ACQUIRE_AS_REQUIRED;
	vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);
	
	// Setup SPI Slave 1
	
	spi1_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPHA;
	spi1_iocb.set.param = SPI_SLAVE_SCK_CPHA_0;
	vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);

	spi1_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SCK_CPOL;
	spi1_iocb.set.param = SPI_SLAVE_SCK_CPOL_0;
	vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);

	spi1_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_DATA_ORDER;
	spi1_iocb.set.param = SPI_SLAVE_DATA_ORDER_MSB;
	vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);

	spi1_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_ADDRESS;
	spi1_iocb.set.param = 7;
	vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);

	spi1_iocb.ioctl_code = VOS_IOCTL_SPI_SLAVE_SET_MODE;
	spi1_iocb.set.param = SPI_SLAVE_MODE_UNMANAGED;
	vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);
	
	spi1_iocb.ioctl_code = VOS_IOCTL_COMMON_ENABLE_DMA;
	spi1_iocb.set = DMA_ACQUIRE_AS_REQUIRED;
	vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);
	
	spi0_iocb.ioctl_code = VOS_IOCTL_COMMON_RESET;
	vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);
	
	spi1_iocb.ioctl_code = VOS_IOCTL_COMMON_RESET;
	vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);
	
	//bus_out[0] = 0x56; bus_out[1] = 0x89; bus_out[2] = 0xA2; bus_out[3] = 0x19; 
	//vos_dev_write(hSPI_SLAVE_0, bus_out, 4, NULL);
	//vos_dev_write(hSPI_SLAVE_1, bus_out, 4, NULL);
	
	// Bus is SPI 0, Payload is SPI 1
	while(1){
		// get bytes available...
	
		vos_delay_msecs(10);
		spi0_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
		vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);
		vos_delay_msecs(100);
		dataAvail = spi0_iocb.get.queue_stat; // How much data to read?
		
		/*vos_delay_msecs(10);
		uart_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
		vos_dev_ioctl(hUART, &uart_iocb);
		dataAvail = uart_iocb.get.queue_stat; // How much data to read?
		vos_delay_msecs(1000);*/
		spi1_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_TX_QUEUE_STATUS;
		vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);
		transmitting = spi1_iocb.get.queue_stat; // How much data to read?
		
		
		if (dataAvail) {
			vos_dev_read(hSPI_SLAVE_0, bus_out, dataAvail, NULL);
			//vos_dev_read(hUART, bus_out, dataAvail, NULL);
			vos_delay_msecs(10);
			if(check_sync(bus_out) && (transmitting==0)){
				//vos_dev_write(hUART, bus_out, dataAvail, NULL);
			    vos_dev_write(hSPI_SLAVE_1, bus_out, dataAvail, NULL);
			    //bus_out[0] = 0xED; bus_out[1] = 0xAB; 
			    //vos_dev_write(hUART, bus_out, 4, NULL);
			}
			dataAvail = 0;
			
		}
		
		vos_delay_msecs(10);
		spi1_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_RX_QUEUE_STATUS;
		vos_dev_ioctl(hSPI_SLAVE_1, &spi1_iocb);
		dataAvail = spi1_iocb.get.queue_stat; // How much data to read?
		
		vos_delay_msecs(10);
		spi0_iocb.ioctl_code = VOS_IOCTL_COMMON_GET_TX_QUEUE_STATUS;
		vos_dev_ioctl(hSPI_SLAVE_0, &spi0_iocb);
		transmitting = spi0_iocb.get.queue_stat; // How much data to read?
		
		if (dataAvail)
		{
			vos_dev_read(hSPI_SLAVE_1, bus_in, dataAvail, NULL);
			vos_delay_msecs(10);
			if(check_sync(bus_in) && (transmitting==0)){
				vos_dev_write(hSPI_SLAVE_0, bus_in, dataAvail, NULL);
				//vos_dev_write(hUART, bus_in, dataAvail, NULL);
			}
			dataAvail = 0;
		}
	}
}

