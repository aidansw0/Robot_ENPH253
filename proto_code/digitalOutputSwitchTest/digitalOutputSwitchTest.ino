#include <phys253.h>          
#include <LiquidCrystal.h> 

#define SWITCH_PIN 9
#define READ_PIN A0

boolean switchOn;
 
void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);

  digitalWrite(SWITCH_PIN, LOW);
  switchOn = false;
}
 
void loop() {
  if (startbutton()) {
    if (switchOn) {
      digitalWrite(SWITCH_PIN, LOW);
      switchOn = false;
    } else {
      digitalWrite(SWITCH_PIN, HIGH);
      switchOn = true;
    }
    delay(500);
  }
  LCD.clear();
  LCD.print("On: ");
  LCD.print(switchOn);
  LCD.setCursor(0, 1);
  LCD.print(analogRead(READ_PIN));
  delay(100);
}
