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
    "test:cmd:1 ok ok",
    "test:cmd:2 ok ok",
    "test:cmd:3 ok ok"
)
#Setup della COM3
#Baudrate = 9600, Terminator: \r\n, Encoding: ASCII, Databits: 8, Parity: None, Stopbits: 1
[System.IO.Ports.SerialPort] $SerialPort = openComPort 3 9600
$SerialPort.Encoding = [System.Text.Encoding]::ASCII

try {
    foreach ($cmd in $CommandList) {
        $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
        $SerialPort.WriteLine($cmd)
        while (-not $SerialPort.ReadExisting() -contains "|CTS|") {}
        $stopwatch.Stop()
        Write-Host "Tempo trascorso: $($stopwatch.Elapsed.TotalMilliseconds) ms"
        #Received CTS
    }
}
finally {
    $SerialPort.Close()
}