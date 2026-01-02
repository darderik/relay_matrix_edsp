# STM32 Relay Matrix Control System

A crosspoint relay matrix controller based on the STM32F401 microcontroller. This project implements SCPI-like commands to control a 4x4 relay matrix using TPL9201 drivers. It is designed for anisotropy measurement and equipment testing applications and was developed for the course "Elettronica dei Sistemi Programmabili".

## 🔧 Features

- **Crosspoint Matrix**: True 4x4 crosspoint switching - connect any input (A-D) to any output (1-4)
- **TPL9201 Drivers**: SPI-controlled MOSFET arrays for relay driving
- **Dual Relay Types**: Interchangeable footprints for HFD2/012-S-L2 (EMR) and OMRON G3MB-202P (SSR)
- **ATX PSU Integration**: Uses ATX power supply for 12V relay power and PS_ON control
- **Power Management**: Basic PSU control with power monitoring
- **Status Monitoring**: Simple relay status reporting
- **Command Queuing**: Basic queuing system for command execution
- **USB Serial Interface**: PC connectivity for control

## 🏗️ Hardware Architecture

### Microcontroller
- **STM32F401XE** - ARM Cortex-M4 with FPU
- **84 MHz** maximum operating frequency
- **512KB Flash** / **96KB RAM**

### Relay Matrix Configuration
- **4x4 Crosspoint Matrix**: Connect any input (A-D) to any output (1-4)
- **TPL9201 SPI Drivers**: MOSFET arrays controlled via SPI interface
- **Relay Options**: HFD2/012-S-L2 (EMR) or OMRON G3MB-202P (SSR)
- **Interchangeable Footprints**: Choose relay type during PCB assembly

### Pin Configuration
```
Power Control:
- PS_ON  → PC9 (ATX Power Supply Enable)
- PWR_OK → PB5 (ATX Power Supply Status)

TPL9201 SPI Control:
Group A: NCS→PA12, NRST→PB12
Group B: NCS→PA9,  NRST→PB9
Group C: NCS→PB6,  NRST→PB14
Group D: NCS→PC5,  NRST→PC6
```

## 📋 Command Reference

### Basic Relay Control
```
SWITCH:COMMUTE <relay>           # Activate specific relay
SWITCH:COMMUTE:EXCLUSIVE <relay> # Activate relay, deactivate others in same group
SWITCH:COMMUTE:RESET <relay>     # Deactivate specific relay
SWITCH:COMMUTE:RESET:ALL         # Deactivate all relays
```

### Status Queries
```
SWITCH:COMMUTE:STATUS?           # Get power status of all groups
SWITCH:COMMUTE:BYTE:STATUS?      # Get detailed binary status
*IDN?                           # Device identification
*OPC?                           # Operation complete
SYS:LOG?                        # System log messages
SYS:GETSTATE?                   # Current system state
```

### System Control
```
SYS:HALT                        # Stop system - power down
```

### Relay Addressing Format
Relays are addressed using the format: `<input><output>`
- **Inputs**: a, b, c, d (case insensitive) 
- **Outputs**: 1, 2, 3, 4

**Examples:**
- `a1` - Connect input A to output 1
- `b3` - Connect input B to output 3  
- `d4` - Connect input D to output 4

**Crosspoint Behavior:**
- `a1 a2` - Connects A to both 1 and 2 (thus 1↔2)
- `a3 b2 c1 d3` - Multiple connections: A↔3, B↔2, C↔1, D↔3 (so A↔D via 3)

### Multi-Relay Commands
Multiple relays can be controlled in a single command for complex routing:
```
SWITCH:COMMUTE a1 b2 c3 d4       # Connect A→1, B→2, C→3, D→4
SWITCH:COMMUTE:RESET a1 a2       # Disconnect A from 1 and 2
```

## 🚀 Getting Started

### Prerequisites
- VS Code
- **STM32Cube for VS Code** extension (STMicroelectronics)
- STM32CubeMX (installed locally; used by the extension to open `RelayMatrix.ioc` and regenerate code)
- ARM GNU Toolchain (`arm-none-eabi-gcc`), CMake, and Ninja
- OpenOCD + ST-LINK (for flashing/debug)

> Note: This repository already contains CubeMX-generated sources (`Core/`, `Drivers/`) and a CMake build that consumes them via `cmake/stm32cubemx`. If you change the hardware configuration (clocks/pinout/peripherals), regenerate code from the `.ioc` and keep the default output folders (or update `cmake/stm32cubemx/CMakeLists.txt` accordingly).

### Recommended VS Code Workflow (STM32Cube)

1. Install the **STM32Cube for VS Code** extension.
2. In VS Code settings, point the extension to your local STM32CubeMX installation (and optional tools like STM32CubeProgrammer/OpenOCD if the extension requests paths).
3. Open this folder/workspace.
4. Open `RelayMatrix.ioc` and use the STM32Cube actions to **Generate Code** when needed.

### Building

This project builds with CMake/Ninja. The workspace already provides ready-to-run VS Code tasks:

