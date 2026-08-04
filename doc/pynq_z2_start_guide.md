# PYNQ-Z2 Quick Start Guide
<img src="images/pynq_z2.png" width="500em"/><br>
## Overview

The **PYNQ-Z2** is a development board based on the **Xilinx Zynq-7000 SoC (XC7Z020)**, combining:

- Dual-core ARM Cortex-A9 Processing System (PS)
- FPGA Programmable Logic (PL)
- DDR3 memory
- Ethernet connectivity
- USB interfaces
- Arduino and Raspberry Pi compatible headers
- General-Purpose I/O (GPIO)

The board is commonly used for embedded Linux, FPGA development and hardware acceleration.



## First Boot
A jumper located on the board permits the selection of the boot mode.

<img src="images/pynq_z2_boot.png" width="500em"/><br>

Before powering on the board :

1. Insert the prepared microSD card.
2. Choose the SD boot mode with the jumper.
3. Configure the power selection jumper.
4. Connect power.
5. Power on the board.
6. Wait for Linux to finish booting.

There is a blinking led on the board when you power it on, it doesn't mean linux is booted or CPU is runing ! It just means that FPGA circuit is working properly. (That LED is blinking with internal FPGA programm and not linked to anything related to CPU)

⚠️ Be extra delicate when inserting the SD card, the SD card reader is fragile ! 



## Powering the Board

<img src="images/pynq_z2_power.png" width="500em"/><br>


To choose the power source of the PYNQ-Z2 you can move the jumper to select the source. It can be powered using either:
- **USB** : up position → Power from Micro-USB
- **REG** : down position → Power from 12V barrel jack or Vin pin

Verify the jumper position before turning on the board.

⚠️ If more power is required the regulator is recommended. 


## Connecting to the Board

Possible connection types :
- Serial Console
- Ethernet / SSH Access

The easiest way to monitor boot messages and access the Linux console is through the serial interface. But the ssh offers the possibility to connect to the board remotely (to avoid damaging it or to share it with multiple clients remotely). If using software like MobaXterm ssh enables SSH browser : a neat way of transfering files to or from the board. 

Examples of terminal software:

- MobaXterm
- PuTTY
- screen

### Serial

```text
Baud rate: 115200
Data bits: 8
Parity: None
Stop bits: 1
Flow control: None
```

### Ethernet / SSH Access

The board is configured with the following static IP address:

```text
IP Address : 192.168.0.10
Netmask    : 255.255.255.0
```

#### Configure Your Computer

Set a static IP address on your computer in the same subnet:

```text
IP Address : 192.168.0.x
Netmask    : 255.255.255.0
```

Where:

- `x` must be between `1` and `254`
- Do **not** use `10` (already used by the board)

Example:

```text
IP Address : 192.168.0.100
Netmask    : 255.255.255.0
```

#### Connect through ssh

Connect the board directly to your computer using an Ethernet cable.

With the tool of your choice connect to the board :

```text
IP Address : 192.168.0.10
port : 22
```



## Login Credentials

### Root

```text
Username: root
Password: <none>
```

### User

```text
Username: user
Password: user
```

## GPIO Pinout

⚠️ **All GPIO signals are 3.3V only.**

Applying higher voltages may permanently damage the board.

GPIO documentation and pinout reference:

- See the GPIO documentation available in this repository `gpio_readme`.
- Official board documentation also contains detailed pin mappings for:
  - Arduino Header
  - Raspberry Pi Header
  - PMOD Connectors



## Useful Commands

Check network configuration:

```bash
ip addr
```

Verify Ethernet connectivity:

```bash
ping 192.168.0.10
```


---

## Notes

- Use the serial console when debugging boot issues.
- Ensure the microSD card is properly inserted before power-up → ⚠️ SD card reader fragile ! 
- Verify the power source jumper setting before turning on the board.
- ⚠️ GPIOs operate at **3.3V logic levels only**.
- When using Ethernet, your computer must be configured in the `192.168.0.0/24` network.