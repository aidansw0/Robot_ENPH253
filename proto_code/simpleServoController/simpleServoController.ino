#include <phys253.h>     
#include <LiquidCrystal.h>

#define KNOB 6

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
}
 
void loop() {
  int reading = knob(KNOB);
  int angle = reading / 1023.0 * 180.0;
  RCServo1.write(angle);

  LCD.clear();
  LCD.print("Angle: ");
  LCD.print(angle);

  delay(100);
}
