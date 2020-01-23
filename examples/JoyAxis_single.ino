/*
  Analog input, JoyAxis scaling, serial output

  Reads an analog input pin & updates a single JoyAxis instance with value,
  prints the JoyAxis position to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0 (A0).
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +Vcc and ground

    for best results, pot value should be between 5k and 20k.
    a small 0.1uf cap can be added between the analog pin and ground
    to help with noisy potentiometers

  created Jan. 2019
  by Marc Paquette

  This example code is in the public domain.
*/

#include <JoyAxis.h>

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

// This is the JoyAxis instance, created with default values (see documentation)
//  -Type       = Centered  (think normal joystick axis)
//  -Direction  = Normal    (not reversed)
//  -Saturation = 0.95f     (95%, 2.5% each end)
//  -Deadzone   = 0.001f    (0.1%, pretty much no deadzone; see Expo)
//  -Expo       = 1.3f      (average Expo)
//  -Smoothing  = 0.6f      (60%, average smoothing)
//  -Range      = 0.0f to 1023.0f (10 bit axis; center is 511.5, integer 512)
JoyAxis axis;

void setup() {
  // small delay to allow analog values to stabilize
  delay(500);
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // reset the axis center & range
  // (this is not strickly needed here)
  axis.reset();
}

void loop() {
  // read the potentiometer position:
  int potPosition = analogRead(analogInPin);

  // update the axis with raw pot value.
  // the first 1024 samples will be used to determine the axis center.
  // Once calibrated, the led will turn on.
  // the axis range will adjust automatically based on lowest/highest value
  axis.update(potPosition);

  // check if axis is calibrated
  if (!axis.isCalibrated()) {
    // turn led off
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    // turn led on
    digitalWrite(LED_BUILTIN, HIGH);
    
    // read current axis position
    float axisPosition = axis.position();
    
    // print the current axis position
    Serial.print("position: ");
    Serial.println(axisPosition);
  }

  // small 10ms delay (100 updates per second)
  delay(10);
}
