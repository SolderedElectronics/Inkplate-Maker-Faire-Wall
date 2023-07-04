# This script is used to batch upload Inkplate sketches on many USB ports
# It doesn't happen simultaneously in parallel, but it's fast enough
# Really useful if you need to change something in the main sketch and quickly upload it

# Import the required libraries
import sys
import glob
import serial
import subprocess
import multiprocessing

# Get the list of all the serial ports
# Taken from https://stackoverflow.com/questions/12090503/listing-available-com-ports-with-python
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    
    print("NUM PORTS: "+str(len(result)))
    return result

# Upload code to the serial port
# The 'original string' is what your command with esptool looks like in arduino IDE when you upload the sketch
# Just copy and paste it in 'original_string' and this function will replace the port to the given one and run the command

def upload(port):
    original_string = r'"C:\Users\Test\AppData\Local\Arduino15\packages\Inkplate_Boards\tools\esptool_py\4.2.1/esptool.exe" --chip esp32 --port "COM13" --baud 921600  --before default_reset --after hard_reset write_flash  -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 "c:\repos\Inkplate-Maker-Fare-Wall\build/inkplate_makerfaire_wall.ino.bootloader.bin" 0x8000 "c:\repos\Inkplate-Maker-Fare-Wall\build/inkplate_makerfaire_wall.ino.partitions.bin" 0xe000 "C:\Users\Test\AppData\Local\Arduino15\packages\Inkplate_Boards\hardware\esp32\6.0.0/tools/partitions/boot_app0.bin" 0x10000 "c:\repos\Inkplate-Maker-Fare-Wall\build/inkplate_makerfaire_wall.ino.bin"'
    command = original_string.replace("COM13", port)
    subprocess.run(command)

# Get all the serial ports, and create a multiprocessing object which uploads the sketch in parallel
if __name__ == '__main__':
    serial_ports()
    inkplates = serial_ports()
    pool_obj = multiprocessing.Pool()
    ans = pool_obj.map(upload,inkplates)
    print(ans)
    pool_obj.close()