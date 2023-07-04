# This script runs on the secondary laptop
# It gets data from the main one via ethernet, upon recieving data it changes the image

import socket
import serial.tools.list_ports
import time

# Function which handles recieving a message over ethernet
def receive_message(port):
    # Create a socket object
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Bind the socket to a specific IP address and port
        sock.bind(('169.254.48.66', port))
        # Listen for incoming connections
        sock.listen(1)

        print("Waiting for a connection...")

        # Accept a connection from the sender
        conn, addr = sock.accept()

        with conn:
            print('Connected by', addr)
            # Receive data from the sender
            data = conn.recv(1024).decode()
            conn.close()
            sock.close()
            # Return it
            return data


# Set the receiver's port number
receiver_port = 12345

# When the script is started, the first image shown is #1
currentThingOnScreen = 1

# Get all the serial ports
serial_ports = [port.device for port in serial.tools.list_ports.comports()]
inkplates = [serial.Serial(port, baudrate=115200) for port in serial_ports]

# Function that maps the current number of the image to the filename on the SD card
def show_on_inkplates(currentThingOnScreen):
    if (currentThingOnScreen == 1):
        print("imagefrme mode car")
        for inkplate in inkplates:
            inkplate.write('car'.encode())
    if (currentThingOnScreen == 2):
        print("castle")
        for inkplate in inkplates:
            inkplate.write('castle'.encode())
    if (currentThingOnScreen == 3):
        print("earth")
        for inkplate in inkplates:
            inkplate.write('earth'.encode())
    if (currentThingOnScreen == 4):
        print("illusion")
        for inkplate in inkplates:
            inkplate.write('illusion'.encode())
    if (currentThingOnScreen == 5):
        print("islands")
        for inkplate in inkplates:
            inkplate.write('islands'.encode())
    if (currentThingOnScreen == 6):
        print("landscape")
        for inkplate in inkplates:
            inkplate.write('landscape'.encode())
    if (currentThingOnScreen == 7):
        print("landscape2")
        for inkplate in inkplates:
            inkplate.write('landscape2'.encode())
    if (currentThingOnScreen == 8):
        print("soldered")
        for inkplate in inkplates:
            inkplate.write('soldered'.encode())
    if (currentThingOnScreen == 9):
        print("solderedmaker")
        for inkplate in inkplates:
            inkplate.write('solderedmaker'.encode())
    if (currentThingOnScreen == 10):
        print("vienna")
        for inkplate in inkplates:
            inkplate.write('vienna'.encode())
    if (currentThingOnScreen == 11):
        print("wildlife")
        for inkplate in inkplates:
            inkplate.write('wildlife'.encode())

# The main loop, when a message is recieved, go to the next image
while (True):
    recieved = receive_message(receiver_port)
    if (recieved):
        print(str(currentThingOnScreen))
        show_on_inkplates(currentThingOnScreen)
        currentThingOnScreen += 1
        if (currentThingOnScreen == 12):
            currentThingOnScreen = 1
