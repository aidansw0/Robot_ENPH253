#include <phys253.h>          
#include <LiquidCrystal.h>
#include <math.h>

//ArmControl
int moveArmCyl (int alpha, float r, float z);
int moveArmAng (int alpha, float theta, float psi);
void moveBaseArmRel (float dTheta);
void moveAlpha (float alpha);
void armPID(float setpoint, float tolerance = 0.5); //Set default tolerance here
float getTheta ();
void setVertCal ();
void setHorCal ();   

//ClawControl
void setupClaw();
void calibrateClaw();
void readyClaw();
boolean checkForObject();
boolean closeClaw();
void openClaw();
void fillHistory();
void addToHistory(int a);
int averageHistory();

//ArmAndClawCommands
boolean searchAlpha(int startAlpha, int endAlpha, float r, float z, float zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
boolean searchTankArc (int startAlpha, int endAlpha, float R, float z, float r0 = TANK_R0, float alpha0 = TANK_ALPHA0, float zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
int sign(double x);
