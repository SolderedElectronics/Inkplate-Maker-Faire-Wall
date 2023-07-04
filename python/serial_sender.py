# Send the same serial command to all inkplates
# For a list of all the useful commands, check the README

import serial
import serial.tools.list_ports

serial_ports = [port.device for port in serial.tools.list_ports.comports()]
inkplates = [serial.Serial(port, baudrate=115200) for port in serial_ports]
for inkplate in inkplates:
    # Change here
    inkplate.write('OP_MODE_RESET'.encode())