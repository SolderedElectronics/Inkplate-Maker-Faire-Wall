# Send the same serial command to all inkplates
# For a list of all the useful commands, check the README

# Import the required libraries
import serial
import serial.tools.list_ports

# Get a list of all the serial ports and create an Inkplate object for each one of them
serial_ports = [port.device for port in serial.tools.list_ports.comports()]
inkplates = [serial.Serial(port, baudrate=115200) for port in serial_ports]
for inkplate in inkplates:
    # Change the command you want to run here
    inkplate.write('OP_MODE_RESET'.encode())