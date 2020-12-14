# Test write UART to VNC2L
$value = [byte](0xCA)

$port = new-Object System.IO.Ports.SerialPort COM6,921600,None,8,one
$port.open()
$port.Write($value)
$port.Close()
