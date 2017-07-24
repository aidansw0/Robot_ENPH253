#include <phys253.h>
#include <math.h>
#include <LiquidCrystal.h>

#define ARM_HOME_ALPHA 20
#define ARM_HOME_R 200
#define ARM_HOME_Z 200

#define SWEEP_DELAY 25

#define DEFAULT_Z_GRAB_OFFSET 50.0
#define TANK_R0 480.0
#define TANK_ALPHA0 60.0

#define Z_BOX 200.0
#define R_BOX 150.0
#define ALPHA_BOX_LEFT 15.0
#define ALPHA_BOX_RIGHT -ALPHA_BOX_LEFT

int moveArmCyl (int alpha, float r, float z);
int moveArmAng (int alpha, float theta, float psi);
void moveBaseArmRel (float dTheta);
void moveAlpha (float alpha);
void armPID(float setpoint, float tolerance = 0.5); //Set default tolerance here
float getTheta ();
void setVertCal ();
void setHorCal ();   

void setupClaw();
void calibrateClaw();
void readyClaw();
boolean checkForObject();
boolean closeClaw();
void openClaw();
void fillHistory();
void addToHistory(int a);
int averageHistory();

boolean searchAlpha(int startAlpha, int endAlpha, float r, float z, float zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
boolean searchTankArc (int startAlpha, int endAlpha, float R, float z, float r0 = TANK_R0, float alpha0 = TANK_ALPHA0, float zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
int sign(double x);
 
void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);  
  moveArmAng(90, 30, -45);
  closeClaw();
}
 
void loop() {
  if (startbutton()) {
    searchAlpha (90, -15, 250, 50);
    //searchTankArc (90, 30, 250, 150);
    delay(200);
  }
  delay(50);
}

void armHome() {
  moveArmCyl(ARM_HOME_ALPHA, ARM_HOME_R, ARM_HOME_Z);
}

boolean searchAlpha(int startAlpha, int endAlpha, float r, float z, float zGrabOffset) {
  moveArmCyl(startAlpha, r, z);
  delay(200);
  readyClaw();
  
  if (endAlpha < startAlpha) {
    endAlpha--;
  } else {
    endAlpha++;
  }
  
  for (int alpha = startAlpha; alpha != endAlpha; alpha += sign(endAlpha-startAlpha)) {
    moveAlpha(alpha);
    if (checkForObject()) {
      moveArmCyl(alpha, r, z-zGrabOffset);
      if (closeClaw()) {
        moveArmCyl(alpha, r, z);
        return true;
      } else {
        moveArmCyl(alpha, r, z);
        openClaw();
      }
    }
    delay(SWEEP_DELAY);
  }
  
  return false;
}

boolean searchTankArc (int startAlpha, int endAlpha, float R, float z, float r0, float alpha0, float zGrabOffset) {
  float r = getRCircularArc(startAlpha, r0, alpha0, R);
  moveArmCyl(startAlpha, r, z);
  delay(200);
  readyClaw();
  
  if (endAlpha < startAlpha) {
    endAlpha--;
  } else {
    endAlpha++;
  }
  
  for (int alpha = startAlpha; alpha != endAlpha; alpha += sign(endAlpha-startAlpha)) {
    r = getRCircularArc(alpha, r0, alpha0, R);
    moveArmCyl(alpha, r, z);
    if (checkForObject()) {
      moveArmCyl(alpha, r, z-zGrabOffset);
      if (closeClaw()) {
        moveArmCyl(alpha, r, z);
        return true;
      } else {
        moveArmCyl(alpha, r, z);
        openClaw();
      }
    }
    delay(SWEEP_DELAY);
  }
  
  return false;
}

float getRCircularArc (int alpha, float r0, float alpha0, float R) {
  float tempCos = cos( (alpha-alpha0) * M_PI/180.0 );
  return r0 * tempCos - sqrt(r0 * r0 * (tempCos * tempCos - 1) + R * R);
}

int sign(double x) {
  if (x >= 0.0)
    return 1;
  else
    return -1;
}

