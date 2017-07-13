#include <phys253.h>
#include <LiquidCrystal.h>

#define SETTINGS 4
#define SERVO_KNOB 6
#define DC_KNOB 7
#define POT1_PIN 0
#define POT2_PIN 1

int speed;
int theta;
int psi;
int alpha;
int claw;
long current_time = 0;

bool button_state;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
}
  
void loop() {
  button_state = stopbutton();

//  if (button_state == HIGH) {
//    theta = knob(SERVO_KNOB) / 1023.0 * 180.0;
//  } else {
    alpha = knob(SERVO_KNOB) / 1023.0 * 180.0;
//  }
  psi = analogRead(POT1_PIN) / 1023.0 * 180.0;
  claw = knob(POT2_PIN) / 1023.0 * 180.0;
  speed = knob(DC_KNOB) / 1023.0 * 255*2 - 255.0;
  
  //Checks every 40 cycles
//  if (current_time % 20 == 0) {
    LCD.clear();
//    LCD.print("T: ");
//    LCD.print(theta);
//    LCD.print(" "); 
    LCD.print("A:");
    LCD.print(alpha);
    LCD.print(" P:");
    LCD.print(psi);
    LCD.setCursor(0, 1);
    LCD.print("Spd: ");
    LCD.print(speed);
    LCD.print(" G: ");
    LCD.print(claw);
//    LCD.print(analogRead(pot));
//    LCD.print(" ");
//    // magic numbers for now.. i know
//    LCD.print((analogRead(pot) - 563) * 90 / (1023 - 563));
//    Serial.print((analogRead(pot) - 563) * 90 / (1023 - 563));
//  }

  RCServo0.write(psi);
  RCServo1.write(alpha);
  RCServo2.write(claw);
  motor.speed(0, speed);

  current_time += 1;

  delay(50);

}

