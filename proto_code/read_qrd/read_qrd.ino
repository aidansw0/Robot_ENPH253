#include <phys253.h>
#include <LiquidCrystal.h>

#define QRD_PIN A1
#define GRAB_THRESHOLD 20 // if the reading goes below base_threshold by this amount, the claw will grab
#define GRAB_DELAY 100 // in milliseconds, time between sensing object and closing claw
#define SERVO_CLOSE 110 // angle value to give servo to close it
#define SERVO_OPEN 15  // angle value to give servo to open it

int base_threshold = 0;
boolean claw_open = true;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  pinMode(QRD_PIN, INPUT);
  RCServo0.write(SERVO_OPEN);
}

void loop() {
  int reading = analogRead(QRD_PIN);
  
  if (startbutton()) {
    base_threshold = reading;
  }
  
  if (claw_open && reading < base_threshold - GRAB_THRESHOLD) {
    delay(GRAB_DELAY);
    RCServo0.write(SERVO_CLOSE);
    claw_open = false;
  }

  if (!claw_open && stopbutton()) {
    RCServo0.write(SERVO_OPEN);
    claw_open = true;
    delay(500);
  }
  
  LCD.clear();
  LCD.print("Reading: ");
  LCD.print(reading);
  LCD.setCursor(0, 1);
  LCD.print("Base: ");
  LCD.print(base_threshold);

  delay(50);
}
