# This is the script for running the laptop which sends serial data to the displays and also has the push button connected
# This is considered the 'main' laptop
# Connect this laptop to the secondary one via a single Ethernet cable so they can communicate
# First start the secondary_slideshow_laptop.py script on the second laptop, and then start this one

# Import the required libraries
import socket
import serial
import time

import serial.tools.list_ports

# Send a message over etherenet
def send_message(message, ip_address, port):
    # Create a socket object
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to the receiver's IP address and port
        sock.connect((ip_address, port))
 
        # Send the message
        sock.sendall(message.encode())
        print("Message sent successfully.")

        sock.close()
 
# Set the receiver's IP address and port
receiver_ip = '169.254.48.66'  # Replace with the actual IP address of PC 2
receiver_port = 12345        # Replace with the port number of PC 2

# Set the pushbutton's port and make the serial object
button_port = 'COM74'
ser = serial.Serial(
   port=button_port,
   baudrate=115200,
   stopbits=serial.STOPBITS_ONE,
   bytesize=serial.EIGHTBITS,
   parity=serial.PARITY_NONE
)

# The inkplates are all the serial ports except the button port
serial_ports = [port.device for port in serial.tools.list_ports.comports()]
inkplate_ports = [port for port in serial_ports if port != button_port]
inkplates = [serial.Serial(port, baudrate=115200) for port in inkplate_ports]

# Variable to remember what we're showing on screen
currentThingOnScreen = 1

# Essentially, this is just a large switch-case to show the correct image on Inkplates
# All we have to do is send the filename over serial
# Some basic error handling is implemented, so we can see which port is broken
# The filenames here are the filenames of our cropped images
def show_on_inkplates(currentThingOnScreen):
    if(currentThingOnScreen == 1):
        for inkplate in inkplates:
            try:
                inkplate.write('car'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 2):
        print("castle")
        for inkplate in inkplates:
            try:
                inkplate.write('castle'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 3):
        print("earth")
        for inkplate in inkplates:
            try:
                inkplate.write('earth'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 4):
        print("illusion")
        for inkplate in inkplates:
            try:
                inkplate.write('illusion'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 5):
        print("islands")
        for inkplate in inkplates:
            try:
                inkplate.write('islands'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 6):
        print("landscape")
        for inkplate in inkplates:
            try:
                inkplate.write('landscape'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 7):
        print("landscape2")
        for inkplate in inkplates:
            try:
                inkplate.write('landscape2'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 8):
        print("soldered")
        for inkplate in inkplates:
            try:
                inkplate.write('soldered'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 9):
        print("solderedmaker")
        for inkplate in inkplates:
            try:
                inkplate.write('solderedmaker'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 10):
        print("vienna")
        for inkplate in inkplates:
            try:
                inkplate.write('vienna'.encode())
            except:
                print("can't print on "+inkplate.port)
    if(currentThingOnScreen == 11):
        print("wildlife")
        for inkplate in inkplates:
            try:
                inkplate.write('wildlife'.encode())
            except:
                print("can't print on "+inkplate.port)
    

# When the sketch is started, show the car image 
send_message("next", receiver_ip, receiver_port)
show_on_inkplates(currentThingOnScreen)
currentThingOnScreen += 1
if(currentThingOnScreen == 12): currentThingOnScreen = 1

# Keep reading the button input and change the image on press
try:
    while True:

        startMeasure = time.time()
        print(startMeasure)
        while True:
            
            if ((time.time() - startMeasure) > 60):
                print("Next!")
                send_message("next", receiver_ip, receiver_port) # Also, notify the other laptop to change the picture
                show_on_inkplates(currentThingOnScreen)
                currentThingOnScreen += 1
                if(currentThingOnScreen == 12): currentThingOnScreen = 1
                break
            
            # Read data from the serial port
            data = ser.readline().decode('utf-8').strip()

            # If data received, print it
            if data:
                print(data)
                send_message("next", receiver_ip, receiver_port)
                show_on_inkplates(currentThingOnScreen)
                currentThingOnScreen += 1
                if(currentThingOnScreen == 12): currentThingOnScreen = 1
                break
            

# To close the serial port gracefully, use Ctrl+C to break the loop
except KeyboardInterrupt:
    print("Closing the serial port.")