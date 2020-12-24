# Imitate bus reprogramming through UART
from crccheck.crc import Crc16CcittFalse as crc16
import serial
import signal
import sys

port = 'COM8'
blob_len = 512
sync = bytearray([0x35, 0x2E, 0xF8, 0x53])

def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    sys.exit(0)

def create_pkt(data_len):
    buf = bytearray()
    buf.extend(sync)
    buf.append(0x02)
    buf.append(0x00)
    buf.append(0x00)
    buf.append(0x00)
    buf.append(((data_len+1) >> 8) & 0xFF)
    buf.append((data_len+1) & 0xFF)
    buf.extend(bytearray([0]*data_len))
    crc = crc16.calc(buf)
    buf.append((crc >> 8) & 0xFF)
    buf.append(crc & 0xFF)
    print(len(buf))
    return buf

def process_pkt(uart):
    sync_index = 0
    while (sync_index < 4):
        b = uart.read(1)
        if(b[0] == sync[sync_index]):
            sync_index += 1
        elif (b[0] == sync[0]):
            sync_index = 1
        else:
            sync_index = 0

    header = uart.read(6)
    apid = ((header[0] << 8) & 0x7) | header[1]
    pkt_len = ((header[4] << 8) | header[5]) + 1
    data = uart.read(pkt_len)
    return apid, data

def read_pkts(uart):
    while True:
        apid, data = process_pkt(uart)
        if apid == 0:
            print("[VNC2L] " + data.decode("utf-8"))
        else:
            print("[" + "0x{:03X}".format(apid) + "]")
        if apid == 0x220:
            break

if __name__ == '__main__':
    uart = serial.Serial(port, 921600)
    uart.write(create_pkt(blob_len))
    print('Awaiting response...')
    read_pkts(uart)
