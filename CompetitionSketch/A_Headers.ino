// Place all includes, function prototypes, defines, and global variables in this file.

// #############################
// ########## IMPORTS ##########
// #############################
  #include <phys253.h>          
  #include <LiquidCrystal.h>
  #include <math.h>
  #include <avr/EEPROM.h>
  #include <avr/interrupt.h>

  
// ##########################
// ########## PINS ##########
// ##########################
// Drive Motors
  #define LEFT_MOTOR 0
  #define RIGHT_MOTOR 1

// TapeFollowing
  #define LEFT_QRD 0        //analog
  #define RIGHT_QRD 1       //analog
  #define LEFT_HASH 4       //digital
  #define RIGHT_HASH 3      //digital
  #define ODOMETER_RIGHT 2  //digital
  #define ODOMETER_LEFT 1   //digital

// IR
  #define IR 2              //analog
  #define IR_SWITCH 10      //digital

// ArmControl
  #define ARM_POT 4         //analog
  #define ARM_MOTOR 3
  #define THETA_SAMPLES 10  //constant

// ClawControl
  #define CLAW_QRD_PIN 7     //analog
  #define CLAW_QRD_ENABLE 9  //digital
  #define GRAB_SENSOR_PIN 3  //analog

// ScissorLiftControl
  #define UP_SWITCH 6 // the digital pin that detects if the scissor lift is in the up position
  #define DOWN_SWITCH 7 // the digital pin that detects if the scissor lift is in the down position
  #define HOOK_SWITCH 5      //digital
  #define SCISSOR_UP 255
  #define SCISSOR_DOWN -255
  #define SCISSOR_MOTOR 2

// TINAHMenu
  #define KNOB 6


// ###############################
// ########## CONSTANTS ##########
// ###############################

// TapeFollowing/IR
  #define INT_THRESH        50
  #define OFF_TAPE_ERROR    5 // absolute value of error when neither QRD sees tape
  #define GATE_IR_THRESH    75 // was 150 for older method
  #define ZIPLINE_IR_THRESH 100
  #define IR_COMP_THRESH    20
  float IR_GATE_DISTANCE = 100.0;
  #define RAMP_LENGTH 130.0
  #define GATE_TO_RAMP_DISTANCE 130.0
  float POST_RAMP_DISTANCE = 115.0;

// ArmAndClawCommands
  #define SWEEP_DELAY 3
  #define DEFAULT_Z_GRAB_OFFSET 50.0
  #define TANK_R0 370.0
  #define TANK_ALPHA0 95.0
  #define Z_TANK 180.0
  #define Z_BOX 250.0
  #define R_BOX 240.0
  #define ALPHA_BOX_LEFT 19.0
  #define ALPHA_BOX_RIGHT -ALPHA_BOX_LEFT
  float AGENT_TANK_R = 155.0; //160 on left course, 150 right
  //Agent heights
  #define Z_1 160.0
  #define Z_2 140.0 //was 150
  #define Z_3 150.0
  #define Z_4 160.0 //was 155
  #define Z_5 140.0 //was 145
  #define Z_6 150.0 
  const float agentHeights[] = {Z_TANK, Z_6, Z_5, Z_4, Z_3, Z_2, Z_1, Z_TANK, Z_6, Z_5, Z_4, Z_3, Z_2, Z_1};
  

// ArmControl
  #define ALPHA_DELAY 0
  #define INT_THRESH 50
  #define L1 276.265
  #define L2 120.0
  #define ALPHA_MIN -135.0
  #define ALPHA_MAX 135.0
  #define THETA_MIN 0.0
  #define THETA_MAX 90.0
  #define PHI_MIN 10.0
  #define PHI_MAX 165.0
  #define CLAW_HEIGHT 140.0
  #define BASE_HEIGHT 185.0 
  #define Z_OFFSET BASE_HEIGHT - CLAW_HEIGHT

// ClawControl
  #define CLAW_QRD_HISTORY 10       // used to help the claw identify objects with the QRD
  #define GRAB_THRESHOLD 0.05        // if the servo reading goes below the average history by this fraction, the claw can grab
  #define CLAW_QRD_THRESHOLD 0.8    // if the super QRD is +/- this percent of the calibrated reading, the grab is considered successful
  #define GRAB_VOLTAGE_THRESHOLD 8  // if the raw analog reading of the servo voltage goes below the calibrated value by this much, the grab is considered successful
  #define GRAB_DELAY 500            // in milliseconds, time between closing claw and checking for a successful grab
  #define CLAW_SERVO_CLOSE 105      // angle value to give servo to close it
  #define CLAW_SERVO_OPEN 15        // angle value to give servo to open it

// TINAHMenu
  #define MENU_OPTIONS 13            // number of options in the menu
  #define BOOT_DELAY 500            // gives the user time to set the TINAH down before menu starts
  #define MENU_REFRESH 100          // menu refresh delay
  #define MAX 1023                  // analogRead maximum
  #define MENU_KNOB_DIV ((double) (MAX + 1) / MENU_OPTIONS)
  // EEPROM addresses
  #define SPEED_ADDR      1
  #define KP_ADDR         2
  #define KD_ADDR         3
  #define KI_ADDR         4
  #define K_ADDR          5
  #define THRESH_ADDR     6
  #define CLAW_QRD_CALIBRATION_ADDR 7
  #define CLAW_GRAB_STRESS_ADDR     8
  #define CLAW_GRAB_EMPTY_ADDR      9
  
