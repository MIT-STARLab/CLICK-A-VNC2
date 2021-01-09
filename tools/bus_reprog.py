# Imitate golden image reprogramming from the bus through UART
from crccheck.crc import Crc16CcittFalse as crc16
import serial
import sys

port = 'COM8'
blob_len = 513
trash_len = 4
sync = bytearray([0x35, 0x2E, 0xF8, 0x53])

def create_pkt(data):
    payload_len = 6 + len(data) + trash_len + 2
    buf = bytearray()
    buf.append(0x02)
    buf.append(0x00)
    buf.append(0x00)
    buf.append(0x00)
    buf.append(((payload_len-1) >> 8) & 0xFF)
    buf.append((payload_len-1) & 0xFF)
    buf.extend(bytearray([0]*6))
    buf.extend(data)
    buf.extend(bytearray([0]*trash_len))
    crc = crc16.calc(buf)
    pkt = bytearray()
    pkt.extend(sync)
    pkt.extend(buf)
    pkt.append((crc >> 8) & 0xFF)
    pkt.append(crc & 0xFF)
    return pkt

def process_pkt(uart):
    sync_index = 0
    while sync_index < 4:
        try:
            b = uart.read(1)
            if len(b) == 1:
                if b[0] == sync[sync_index]:
                    sync_index += 1
                elif b[0] == sync[0]:
                    sync_index = 1
                else:
                    sync_index = 0
        except:
            sys.exit()

    header = uart.read(6)
    apid = (((header[0] & 0x7) << 8)) | header[1]
    pkt_len = ((header[4] << 8) | header[5]) + 1
    data = uart.read(pkt_len)
    return apid, data

def read_pkts(uart):
    while True:
        apid, data = process_pkt(uart)
        if apid == 0:
            print("[VNC2L] " + data.decode("utf-8"))
        elif apid == 0x220:
            break
        else:
            print("[" + "0x{:03X}".format(apid) + "] ")

if __name__ == "__main__":
    uart = serial.Serial(port, 921600, timeout = 1)
    with open("click_golden.img", "rb") as msd:
        print("Awaiting response...")
        while True:
            blob = msd.read(blob_len)
            if len(blob) > 0:
                uart.write(create_pkt(blob))
            read_pkts(uart)
