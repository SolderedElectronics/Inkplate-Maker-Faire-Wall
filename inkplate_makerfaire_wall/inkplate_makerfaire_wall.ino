/**
 **************************************************
 *
 * @file        inkplate_makerfaire_wall.ino
 * @brief       This is the main sketch for the Inkplate 10 Wall, as seen on Maker Faire in Vienna in 2023
 *
 *              Details on how it all works are in the README
 *
 *              Inkplate 10 with battery: solde.red/333247
 *
 * @authors     Robert @ soldered.com
 ***************************************************/

// Include needed libraries
#include "Inkplate.h"
#include "EEPROM.h"

#include "soldered_logo.h"

// EEPROM location where the config is written
#define WALL_CONFIG_EEPROM_ADDR 180

// Define the timeout in seconds for configuring
#define CONFIG_TIMEOUT_SECONDS 10

// Define the folder name where all the image files are located
const char *folderName = "wall_img";

// VCOM value if it gets configured:
double vcomVoltage;

// All waveforms for Inkplate 10 boards
uint8_t waveform1[8][9] = {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0}, {0, 1, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 2, 2, 2, 2, 2, 2, 1, 0}, {0, 0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};
uint8_t waveform2[8][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 2, 1, 2, 1, 1, 0}, {0, 0, 0, 2, 2, 1, 2, 1, 0}, {0, 0, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 0, 2, 1, 1, 1, 2, 0}, {0, 0, 2, 2, 2, 1, 1, 2, 0}, {0, 0, 0, 0, 0, 1, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};
uint8_t waveform3[8][9] = {{0, 3, 3, 3, 3, 3, 3, 3, 0}, {0, 1, 2, 1, 1, 2, 2, 1, 0}, {0, 2, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 2, 2, 2, 2, 1, 0}, {0, 3, 3, 2, 1, 1, 1, 2, 0}, {0, 3, 3, 2, 2, 1, 1, 2, 0}, {0, 2, 1, 2, 1, 2, 1, 2, 0}, {0, 3, 3, 3, 2, 2, 2, 2, 0}};
uint8_t waveform4[8][9] = {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0}, {1, 1, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 1, 2, 2, 2, 2, 2, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};
uint8_t waveform5[8][9] = {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {2, 2, 2, 1, 0, 2, 1, 0, 0}, {2, 1, 1, 2, 1, 1, 1, 2, 0}, {2, 2, 2, 1, 1, 1, 0, 2, 0}, {2, 2, 2, 1, 1, 2, 1, 2, 0}, {0, 0, 0, 0, 2, 1, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};
uint8_t *waveformList[] = {&waveform1[0][0], &waveform2[0][0], &waveform3[0][0], &waveform4[0][0], &waveform5[0][0]};

// Calculate number of possible waveforms
uint8_t waveformListSize = (sizeof(waveformList) / sizeof(uint8_t *));

// Struct for reading waveform from EEPROM memory of ESP32
struct waveformData waveformEEPROM;

// Waveform 1 is index 0
// Waveform 2 is index 1
// Waveform 3 is index 2
// Waveform 4 is index 3
// Waveform 5 is index 4
int selectedWaveform = 0;

// Select operation mode. Imageframe is thje only one really fully implemented, so we're leaving that one here.
// More to come in the future!
enum OPERATION_MODE
{
    OP_MODE_SLEEP = 0,
    OP_MODE_IMAGEFRAME = 1,
    OP_MODE_WAIT = 4,
    OP_MODE_CLEAN = 7,
    OP_MODE_VCOM = 8,
    OP_MODE_RESET = 9
};

// Set to run as image frame on launch
OPERATION_MODE currentOpMode = OP_MODE_IMAGEFRAME;

// Create object on Inkplate library and set library to work in 3bit mode
Inkplate display(INKPLATE_3BIT);
bool isCurrentlySetTo3BitMode = true;

// The display's ID (which display it is on the grid)
char displayID[2];

void setup()
{
    // For communication
    Serial.begin(115200);

    // Start EEPROM
    EEPROM.begin(512);

    // Init display
    display.begin();

    // Get the config from serial
    int enteredConfiguration = 1;
    bool confWasSet = false;
    do
    {
        confWasSet = getConfigFromSerial(&enteredConfiguration);
    } while ((enteredConfiguration < 0 || enteredConfiguration > 59) && confWasSet);
    // If out of range, repeat

    if (confWasSet)
    {
        // Write configuration to EEPROM if it was set
        Serial.println("Writing to EEPROM");
        display.pinModeInternal(IO_INT_ADDR, display.ioRegsInt, 6, INPUT_PULLUP);
        EEPROM.write(WALL_CONFIG_EEPROM_ADDR, enteredConfiguration);
        EEPROM.commit();
    }

    // Load the configuration number
    int savedConfiguration = EEPROM.read(WALL_CONFIG_EEPROM_ADDR);
    Serial.print("Saved configuration: ");
    Serial.println(savedConfiguration);

    // Get the saved configuration represented as chars (to look for filenames and other things)
    sprintf(displayID, "%02d", savedConfiguration);

    Serial.print("Your display is #");
    Serial.println(displayID);

    display.clearDisplay();
    display.display();

    delay(1000);

    // Init SD card. Display if SD card has init. properly or not.
    if (!display.sdCardInit())
    {
        display.clearDisplay();
        display.println("Can't load Sd!");
        display.display();
    }
}

// In loop, act differently according to the current set mode
void loop()
{
    switch (currentOpMode)
    {
    case OP_MODE_SLEEP:
        // Go to sleep!
        esp_deep_sleep_start();
        break;

    case OP_MODE_IMAGEFRAME:
        operationAsImageFrame();
        break;

    case OP_MODE_WAIT:
        operationWaitForInstructions();
        break;

    case OP_MODE_CLEAN:
        cleanScreen();
        break;

    case OP_MODE_VCOM:
        setVCOM();

    case OP_MODE_RESET:
        // Reset to reconfigure!
        esp_restart();
        break;
    }
}

// Prompt user to enter a config number
bool getConfigFromSerial(int *configNum)
{
    int configNumber = 1;
    char serialBuffer[50];
    unsigned long serialTimeout;

    // Printing settings
    display.setTextColor(BLACK);
    display.setTextSize(3);
    display.setCursor(10, 10);

    // Print the grid of numbers
    display.setCursor(160, 250);
    display.print("[00] [01] [02] [03] [04] [05] [06] [07] [08] [09]");
    display.setCursor(160, 300);
    display.print("[10] [11] [12] [13] [14] [15] [16] [17] [18] [19]");
    display.setCursor(160, 350);
    display.print("[20] [21] [22] [23] [24] [25] [26] [27] [28] [29]");
    display.setCursor(160, 400);
    display.print("[30] [31] [32] [33] [34] [35] [36] [37] [38] [39]");
    display.setCursor(160, 450);
    display.print("[40] [41] [42] [43] [44] [45] [46] [47] [48] [49]");
    display.setCursor(160, 500);
    display.print("[50] [51] [52] [53] [54] [55] [56] [57] [58] [59]");

    // Print the timeout info:
    display.setCursor(460, 680);
    display.print("Timeout: ");
    display.print(CONFIG_TIMEOUT_SECONDS);
    display.print("s!");

    display.setCursor(10, 715);
    // Show it
    display.display();

    // Get the config number from serial
    // This is very similar to getting VCOM
    while (true)
    {
        int i = 0;
        Serial.println("Send a two digit number which corresponds to which Inkplate this is in the grid:\n");

        serialTimeout = millis();
        while (!Serial.available())
        {
            unsigned long now = millis();
            if (now - serialTimeout > CONFIG_TIMEOUT_SECONDS * 1000)
            {
                Serial.println("No config entered, going into normal operation...");
                return false;
            }
        }
        while (Serial.available())
        {
            serialBuffer[i++] = Serial.read();
        }
        serialBuffer[i] = 0;

        if (sscanf(serialBuffer, "%d", &configNumber) == 1)
        {
            *configNum = configNumber;
            return true;
        }
    }

    Serial.println("Config enter error!");
    return false;
}

// This function runs in each of the modes, and is used to check if the reading from serial means the user wants to change the mode
bool switchMode(String input)
{
    if (input == "OP_MODE_SLEEP")
    {
        Serial.println("Going to sleep!");
        currentOpMode = OP_MODE_SLEEP;
        return true;
    }
    if (input == "OP_MODE_IMAGEFRAME")
    {
        Serial.println("Now working as Image frame!");
        currentOpMode = OP_MODE_IMAGEFRAME;
        return true;
    }
    if (input == "OP_MODE_WAIT")
    {
        currentOpMode = OP_MODE_WAIT;
        return true;
    }
    if (input == "OP_MODE_CLEAN")
    {
        Serial.println("Cleaning screen!");
        currentOpMode = OP_MODE_CLEAN;
        return true;
    }
    if (input == "OP_MODE_VCOM")
    {
        Serial.println("Setting VCOM!");
        currentOpMode = OP_MODE_VCOM;
        return true;
    }
    if (input == "OP_MODE_RESET")
    {
        Serial.println("Resetting!");
        currentOpMode = OP_MODE_RESET;
        return true;
    }
    return false;
}

// The main function which draws images from SD card to show on the Inkplate wall
void operationAsImageFrame()
{
    // Just to be sure check if we are in 3Bit mode, if not, switch to it
    if (!isCurrentlySetTo3BitMode)
    {
        display.setDisplayMode(INKPLATE_3BIT);
        isCurrentlySetTo3BitMode = true;
    }

    if (Serial.available())
    {
        // Get the input
        // In case the input was to switch the operational mode, break out of the function
        String serialInput = Serial.readString();
        if (switchMode(serialInput))
            return;

        // Construct the image path and display the image
        String imagePath;
        imagePath += "/";
        imagePath += folderName;
        imagePath += "/";
        imagePath += displayID;
        imagePath += "_";
        imagePath += serialInput;
        imagePath += ".jpg";
        Serial.println(imagePath);
        display.drawImage(imagePath, 0, 0, true, false);
        display.display();
    }
}

// Set the VCOM and waveform, works the same as in the diagnostic examples for Inkplate 10
void setVCOM()
{
    // Get the input
    // In case the input was to switch the operational mode, break out of the function
    String serialInput = Serial.readString();
    if (switchMode(serialInput))
        return;

    do
    {
        // Get VCOM voltage from serial from user
        uint8_t flag = getVCOMFromSerial(&vcomVoltage);

        // Show the user the entered VCOM voltage
        Serial.print("Entered VCOM: ");
        Serial.println(vcomVoltage);
        display.print(vcomVoltage);
        display.partialUpdate();

        if (vcomVoltage < -5.0 || vcomVoltage > 0.0)
        {
            Serial.println("VCOM out of range!");
            display.print(" VCOM out of range!");
            display.partialUpdate();
        }

    } while (vcomVoltage < -5.0 || vcomVoltage > 0.0);

    // Write VCOM to EEPROM
    display.pinModeInternal(IO_INT_ADDR, display.ioRegsInt, 6, INPUT_PULLUP);
    writeVCOMToEEPROM(vcomVoltage);
    EEPROM.commit();

    display.selectDisplayMode(INKPLATE_3BIT);

    // Select the waveform via serial
    // Write "OK" to confirm input
    int result = 1;
    do
    {
        result = getWaveformFromSerial(&selectedWaveform);
    } while (result != 0);

    // Write waveform to EEPROM
    waveformEEPROM.waveformId = INKPLATE10_WAVEFORM1 + selectedWaveform;
    memcpy(&waveformEEPROM.waveform, waveformList[selectedWaveform], sizeof(waveformEEPROM.waveform));
    waveformEEPROM.checksum = display.calculateChecksum(waveformEEPROM);
    display.burnWaveformToEEPROM(waveformEEPROM);

    Serial.println("Waveform burned to EEPROM!");
    Serial.println("Resetting...");

    currentOpMode = OP_MODE_RESET;
}

// Prompt the user to enter '1', '2', '3' od '4' via serial to select the waveform
// Returns 0 when done
int getWaveformFromSerial(int *selected)
{
    char serialBuffer[50];
    unsigned long serialTimeout;

    showGradient(*selected);

    while (true)
    {
        Serial.println("Write '1', '2', '3', '4' or '5' to select waveform.\nWrite 'OK' to confirm.");
        while (!Serial.available())
            ;

        serialTimeout = millis();
        int i = 0;
        while ((Serial.available()) && ((unsigned long)(millis() - serialTimeout) < 500))
        {
            if ((Serial.available()) && (i < 49))
            {
                serialBuffer[i++] = Serial.read();
                serialTimeout = millis();
            }
        }
        serialBuffer[i] = 0;
        if (strstr(serialBuffer, "1") != NULL)
        {
            *selected = 0;
            return 1;
        }
        else if (strstr(serialBuffer, "2") != NULL)
        {
            *selected = 1;
            return 1;
        }
        if (strstr(serialBuffer, "3") != NULL)
        {
            *selected = 2;
            return 1;
        }
        if (strstr(serialBuffer, "4") != NULL)
        {
            *selected = 3;
            return 1;
        }
        if (strstr(serialBuffer, "5") != NULL)
        {
            *selected = 4;
            return 1;
        }
        else if (strstr(serialBuffer, "OK") != NULL || strstr(serialBuffer, "ok") != NULL ||
                 strstr(serialBuffer, "Ok") != NULL || strstr(serialBuffer, "oK") != NULL)
        {
            return 0;
        }
    }

    Serial.println("Input error!");
    return 0;
}

// Prompt user to enter VCOM
double getVCOMFromSerial(double *_vcom)
{
    double vcom = 1;
    char serialBuffer[50];
    unsigned long serialTimeout;

    // Display a message on Inkplate
    display.print("\r\n- Write VCOM on UART: ");
    display.partialUpdate(0, 1);

    while (true)
    {
        Serial.println(
            "Write VCOM voltage from epaper panel.\r\nDon't forget negative (-) sign!\r\nUse dot as the decimal point. "
            "For example -1.23\n");
        while (!Serial.available())
            ;

        serialTimeout = millis();
        int i = 0;
        while ((Serial.available()) && ((unsigned long)(millis() - serialTimeout) < 500))
        {
            if ((Serial.available()) && (i < 49))
            {
                serialBuffer[i++] = Serial.read();
                serialTimeout = millis();
            }
        }
        serialBuffer[i] = 0;
        if (sscanf(serialBuffer, "%lf", &vcom) == 1)
        {
            *_vcom = vcom;
            return 1;
        }
    }

    return 0;
}

// Show gradient of waveform - for testing which one is best
void showGradient(int _selected)
{
    int w = display.width() / 8;
    int h = display.height() - 100;

    display.changeWaveform(waveformList[_selected]);

    display.fillRect(0, 725, 1200, 100, 7);

    display.setTextSize(3);
    display.setTextColor(0);
    display.setCursor(50, 740);
    display.print("Send '1', '2', '3', '4' or '5' via serial to select waveform.");
    display.setCursor(50, 780);
    display.print("Currently selected: ");
    display.print(_selected + 1); // Increment by 1 for printing
    display.print(", send 'OK' to confirm.");

    for (int i = 0; i < 8; i++)
    {
        display.fillRect(i * w, 0, w, h, i);
    }
    display.display();
}

// Show Inkplate 10 Null waveform
void writeToScreen()
{
    display.clean(1, 8);
    display.clean(0, 2);
    display.clean(2, 10);
    // delay(10);
}

// Write VCOM
void writeVCOMToEEPROM(double v)
{
    int vcom = int(abs(v) * 100);
    int vcomH = (vcom >> 8) & 1;
    int vcomL = vcom & 0xFF;
    // First, we have to power up TPS65186
    // Pull TPS65186 WAKEUP pin to High
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 3, HIGH);

    // Pull TPS65186 PWR pin to High
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 4, HIGH);
    delay(10);

    // Send to TPS65186 first 8 bits of VCOM
    writeReg(0x03, vcomL);

    // Send new value of register to TPS
    writeReg(0x04, vcomH);
    delay(1);

    // Program VCOM value to EEPROM
    writeReg(0x04, vcomH | (1 << 6));

    // Wait until EEPROM has been programmed
    delay(1);
    do
    {
        delay(1);
    } while (display.digitalReadInternal(IO_INT_ADDR, display.ioRegsInt, 6));

    // Clear Interrupt flag by reading INT1 register
    readReg(0x07);

    // Now, power off whole TPS
    // Pull TPS65186 WAKEUP pin to Low
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 3, LOW);

    // Pull TPS65186 PWR pin to Low
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 4, LOW);

    // Wait a little bit...
    delay(1000);

    // Power up TPS again
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 3, HIGH);

    delay(10);

    // Read VCOM valuse from registers
    vcomL = readReg(0x03);
    vcomH = readReg(0x04);
    Serial.print("Vcom: ");
    Serial.println(vcom);
    Serial.print("Vcom register: ");
    Serial.println(vcomL | (vcomH << 8));

    // Trun off the TPS65186 and wait a little bit
    display.einkOff();
    delay(100);

    if (vcom != (vcomL | (vcomH << 8)))
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING FAILED!\n");
    }
    else
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");
    }
}

// Low level functions for reading and writing to registers
void writeReg(uint8_t _reg, uint8_t _data)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.write(_data);
    Wire.endTransmission();
}

uint8_t readReg(uint8_t _reg)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x48, 1);
    return Wire.read();
}

// Do 5 clean cycles, in case there is some burn-in
void cleanScreen()
{
    if (Serial.available())
    {
        // Get the input
        // In case the input was to switch the operational mode, break out of the function
        String serialInput = Serial.readString();
        if (switchMode(serialInput))
            return;
    }

    int cycles = 5;

    // Clean it by writing clear sequence to the panel.
    while (cycles)
    {
        display.clean(1, 12);
        display.clean(2, 1);
        display.clean(0, 9);
        display.clean(2, 1);
        display.clean(1, 12);
        display.clean(2, 1);
        display.clean(0, 9);
        display.clean(2, 1);

        delay(5000);
        cycles--;
    }
}

// Just wait for user input, don't do anything else
// Sometimes useful
void operationWaitForInstructions()
{
    if (Serial.available())
    {
        // Get the input
        // In case the input was to switch the operational mode, break out of the function
        String serialInput = Serial.readString();
        if (switchMode(serialInput))
            return;
    }
}
