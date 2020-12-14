# Test write UART to VNC2L
$len = 1
if ($args.Count -gt 0) {
    $len = $args[0] -as [int]
}

Write-Output "Sending $len"

$data = [System.Byte[]]::new($len)

$Random = [System.Random]::new()
$Random.NextBytes($data)

$port = new-Object System.IO.Ports.SerialPort COM8,921600,None,8,one
$port.open()
$port.Write($data, 0, $len)
$port.Close()