// Interrupts
  #define WHEEL_DIAMETER 6.51
  #define WHEEL_DIVS 9.0
  #define WHEEL_CIRCUMFERENCE WHEEL_DIAMETER * M_PI

// #############################
// ########## GLOBALS ##########
// #############################
// Course Selection
  #define LEFT 1
  #define RIGHT -1
  int course = LEFT;

// TapeFollowing
  //PID
  int error = 0;
  int last_error = 0;
  int recent_error = last_error;
  int current_time = 0;
  int last_time = 0;
  int turnOffset = 0;
  int errorOffset = 0;
  //IR control
  bool stopped = false;
  bool gatePassed = false;
  bool newCycle = false;
  long timerPID = 0;
  int irSide = 0;
  bool detectedIR = false;
  //Hashmark control
  int hash = 0;

// ArmControl
  int psiCal = 20; // Calibration (raw)
  int vertCal = 350;
  int horCal = 600; 

// ClawControl
  int history[CLAW_QRD_HISTORY];
  int closedReading = 30;         // the QRD reading when the claw is closed
  int closedStressVoltage = 1023; // the grab sensor voltage when the claw is drawing current
  int closedEmptyVoltage = 1004;

// TINAHMenu
  // Values that the menu alters:
  int speed         = 100;
  int kp            = 20;
  int kd            = 50;
  int ki            = 0;
  int k             = 2;
  int thresh        = 120;
  double distance   = 0.0;
  // Variables for the menu:
  boolean inMenu = true;
  int menuPos;
  String options[] = {"Start", "Course", "Speed", "DeployArm", "StowArm", "CalibrateClaw", "MoveScissor", "DropInBox", "k", "kp", "kd", "ki", "Thresh"}; // BE SURE TO CHANGE THE "MENU_OPTIONS" CONSTANT ABOVE!!!
  /*
   * Each option must have an action associated with it. Each action results
   * in different menu behaviour.
   * 
   * QUIT - Exit the menu and start some other code.
   * EDIT - Adjust an integer variable using a knob.
   * TOGGLE - Simply change swap true and false without entering a sub-menu.
   * DRESET - Sets a double value back to zero without entering a sub-menu.
   * IRESET - Sets a double value back to zero without entering a sub-menu.
   */
  String actions[] = {"QUIT", "TOGGLE", "EDIT", "TOGGLE", "TOGGLE", "TOGGLE", "TOGGLE", "TOGGLE", "EDIT", "EDIT", "EDIT", "EDIT", "EDIT"};

// Interrupts
  volatile double leftDistance = 0;
  volatile double rightDistance = 0;
  volatile long lastInterrupt;

// ################################
// ########## PROTOTYPES ##########
// ################################
// TapeFollowing
  void pid();
  void hashmark();
  void zipline();
  void getError();

// ArmAndClawCommands
  boolean searchAlpha(int startAlpha, int endAlpha, double r, double z, double zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
  boolean searchTankArc (int startAlpha, int endAlpha, double R, double z, double r0 = TANK_R0, double alpha0 = TANK_ALPHA0, double zGrabOffset = DEFAULT_Z_GRAB_OFFSET);
  void deployArm ();
  void stowArm();
  void dropInBox (int side);
  double getRCircularArc (int alpha, double r0, double alpha0, double R);
  double getMaxAlphaOffset (double r0, double R);
  int sign(double x);

// ArmControl
  int moveArmCyl (int alpha, float r, float z);
  int moveArmAng (int alpha, float theta, float psi);
  void moveBaseArmRel (float dTheta);
  void moveAlpha (float alpha);
  void armPID(float setpoint, float tolerance = 1.0); //Set default tolerance here
  float getTheta ();
  void setVertCal ();
  void setHorCal (); 

// ClawControl
  void setupClaw();
  void calibrateClawQRD(boolean LCDprint = false);
  void calibrateClawGrab(boolean LCDprint = false);
  void readyClaw();
  boolean checkForObject();
  boolean closeClaw();
  void openClaw();
  void fillHistory();
  void addToHistory(int a);
  int averageHistory();
  void disableClawQrd();
  void enableClawQrd();

// TINAHMenu
  int getValInt(int i);
  void setValInt(int i, int val);
  double getValDouble(int i);
  void setValDouble(int i, double val);
  boolean getValBool(int i);
  void setValBool(int i, boolean val);
  void displayMenu();
  int getMenuPos(int menuReading);
  void populateMenuLCD();
  String getMenuVal(int i);
  uint16_t readEEPROM(int addressNum);
  void writeEEPROM(int addressNum, uint16_t val);

// Interrupts
  void enableExternalInterrupt(unsigned int INTX, unsigned int mode);
  void disableExternalInterrupt(unsigned int INTX);
  void waitDistance(double distance);
  double getDistance();
  double getSpeed();
  ISR(INT1_vect);
  ISR(INT2_vect);

  
