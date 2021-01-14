# Windows Makefile, needs Vinculum II Toolchain V2.0.0-SP1
$name = "click"
$configuration = "Release"
$files = @("main", "dev_conf", "spi_handler", "uart_handler", "usb_handler", "packets", "crc")
$libs = @("kernel.a", "USBHost.a", "UART.a", "SPISlave.a", "BOMS.a")

$objects = @()
$rebuilt = 0

New-Item -ItemType Directory -Force -Path $configuration | Out-Null

foreach ($file in $files) {
    $date_c = (Get-Item "$file.c").LastWriteTime
    $date_h = $date_c
    if (Test-Path "$file.h" -PathType Leaf) {
        $date_h = (Get-Item "$file.h").LastWriteTime
    }
    if (Test-Path "$configuration\$file.obj" -PathType Leaf) {
        $date_obj = (Get-Item "$configuration\$file.obj").LastWriteTime
        if (($date_obj -ge $date_c) -and ($date_obj -ge $date_h)) {
            $objects += "$configuration\$file.obj"
            continue
        }
    }
    VinC.exe -c -d 0 -O 4 -o "$configuration\$file.asm" "$file.c"
    VinAsm.exe -d 0 -o "$configuration\$file.obj" "$configuration\$file.asm"
    $objects += "$configuration\$file.obj"
    $rebuilt++
}

if ($rebuilt -gt 0) {
    VinL.exe -d 0 -O -o $configuration\$name @objects -U @libs
    scp $configuration\$name.rom pi@18.18.33.36:/home/pi/vnclFlash/CLICK.rom
} else {
    Write-Output "Up to date"
}
