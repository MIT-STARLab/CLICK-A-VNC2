# CLICK-A-VNC2
Firmware for the VNC2L chip on the CLICK-A payload. The VNC2L implements:
* A SPI master-to-master pipe between the BCT bus and the payload Raspberry Pi CM3
* Reprogramming of the CM3 embedded flash memory when commanded

## SPI pipe
Since both the bus and the CM3 are SPI masters, a master-to-master pipe is implemented using two SPI slave peripherals on the VNC2L. A GPIO routed to the CM3 is used to signal when a packet from the bus is received. The CM3 then performs a read-write to receive the packet and send telemetry back (if any).

The pipe is implemented using three threads, described below.

### 1. The bus SPI thread
1. It starts in an unblocked state
2. It acquires DMA for the bus SPI and waits indefinitely for a packet from the bus.
2. When a packet is received, it checks if it is a special reprogramming initialization command.
3. If yes, it runs the reprogramming sequence (see details below).
4. If not, it switches DMA to the payload SPI and unblocks the other SPI payload thread to run in parallel.
5. It starts a blocking SPI slave write operation on the CM3 SPI.
6. If the packet is clocked out by the CM3 successfully, it waits for the operations to finish on the other thread, if any.
7. If the packet is never clocked out, the VNC2L will be stuck here (see details in reprogramming).
8. On success it jumps back to #2

### 2. The payload SPI thread
1. It starts in a blocked state, waiting for the bus SPI thread to unblock it.
2. Once unblocked, it toggles the signal GPIO connected to the CM3.
3. Then, it immediately starts a read operation on the payload SPI bus, waiting for any telemetry packet.
4. At this point, both threads are waiting for a read/write to be performed by the CM3.
5. If a valid telemetry packet is received, it will start a blocking SPI slave write operation on the bus SPI.
6. It jumps back to #1

### 3. The watchdog SPI thread
The third thread is a special watchdog that checks if packets were successfully clocked out by the CM3. In case there is a readback pending from the CM3 for longer than two seconds, this thread will toggle the signal GPIO again at 1 Hz, until the CM3 clocks the data out.

This is helpful for example when a packet is received by the VNC2L before the CM3 has finished booting up, in which case it would miss the interrupt on the signal GPIO.

However, if the CM3 is not responding for some reason, the VNC2L will still be stuck and unable to receive any more bus commands.

## Reprogramming
When commanded with a special SPI payload write packet (APID 0x7EF, no data), the VNC2L will run the CM3 reprogramming sequence and overwrite the flash with contents recieved through UART.

The reprogramming sequence is based on the usbboot project from Raspberry Pi: [https://github.com/raspberrypi/usbboot/tree/f4e3f0f9a3c64d846ba53ec3367e33a4f9a7d051](https://github.com/raspberrypi/usbboot/tree/f4e3f0f9a3c64d846ba53ec3367e33a4f9a7d051). Note that an old version from October 2016 is used, because this version has less overhead (bootloader files are smaller) and the logic is simpler than in the newer versions. The newer versions support other boot modes than just mass storage device, which CLICK does not need.

### Caveats
Unfortunately, due to the VNC2L driver limitations, the 0x7EF packet must be sent by the bus ONLY when there is no payload SPI readback pending. This means that it can be sent:
* Anytime the CM3 is "responsive", and completes read back of the SPI packets. This will make sure the VNC2L is not blocked awaiting a read from the CM3.
* If the CM3 is not responsive, the 0x7EF must be the FIRST packet the VNC2L receives on boot. This can be accomplished by disabling the Time of Tone and no-op bus packets, then turning the power off and on, and sending the 0x7EF packet.

Furthermore, due to another bug in the VNC2L USB driver, the VNC2L has to reset itself roughly after 12-13 seconds since the first 0x7EF is received.

After that, another 0x7EF must be sent to finish the USB initialization sequence. Only then can the UART data be sent from the bus.

The bus shall thus implement the following reprogramming sequence:
1. Turn ToT & no-op packets off
2. Turn payload power off
3. Turn payload power on
4. Wait 1 sec for VNC2L boot
5. Turn payload enable on
6. Send 0x7EF
7. Wait 14-15 sec
8. Send 0x7EF
9. Wait 3 sec
10. Start UART golden image transfer
11. Turn ToT & no-op packets on
