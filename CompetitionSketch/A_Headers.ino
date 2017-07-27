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
  #define LEFT_QRD 1
  #define RIGHT_QRD 0
  #define LEFT_HASH 3
  #define RIGHT_HASH 2

// IR
  #define IR 2
  #define IR_SWITCH 10

// ArmControl
  #define ARM_POT 5
  #define ARM_MOTOR 3

// ClawControl
  #define CLAW_QRD_PIN 7
  #define CLAW_QRD_ENABLE 9
  #define GRAB_SENSOR_PIN 3

// ScissorLiftControl
  #define UP_SWITCH 6 // the digital pin that detects if the scissor lift is in the up position
  #define DOWN_SWITCH 7 // the digital pin that detects if the scissor lift is in the down position
  #define HOOK_SWITCH 5
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
  #define GATE_IR_THRESH    150

// ArmAndClawCommands
  #define SWEEP_DELAY 25
  #define DEFAULT_Z_GRAB_OFFSET 50.0
  #define TANK_R0 370.0
  #define TANK_ALPHA0 80.0
  #define Z_TANK 190.0
  #define Z_BOX 170.0
  #define R_BOX 230.0
  #define ALPHA_BOX_LEFT 20.0
  #define ALPHA_BOX_RIGHT -ALPHA_BOX_LEFT
  #define AGENT_TANK_R 190.0
  //Agent heights
  #define Z_1 180.0
  #define Z_2 180.0
  #define Z_3 180.0
  #define Z_4 180.0
  #define Z_5 180.0
  #define Z_6 180.0
  const float agentHeights[] = {NULL, Z_1, Z_2, Z_3, Z_4, Z_5, Z_6};
  

// ArmControl
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
  #define BASE_HEIGHT 200.0
  #define Z_OFFSET BASE_HEIGHT - CLAW_HEIGHT

// ClawControl
  #define CLAW_QRD_HISTORY 10       // used to help the claw identify objects with the QRD
  #define GRAB_THRESHOLD 0.1        // if the servo reading goes below the average history by this fraction, the claw can grab
  #define CLAW_QRD_THRESHOLD 0.5    // if the super QRD is +/- this percent of the calibrated reading, the grab is considered successful
  #define GRAB_VOLTAGE_THRESHOLD 8  // if the raw analog reading of the servo voltage goes below the calibrated value by this much, the grab is considered successful
  #define GRAB_DELAY 700            // in milliseconds, time between closing claw and checking for a successful grab
  #define CLAW_SERVO_CLOSE 110      // angle value to give servo to close it
  #define CLAW_SERVO_OPEN 30        // angle value to give servo to open it

// TINAHMenu
  #define MENU_OPTIONS 10            // number of options in the menu
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
  #define CLAW_GRAB_CALIBRATION_ADDR 8

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
  //IR control
  bool stopped = false;
  bool gatePassed = true;
  bool newCycle = false;
  long timerPID = 0;
  //Hashmark control
  int hash = 0;

// ArmControl
  int psiCal = -15; // Calibration (raw)
  int vertCal = 227;
  int horCal = 496; 

// ClawControl
  int history[CLAW_QRD_HISTORY];
  int closedReading = 30;         // the QRD reading when the claw is closed
  int closedVoltage = 1004;       // the grab sensor voltage when the claw is properly 

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
  String options[] = {"Start", "Course", "Speed", "DeployArm", "StowArm", "CalibrateClaw", "k", "kp", "kd", "ki", "Thresh"}; // BE SURE TO CHANGE THE "MENU_OPTIONS" CONSTANT ABOVE!!!
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
  String actions[] = {"QUIT", "TOGGLE", "EDIT", "TOGGLE", "TOGGLE", "TOGGLE", "EDIT", "EDIT", "EDIT", "EDIT", "EDIT"};

// Interrupts
  volatile unsigned int INT_2 = 0; // left wheel odometer
  volatile unsigned int INT_1 = 0; // right wheel odometer
  int interrupt_count = 0;
  int od_time = 0;


// ################################
// ########## PROTOTYPES ##########
// ################################
// TapeFollowing
  void pid();
  void hashmark();
  void zipline();

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
  void armPID(float setpoint, float tolerance = 0.5); //Set default tolerance here
  float getTheta ();
  void setVertCal ();
  void setHorCal (); 

// ClawControl
  void setupClaw();
  void calibrateClaw(boolean LCDprint = false);
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
  ISR(INT1_vect);
  ISR(INT2_vect);

  
