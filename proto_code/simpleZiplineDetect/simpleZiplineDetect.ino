#include <phys253.h>          
#include <LiquidCrystal.h> 

#define HOOK_DETECT_PIN 0
 
void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);  
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
