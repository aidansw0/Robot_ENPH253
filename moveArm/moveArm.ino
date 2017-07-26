#include <phys253.h>
#include <math.h>
#include <LiquidCrystal.h>

//Pins
#define ARM_POT 5
#define ARM_MOTOR 3

//Arm lengths and limits (degrees, mm)
#define L1 276.265
#define L2 120.0
#define ALPHA_MIN -135.0
#define ALPHA_MAX 135.0
#define THETA_MIN 0.0
#define THETA_MAX 90.0
#define PHI_MIN 0.0
#define PHI_MAX 150.0

//PID
#define INT_THRESH 50

//Calibration (raw)
int psiCal = 15;
int vertCal = 227;
int horCal = 496;

//Function prototypes
int moveArmCyl (int alpha, float r, float z);
int moveArmAng (int alpha, float theta, float psi);
void moveBaseArmRel (float dTheta);
void moveAlpha (float alpha);
void armPID(float setpoint, float tolerance = 1); //Set default tolerance here
float getTheta ();
void setVertCal ();
void setHorCal ();

//Void/setup for compiling/testing this file only
void setup() {
  RCServo2.write(90);
  RCServo0.write(90);
  RCServo1.write(90);
  #include <phys253setup.txt>
  Serial.begin(9600);
  Serial.println("Enter:\nh [horCal]; v [vertCal]; m dTheta [moveBaseArmRel]; a alpha [moveAlpha];\nc alpha r z [moveArmCyl]; s alpha theta psi [moveArmAng]");
}
void loop() {
  if (Serial.available()) {
    float alpha;
    float r;
    float z;
    char select = Serial.read();
    Serial.read();
    switch(select) {
      case 'h':
        setHorCal();
        Serial.println(horCal);
        break;
      case 'v':
        setVertCal();
        Serial.println(vertCal);
        break;
      case 'a':
        moveAlpha(Serial.parseFloat());
        break;
      case 'c':
        alpha = Serial.parseFloat();
        r = Serial.parseFloat();
        z = Serial.parseFloat();
        moveArmCyl(alpha, r, z);
        Serial.print("Theta: ");
        Serial.println(getTheta());
        break;
      case 's':
        alpha = Serial.parseFloat();
        r = Serial.parseFloat();
        z = Serial.parseFloat();
        moveArmAng(alpha, r, z);
        Serial.print("Theta: ");
        Serial.println(getTheta());
        break;
      case 'm':
        moveBaseArmRel(Serial.parseFloat());
        Serial.print("Theta: ");
        Serial.println(getTheta());
        break;
    }
  }
  delay(50);
}


//Moves the arm to alpha, r, z coordinates (degrees, mm). Returns -1 if exceeds arm limits.
int moveArmCyl (int alpha, float r, float z) {
  float sqsum = r * r + z * z;
  float theta = acos( (L1 * L1 + sqsum - L2 * L2) / (2.0 * L1 * sqrt(sqsum)) ) + atan(z / r);
  float psi = theta + acos( (L1 * L1 + L2 * L2 - sqsum) / (2.0 * L1 * L2) ) - M_PI;

  //to degrees
  theta *= 180.0 / M_PI;
  psi *= 180.0 / M_PI;

  return moveArmAng(alpha, theta, psi);
}

//Moves the arm to alpha, theta, psi coordinates (degrees). Returns -1 if exceeds arm limits.
int moveArmAng (int alpha, float theta, float psi) {
  psi = theta - psi; //convert to phi
  if (alpha < ALPHA_MIN || alpha > ALPHA_MAX || 
      theta < THETA_MIN || theta > THETA_MAX || 
      psi < PHI_MIN || psi > PHI_MAX) 
    return -1;
  
  moveAlpha(alpha);
  RCServo1.write(psi + psiCal);
  armPID(theta);
  return 0;
}

//Moves the large arm by increment dTheta (degrees). NOT LIMITED
void moveBaseArmRel (float dTheta) {
  armPID(getTheta() + dTheta);
}

void moveAlpha (float alpha) {
  RCServo0.write(alpha / 135.0 * 90 + 90);
}

//Moves the large arm to setpoint within tolerance (degrees). Default tolerance is 1 degree (set in prototype)
void armPID(float setpoint, float tolerance) {
  int kp = 50;
  int kd = 100;
  int ki = 0;

  int control;
  float Theta;
  float error;
  float lastError = 0;
  float prop;
  float deriv;
  float integral = 0;

  while (true) {
    error = getTheta() - setpoint;

    //Exit condition
    if (abs(error) < tolerance) {
      motor.speed(ARM_MOTOR, 0);
      delay(50);
      if (abs(error) < tolerance)
        return;
    }

    prop = kp * error;
    deriv = kd * (error - lastError);
    //integral += ki * error;

    //Anti-windup
    /*if (integral > INT_THRESH) {
      integral = INT_THRESH;
      } else if (integral < -INT_THRESH) {
      integral = -INT_THRESH;
      }*/

    lastError = error;
    int control = prop + deriv + integral;
    motor.speed(ARM_MOTOR, control);
  }
}

float getTheta () {
  return 90.0 - (analogRead(ARM_POT) - vertCal) / (float) (horCal - vertCal) * 90.0;
}

void setVertCal () {
  vertCal = analogRead(ARM_POT);
}

void setHorCal () {
  horCal = analogRead(ARM_POT);
}

