#include <phys253.h>
#include <math.h>
#include <LiquidCrystal.h>

// PID
#define SETTINGS 4
#define ARM_KNOB 7
#define ARM_POT 0
#define ARM_MOTOR 0
#define D_SAMPLES 5
#define INT_THRESH 50

// menu
#define KNOB_N 6
#define MENU_OPTIONS 6

// member lengths
#define L1 276.265
#define L2 120.0

// PID
int kp = 5;
int kd = 10;
int ki = 0;
float integral = 0;
int k = 10;
float tolerance = 1;
float tolMargin = 1;
bool hold;
float last_error = 0;
long current_time = 0;
float vertCal = 68;
float horCal = 389;
float theta = 45;
float alpha = 0;
float psi = 0;
float r, z;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    alpha = Serial.parseFloat();
    r = Serial.parseFloat();
    z = Serial.parseFloat();
  
    float sqsum = r*r+z*z;
    float hyp = sqrt(sqsum);
    theta = acos( (L1*L1+sqsum-L2*L2)/(2.0*L1*hyp) ) + atan(z/r);
    psi = - M_PI + theta + acos( (L1*L1+L2*L2-sqsum)/(2.0*L1*L2) );
    theta *= 180.0/M_PI;
    psi *= 180.0/M_PI;
    Serial.println((acos( (L1*L1+sqsum-L2*L2)/(2.0*L1*hyp) ) + atan(z/r)) * 180.0/M_PI);
    Serial.println((M_PI - (acos( (L1*L1+sqsum-L2*L2)/(2.0*L1*hyp) ) + atan(z/r)) - acos( (L1*L1+L2*L2-sqsum)/(2.0*L1*L2) )) * 180.0/M_PI);
  }
  
  RCServo0.write(alpha / 135.0 * 90 + 90);
  RCServo1.write(theta - psi);
  armPID(theta);
}

void armPID(float setpoint) {
  float armPos = getArmPosition();
  float error = armPos - setpoint;

  float prop = k * kp * error;
  float deriv = k * (kd * (float) (error - last_error));
  integral += k * ki * error;

  //anti-windup
  if (integral > INT_THRESH) {
    integral = INT_THRESH;
  } else if (integral < -INT_THRESH) {
    integral = -INT_THRESH;
  }

  current_time++;
  last_error = error;
  float control = prop + deriv + integral;
  
  if(!hold) {
    if (abs(error) < tolerance) {
      hold = 1;
      motor.speed(ARM_MOTOR, 0);
    } else {
      motor.speed(ARM_MOTOR, control);
    }
  } else if (abs(error) > tolerance+tolMargin) {
    hold = 0;
    integral = 0;
  }

  if (stopbutton()) {
    //inMenu = true;
    //motor.speed(ARM_MOTOR, 0);
    vertCal = analogRead(ARM_POT);
    delay(500);
  }
  if (startbutton()) {
    //inMenu = true;
    //motor.speed(ARM_MOTOR, 0);
    horCal = analogRead(ARM_POT);
    delay(500);
  }

  if (current_time % 50 == 0) {
    LCD.clear();
    LCD.print("P:");
    LCD.print(armPos);
    LCD.print(" Set:");
    LCD.print(setpoint);
    LCD.setCursor(0,1);
    k = knob(KNOB_N)/1023.0*20;
    LCD.print("Gn:");
    LCD.print(k);
    LCD.print(" Err:");
    LCD.print(error);
    Serial.print("V:");
    Serial.print(vertCal);
    Serial.print(" H:");
    Serial.print(horCal);
    Serial.print(" P:");
    Serial.print(armPos);
    Serial.print(" Alpha:");
    Serial.print(alpha);
    Serial.print(" Set:");
    Serial.print(setpoint);
    Serial.print(" Psi:");
    Serial.print(psi);
    Serial.print(" Gn:");
    Serial.print(k);
    Serial.print(" Err:");
    Serial.print(error);
    Serial.print(" P:");
    Serial.print(prop);
    Serial.print(" D:");
    Serial.print(deriv);
    Serial.print(" I:");
    Serial.print(integral);
    Serial.print(" Ctrl:");
    Serial.print(control);
    Serial.print("\n");
  }
} 

float getArmPosition() {
  return 90-(analogRead(ARM_POT) - vertCal) / (horCal-vertCal) * 90;
}


