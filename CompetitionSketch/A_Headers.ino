//Place all includes, function prototypes, defines, and global variables in this file.

//Includes
  #include <phys253.h>          
  #include <LiquidCrystal.h>
  #include <math.h>


//ArmControl
  //Pins
  #define ARM_POT 4
  #define ARM_MOTOR 0
  
  //Arm dimensions and limits (degrees, mm)
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

  //Prototypes
  int moveArmCyl (int alpha, float r, float z);
  int moveArmAng (int alpha, float theta, float psi);
  void moveBaseArmRel (float dTheta);
  void moveAlpha (float alpha);
  void armPID(float setpoint, float tolerance = 0.5); //Set default tolerance here
  float getTheta ();
  void setVertCal ();
  void setHorCal (); 
   
  //Globals
  //Calibration (raw)
  int psiCal = -15;
  int vertCal = 49;
  int horCal = 344; 


//ClawControl
  #define CLAW_QRD_PIN 6
  #define GRAB_SENSOR_PIN 5
  #define CLAW_QRD_HISTORY 10 // used to help the claw identify objects with the QRD
  #define GRAB_THRESHOLD 0.1 // if the servo reading goes below the average history by this fraction, the claw can grab
  #define CLAW_QRD_THRESHOLD 0.5 // if the super QRD is +/- this percent of the calibrated reading, the grab is considered successful
  #define GRAB_VOLTAGE_THRESHOLD 8 // if the raw analog reading of the servo voltage goes below the calibrated
                                   // value by this much, the grab is considered successful
  #define GRAB_DELAY 200 // in milliseconds, time between closing claw and checking for a successful grab
  #define CLAW_SERVO_CLOSE 110 // angle value to give servo to close it
  #define CLAW_SERVO_OPEN 15  // angle value to give servo to open itclosed

  //Prototypes
  void setupClaw();
  void calibrateClaw();
  void readyClaw();
  boolean checkForObject();
  boolean closeClaw();
  void openClaw();
  void fillHistory();
  void addToHistory(int a);
  int averageHistory();

  //Globals
  int history[CLAW_QRD_HISTORY];
  int closedReading = 30; // the QRD reading when the claw is closed
  int closedVoltage = 1004; // the grab sensor voltage when the claw is properly 


//ArmAndClawCommands
  #define SWEEP_DELAY 25
  #define DEFAULT_Z_GRAB_OFFSET 50.0
  #define TANK_R0 480.0
  #define TANK_ALPHA0 60.0
  #define Z_BOX 200.0
  #define R_BOX 150.0
  #define ALPHA_BOX_LEFT 15.0
  #define ALPHA_BOX_RIGHT -ALPHA_BOX_LEFT
  
  //Prototypes
  boolean searchAlpha(int startAlpha, int endAlpha, float r, float z, float zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
  boolean searchTankArc (int startAlpha, int endAlpha, float R, float z, float r0 = TANK_R0, float alpha0 = TANK_ALPHA0, float zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
  float getRCircularArc (int alpha, float r0, float alpha0, float R);
  int sign(double x);

