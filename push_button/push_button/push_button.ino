// Push button to change the image displayed on the Inkplate wall
// Based on the RBD_Button library

#include <RBD_Timer.h>  // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Button.h> // https://github.com/alextaujenis/RBD_Button

RBD::Button button(PA4);

void setup() {
  Serial.begin(115200);
}

void loop() {
  if(button.onPressed()) {
    Serial.println("NEXT");
  }
}
