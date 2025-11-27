<#
"switch:commute:reset:all",
"switch:commute a1 a2 a3 a4",
"switch:commute d1 d2 d3 d4",
"switch:commute:reset:all",
"switch:commute c1 c2 c3 c4",
"switch:commute b1 b2 b3 b4"

"switch:commute a1",
"switch:commute a2",
"switch:commute a3",
"switch:commute a4",
"switch:commute b1",
"switch:commute b2",
"switch:commute b3",
"switch:commute b4",
"switch:commute c1",
"switch:commute c2",
"switch:commute c3",
"switch:commute c4",
"switch:commute d1",
"switch:commute d2",
"switch:commute d3",
"switch:commute d4"

#>


function openComPort {
    Param(
        [int]$number,
        [int]$baud
    )

    $port = New-Object System.IO.Ports.SerialPort("COM$number", $baud, 'None', 8, 'One')
    if ($port.IsOpen) {
        $port.Close()
        Write-Host "COM3 port closed before reopening."
    }
    $port.Open()
    return $port
}
$CommandList = @(
"switch:commute:reset:all",
"switch:commute a1",
"switch:commute a2",
"switch:commute a3",
"switch:commute a4",
"switch:commute b1",
"switch:commute b2",
"switch:commute b3",
"switch:commute b4",
"switch:commute c1",
"switch:commute c2",
"switch:commute c3",
"switch:commute c4",
"switch:commute d1",
"switch:commute d2",
"switch:commute d3",
"switch:commute d4"
)
#Setup of COM3
[System.IO.Ports.SerialPort] $SerialPort = openComPort 11 115200
$SerialPort.Encoding = [System.Text.Encoding]::ASCII
    

try {
    while ($true) {

        foreach ($cmd in $CommandList) {
            $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
            $SerialPort.WriteLine($cmd)
            while (-not $SerialPort.ReadExisting() -contains "OK") {}
            # Received CTS
            $stopwatch.Stop()
            Write-Host "Elapsed time: $($stopwatch.Elapsed.TotalMilliseconds) ms"
            Start-Sleep -Seconds 2
        }
        Start-Sleep -Seconds 5
    }
}
finally {
    $SerialPort.Close()
}
