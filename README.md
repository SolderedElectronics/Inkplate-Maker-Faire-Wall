#  Inkplate Maker Faire Wall

![Inkplate Maker Faire Wall](https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Maker-Faire-Wall/main/images/makerfairewall.JPG)

As seen at Maker Faire Vienna 2023, the Inkplate Make Faire Wall is a large display made from *60* Inkplate 10's!. Yes, 60.
It's ~2.24 meters wide and ~1 meter tall. The total resolution is 13170x5630 px.

It has pre-loaded images on SD cards and changes the image at the press of a button.

Everything which is required to make it run is included in this repository. You will need:
* 60 Inkplate 10's with batteries
* The 3D printed enclosure which holds the Inkplates together
* 60 MicroSD cards (with enough space to contain all the images you want displayed)
* Enough USB Hubs to connect 60 Inkplates via serial
* A Computer (or two) to run the show (more info below)

## How it works:

### Schematic
Below is the schematic of how everything is connected:
![Inkplate Maker Faire Wall Schematic](https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Maker-Faire-Wall/main/images/inkplate_wall_schematic.png)

### The Sketch
All Inkplates are running the same sketch (inkplate_makerfaire_wall.ino). This sketch is used to recieve commands via serial to switch the operation mode and also to configure each Inkplate. The configuration process involves defining which display on the wall they represent (00, 01, 02... from left to right, top to bottom. On the bottom right is display 59). This is then stored in EEPROM. This way, each Inkplate knows the segment of the image (and thus, the filename) that it has to show. Then, sending the root filename of the image via serial to each Inkplate will trigger that image being shown on them.

Here are the different operational modes implemented, to swtich all the Inkplates to a certain mode, send the name of the operational mode over Serial.
| Operational mode name  | Description |
| ------------- | ------------- |
| OP_MODE_SLEEP  | Put all the Inkplates to sleep, useful for charging them.  |
| OP_MODE_IMAGEFRAME  | Put the inkplates into the image frame mode. While Inkplates are in this mode, sending the root filename (eg. 'car') over serial opens car_xx.jpg from the SD card, xx being the number assigned to this display over configuration. |
| OP_MODE_WAIT  | Just wait for the next command, do nothing. |
| OP_MODE_VCOM  | Goes to set the VCOM of the Inkplate. This is useful to get the best looking image.  |
| OP_MODE_RESET  | Reset the Inkplate  |

### The computers
So, why two laptops? Let's just say there are difficulties when you try to connect 60 COM ports to a Raspberry Pi or Windows PC. Theoretically this would be supported, but we ended up having to split the COM ports between two laptops - one 'main' and one 'secondary'. Each of them has 30 Inkplate 10's connected via a USB hub. The 'main' laptop also has the Button on another Serial port. The two laptops are then connected together via Ethernet for fast communication. The communication between the two laptops basically involves the first laptop telling the other 'go to the next image'.

### Batteries
Batteries are required. Inkplate 10 draws a bit more current when it's refreshing. This quickly adds up, so the USB Hubs can't provide enough power. This is why the Inkplates need to have a battery. In order for the display to work - the battery has to be charged, as it automatically compensates for the current draw when the screen is being refreshed. The best way to charge the battery is to connect everything and then send OP_MODE_SLEEP via the serial_sender.py script to put all the ESP's to sleep - allowing for the battery to be charged.

### Image files
Image files to display have to be loaded on the SD card. Crop them using the inkplate-wall-image-cropper tool included in this repo. The images to input to the tool must be exactly 13170 x 5630px, .jpg works best. To find good images search for 'gigapixel images' or use Google advanced image search, find the image you want and crop to fit using your favorite image editing program. All of the cropped image outputs from the tool need to be in the (SD Card root)/wall_img directory. So, this directory will be full of many images like 00_image1.jpg, 32_solderedLogo.jpg, 14_test.jpg, 02_image2.jpg etc. You don't need to delete images which are not for this particular display to save time, the right one will be loaded via the identifier.

### Python scripts
This is the list of Python scripts used and what they do:
| Script name  | Description |
| ------------- | ------------- |
| serial_sender.py  | Used for diagnostics and testing, sends a single line over Serial to all ports. |
| num_ports.py  | Just prints the number of ports connected via Serial to the current computer. Each Computer needs to have 30 ports connected via Serial, so this quickly checks if everything is connected well.  |
| batch_uploader.py | Uploads a pre-compiled ESP32 .bin file to all the Serial ports. Useful if something in the main sketch needs to be changed. |
| inkplate_wall_image_cropper.py  | Crops a 13170x5630 image file to 60 different files with some spacing in between. |
| main_slideshow_laptop.py  | Runs on the main laptop  |
| secondary_slideshow_laptop.py  | Runs on the second laptop  |

### The button
The button is actually a Dasduino which sends a string via Serial upon press of a button. It's running the simple push_button.ino sketch.

### 3D printed case
You can find the files for 3D printing the casing in the 'case' folder. This design enables modular connecting of Inkplate 10's side-by-side in any dimension. The practicality of this design made it possible to dissasemble the panels and transport them and also enabled us to easily repair the display as a whole.

### If you want to make this
Contact us! We'd be glad to help.

### About Soldered

<img src="https://raw.githubusercontent.com/SolderedElectronics/Soldered-Generic-Arduino-Library/dev/extras/Soldered-logo-color.png" alt="soldered-logo" width="500"/>

At Soldered, we design and manufacture a wide selection of electronic products to help you turn your ideas into acts and bring you one step closer to your final project. Our products are intented for makers and crafted in-house by our experienced team in Osijek, Croatia. We believe that sharing is a crucial element for improvement and innovation, and we work hard to stay connected with all our makers regardless of their skill or experience level. Therefore, all our products are open-source. Finally, we always have your back. If you face any problem concerning either your shopping experience or your electronics project, our team will help you deal with it, offering efficient customer service and cost-free technical support anytime. Some of those might be useful for you:

- [Web Store](https://www.soldered.com/shop)
- [Tutorials & Projects](https://soldered.com/learn)
- [Community & Technical support](https://soldered.com/community)

## Have fun!

And thank you from your fellow makers at Soldered Electronics.
