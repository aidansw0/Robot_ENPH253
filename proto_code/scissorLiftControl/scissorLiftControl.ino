#include <phys253.h>
#include <LiquidCrystal.h>

#define UP_SWITCH 6 // the digital pin that detects if the scissor lift is in the up position
#define DOWN_SWITCH 7 // the digital pin that detects if the scissor lift is in the down position
#define SPEED_KNOB 6 // the knob that selects the speed of the motor
#define MOTOR_PIN 2

int runs = 200;
int speed = 0;
String state = "UNKNOWN";

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  RCServo0.write(90);
  RCServo2.write(90);
  RCServo1.write(90);
}

void loop() {
  boolean up = !digitalRead(UP_SWITCH);
  boolean down = !digitalRead(DOWN_SWITCH);

  if (up) {
    state = "UP";
  } else if (down) {
    state = "DOWN";
  } else {
    state = "UNKNOWN";
  }
  
  if (runs >= 200) {
    speed = knob(SPEED_KNOB) / 1023.0 * 255.0;
    displayState();
    runs = 0;
  } else {
    runs++;
  }

  if ((startbutton() || !digitalRead(5)) && up) {
    moveDown();
  } else if (startbutton() && down) {
    moveUp();
  } else if (startbutton() && state == "UNKNOWN") {
    moveDown();
  }
  delay(1);
}

void displayState() {
    LCD.clear();
    LCD.print(state);
    LCD.setCursor(0, 1);
    LCD.print("SPD: ");
    LCD.print(speed);
}

void moveUp() {
    motor.speed(MOTOR_PIN, speed);
    state = "MOVING";
    displayState();
    while(digitalRead(UP_SWITCH)) {
      delay(1);
    }
    motor.speed(MOTOR_PIN, 0);
}

void moveDown() {
    motor.speed(MOTOR_PIN, -speed);
    state = "MOVING";
    displayState();
    while(digitalRead(DOWN_SWITCH)) {
      delay(1);
    }
    motor.speed(MOTOR_PIN, 0);
}

