#include <phys253.h>
#include <LiquidCrystal.h>

#define SETTINGS 4
#define SERVO_KNOB 6
#define DC_KNOB 7
#define POT_PIN 0

int speed;
int theta;
int psi;
int alpha;
int pot;
int current_time = 0;

bool button_state;

void setup() {
#include <phys253setup.txt>
  Serial.begin(9600);

  button_state = stopbutton();
  pot = analogRead(POT_PIN);
}
  
void loop() {
  button_state = stopbutton();

//  if (button_state == HIGH) {
//    theta = knob(SERVO_KNOB) / 1023.0 * 180.0;
//  } else {
    psi = knob(SERVO_KNOB) / 1023.0 * 180.0;
//  }
 
  speed = (knob(DC_KNOB) - 511.5) / 1023.0 * 510.0;
  //Checks every 40 cycles
  if (current_time % 40 == 0) {
    LCD.clear();
//    LCD.print("T: ");
//    LCD.print(theta);
//    LCD.print(" "); 
    LCD.print("P: ");
    LCD.print(psi);
    LCD.setCursor(0, 1);
    LCD.print("Spd: ");
    LCD.print(speed);
//    LCD.print(analogRead(pot));
//    LCD.print(" ");
//    // magic numbers for now.. i know
//    LCD.print((analogRead(pot) - 563) * 90 / (1023 - 563));
//    Serial.print((analogRead(pot) - 563) * 90 / (1023 - 563));
  }

  //RCServo0.write(theta);
  RCServo1.write(psi);
  motor.speed(0, speed);

  current_time += 1;

  delay(10);

}