- `Ctrl+Shift+P` → **Tasks: Run Task** → **Build: CMake Debug**
- `Ctrl+Shift+P` → **Tasks: Run Task** → **Build: CMake (default preset)**
- `Ctrl+Shift+P` → **Tasks: Run Task** → **Build: Ninja default**
- `Ctrl+Shift+P` → **Tasks: Run Task** → **Build: Ninja (build folder)**

If you prefer the command line, you can also use CMake presets (see `CMakePresets.json`).

### Flashing

Use the provided OpenOCD task:

- `Ctrl+Shift+P` → **Tasks: Run Task** → **Flash: OpenOCD (flash-only)**

This programs `debug/RelayMatrix.elf` using `openocd.cfg`.


### Hardware Setup
1. Insert STM32F401RE Nucleo board into the custom PCB sockets (2x 20-pin headers, 2.54mm pitch)
2. Connect ATX power supply: PS_ON control line and 12V for relay power
3. Connect USB to Nucleo for serial communication
4. Choose and mount either HFD2/012-S-L2 (EMR) or OMRON G3MB-202P (SSR) relays during PCB assembly

**Assembly Notes:**
- The only valued components are the capacitors and resistors around the TPL9201 driver
- Make sure to solder the TPL9201 with the first pin aligned to the silkscreen mark
- See images section below for TPL9201 component values and PCB layout

### Serial Communication
- Baud Rate: 115200
- Data Bits: 8
- Stop Bits: 1
- Parity: None
- Flow Control: None
- Terminator: `\n` (configurable)

## 💻 Usage Examples

### Python Control Script
```python
import serial

ser = serial.Serial('COM3', 115200, timeout=1)

# Activate relay A1
ser.write(b'SWITCH:COMMUTE a1\n')
response = ser.readline()

# Get status
ser.write(b'SWITCH:COMMUTE:STATUS?\n')
status = ser.readline().decode().strip()

# Emergency stop
ser.write(b'SYS:HALT\n')
```

### PowerShell Control
See `Powershell/comms.ps1` for communication examples.

## 🔧 Configuration

### Relay Mode Selection
Edit `Core/Src/components/config/config.h`:
```c
#define SWITCH_COMMUTE_MODE SWITCH_EMR  // for HFD2/012-S-L2
// or
#define SWITCH_COMMUTE_MODE SWITCH_SSR  // for OMRON G3MB-202P
```

### Communication Settings
```c
#define HANDSHAKE_SCPI 1          // Enable SCPI handshaking
#define TERM_CHAR "\n"            // Command terminator
#define MAX_COMMAND_LENGTH 256    // Maximum command length
```

### Queue Configuration
```c
#define MAX_COMMAND_QUEUE_SIZE 16 // Command buffer size
#define ACTION_QUEUE_SIZE 16      // Action buffer size
```

## 🏛️ Project Structure

### Core Components
- **Parser**: SCPI-like command parsing and validation
- **Interpreter**: Command execution and relay matrix control
- **FSM**: System state management (PASSIVE/ACTIVE/ERROR)
- **Queues**: Command and message management
- **Actions**: TPL9201 SPI control and relay matrix logic
- **Callbacks**: UART interrupt handlers for serial communication

### State Machine
- **PASSIVE**: Low power, PSU off
- **ACTIVE**: Normal operation, PSU on
- **ERROR**: Fault condition handling

### Communication Flow
```
UART RX → Parser → Command Queue → Interpreter → TPL9201 SPI → Relay Matrix
                                                       ↓
Response Queue ← Status Monitor ← Matrix State ←——————————————————
```

## 🐛 Troubleshooting

### Common Issues
- **No Response**: Check serial connection and baud rate
- **Command Error**: Verify command syntax and relay addressing
- **Power Issues**: Check ATX PSU connections and PWR_OK signal  
- **Relay Not Switching**: Verify TPL9201 SPI connections and 12V supply

### Debug Features
- System log accessible via `SYS:LOG?`
- Status monitoring with `SYS:GETSTATE?`
- Basic self-test capabilities

## 📸 Images

### TPL9201 Component Values
Component values for the relay driving module around the TPL9201 driver:

<img width="563" height="381" alt="Relay Driving module" src="https://github.com/user-attachments/assets/66ce1e1b-c05f-42fd-a2a6-b0c6a30d4fbe" />

### PCB Layout View
Complete PCB layout showing the 4x4 relay matrix design:

<img width="7120" height="3840" alt="PCB Layout - Relay Matrix 4x4" src="https://github.com/user-attachments/assets/06360b66-cf84-4ccb-a786-7c04ecf8cf1a" />

### 3D PCB Render
3D visualization of the assembled PCB:

<img width="948" height="487" alt="3D PCB Render" src="https://github.com/user-attachments/assets/f3b57e82-f0c8-4ac7-bc0b-ef88ce001bf5" />

## 📄 License

This project is licensed under the terms specified in the LICENSE file.

## 👨‍💻 Author

**darderik** - DIIEM, Università Roma Tre  

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

---