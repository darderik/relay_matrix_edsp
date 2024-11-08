function openComPort {
    Param(
        [int]$number,
        [int]$baud
    )

    $port = New-Object System.IO.Ports.SerialPort("COM$number", $baud, 'None', 8, 'One')
    if ($port.IsOpen) {
        $port.Close()
        Write-Host "Porta COM3 chiusa prima di riaprirla."
    }
    $port.Open()
    return $port
}
$CommandList = @(
    "switch:commute:reset:all",
    "switch:commute c3",
    "switch:commute d4",
    "switch:commute:reset c3",
    "switch:commute:reset d4",
    "switch:commute c4",
    "switch:commute d3"
)
#Setup della COM3
#Baudrate = 9600, Terminator: \r\n, Encoding: ASCII, Databits: 8, Parity: None, Stopbits: 1
[System.IO.Ports.SerialPort] $SerialPort = openComPort 3 115200
$SerialPort.Encoding = [System.Text.Encoding]::ASCII
    

try {
    while ($true) {

        foreach ($cmd in $CommandList) {
            $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
            $SerialPort.WriteLine($cmd)
            while (-not $SerialPort.ReadExisting() -contains "|CTS|") {}
            # Received CTS
            $stopwatch.Stop()
            Write-Host "Tempo trascorso: $($stopwatch.Elapsed.TotalMilliseconds) ms"
            Start-Sleep -Milliseconds 100
        }
    }
}
finally {
    $SerialPort.Close()
}