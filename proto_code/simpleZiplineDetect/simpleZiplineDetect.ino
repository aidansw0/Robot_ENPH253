#include <phys253.h>          
#include <LiquidCrystal.h> 

#define HOOK_DETECT_PIN 5
 
void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);  
  RCServo0.write(140);
  RCServo2.write(90);
  RCServo1.write(90);
}
 
void loop() {
  if (digitalRead(HOOK_DETECT_PIN)) {
    LCD.clear();
    LCD.print("READY");
  } else {
    LCD.clear();
    LCD.print("DETECT!");
    while(!stopbutton()) {
      delay(1);
    }
  }
  delay(50);
}
