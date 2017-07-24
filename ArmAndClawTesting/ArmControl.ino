//Pins
#define ARM_POT 4
#define ARM_MOTOR 0

//Arm lengths and limits (degrees, mm)
#define L1 276.265
#define L2 120.0
#define ALPHA_MIN -135.0
#define ALPHA_MAX 135.0
#define THETA_MIN 0.0
#define THETA_MAX 90.0
#define PHI_MIN 0.0
#define PHI_MAX 165.0

#define CLAW_HEIGHT 140.0
#define BASE_HEIGHT 185.0
#define Z_OFFSET BASE_HEIGHT - CLAW_HEIGHT

//PID
#define INT_THRESH 50

//Calibration (raw)
int psiCal = -15;
int vertCal = 49;
int horCal = 344;

//Function prototypes

//Void/setup for compiling/testing this file only


//Moves the arm to alpha, r, z coordinates (degrees, mm). Returns -1 if exceeds arm limits.
int moveArmCyl (int alpha, float r, float z) {
  z -= Z_OFFSET;
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
  psi = theta - (psi + psiCal); //convert to phi
  if (alpha < ALPHA_MIN || alpha > ALPHA_MAX || 
      theta < THETA_MIN || theta > THETA_MAX || 
      psi < PHI_MIN || psi > PHI_MAX) 
    return -1;
  
  moveAlpha(alpha);
  RCServo1.write(psi);
  armPID(theta);
  return 0;
}

//Moves the large arm by increment dTheta (degrees). NOT LIMITED
void moveBaseArmRel (float dTheta) {
  armPID(getTheta() + dTheta);
}

void moveAlpha (float alpha) {
  RCServo2.write(alpha / 135.0 * 90 + 90);
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
      //delay(50);
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

