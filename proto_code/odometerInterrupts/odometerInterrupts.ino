#include <phys253.h>
#include <avr/interrupt.h> // enable interrupts
#include <LiquidCrystal.h>
#include <avr/EEPROM.h>

// odometer
#define OD_TIMEOUT              40
#define WHEEL_DIAMETER          6.5 // in cm
#define WHEEL_DIVS              5 // a division is a black section followed by a white section on the encoder.
// assumes equal spacing and widths of divisions
#define WHEEL_CIRCUMFERENCE (PI * WHEEL_DIAMETER)
#define DISTANCE_COMPENSATOR 1.0
float leftWheelSpeed;
long lastLeftWheelTime;
float rightWheelSpeed;
long lastRightWheelSpeed;

// PID
#define SETTINGS          4 
#define LEFT_QRD          A1
#define RIGHT_QRD         A2 
#define LEFT_MOTOR        0
#define RIGHT_MOTOR       1
#define INT_THRESH        100
#define RAMP_LENGTH       135.0// cm, 125 actual
#define TO_RAMP           285.0 // cm, distance to ramp from start,  initial value is 285.0
#define CHASSIS_LENGTH    30.0 // cm
#define OFF_TAPE_ERROR    5 // absolute value of error when neither QRD sees tape

// ##### MENU VARIABLES #####
int speed         = 60;
int kp            = 2;
int kd            = 100;
int ki            = 0;
int k             = 2;
int thresh        = 100;
double leftDistance = 0.0;
double rightDistance = 0.0;
double distance   = 0.0;
 
// EEPROM addresses
#define SPEED_ADDR      1
#define KP_ADDR         2
#define KD_ADDR         3
#define KI_ADDR         4
#define K_ADDR          5
#define THRESH_ADDR     6

// ##### MENU CONSTANTS #####
#define MENU_OPTIONS        8 // number of options in the menu
#define KNOB                6 // the knob to use for scrolling and setting variables
#define SCALE_KNOB          7 // used to scale the first knob's input to help with selecting a variable
#define BOOT_DELAY          500 // gives the user time to set the TINAH down before menu starts
#define MENU_REFRESH        100 // menu refresh delay
#define MAX                 1023 // analogRead maximum
#define MENU_KNOB_DIV       ((double) (MAX + 1) / MENU_OPTIONS)

// BE SURE TO CHANGE THE MENU_OPTIONS VARIABLE ABOVE
const String options[] = {"Start", "Speed", "Distance", "k", "kp", "kd", "ki", "Thresh"};
/*
   Each option must have an action associated with it. Each action results
   in different menu behaviour.

   QUIT - Exit the menu and start some other code.
   EDIT - Adjust an integer variable using a knob.
   TOGGLE - Simply change from true to false without entering a sub-menu.
   DRESET - Sets a double value back to zero without entering a sub-menu.
   IRESET - Sets a integer value back to zero without entering a sub-menu.
*/
const String actions[] = {"QUIT", "EDIT", "DRESET", "EDIT", "EDIT", "EDIT", "EDIT", "EDIT"};

boolean inMenu = true;
int menuPos;
// ##### END MENU CONSTANTS #####

// PID
int last_error = 0;
int recent_error = last_error;
int current_time = 0;
int last_time = 0;

// interrupts
volatile unsigned int INT_2 = 0; // left wheel odometer 
volatile unsigned int INT_1 = 0; // right wheel odometer 
int interrupt_count = 0;

int od_time = 0;
bool stopped = false;


void setup() {
#include <phys253setup.txt>
  Serial.begin(9600);
  enableExternalInterrupt(INT2, FALLING);
  enableExternalInterrupt(INT1, FALLING);

  LCD.print("Booting...");
  delay(BOOT_DELAY);

  speed = readEEPROM(SPEED_ADDR);
  kp = readEEPROM(KP_ADDR);
  kd = readEEPROM(KD_ADDR);
  ki = readEEPROM(KI_ADDR);
  k = readEEPROM(K_ADDR);
  thresh = readEEPROM(THRESH_ADDR);
}

void loop() {
  if (!inMenu) {
    double dis_right = (double) WHEEL_CIRCUMFERENCE / WHEEL_DIVS * ((double) INT_1 / 2.0) * DISTANCE_COMPENSATOR;
    double dis_left = (double) WHEEL_CIRCUMFERENCE / WHEEL_DIVS * ((double) INT_2 / 2.0) * DISTANCE_COMPENSATOR;
    distance = (dis_right + dis_left) / 2.0;
    printDistance(distance); 
  } 

//  if (stopped || inMenu) {
//    motor.speed(LEFT_MOTOR, 0);
//    motor.speed(RIGHT_MOTOR, 0);
//  } else if (distance <  TO_RAMP - CHASSIS_LENGTH) {
//    speed = 110;
//  } else if (distance >= TO_RAMP && distance < (TO_RAMP + RAMP_LENGTH + CHASSIS_LENGTH)) {
//    speed = 200; //intial value 120
//  } else if (distance >= 510.0) {
//    stopped = true;
//    motor.speed(LEFT_MOTOR, 0);
//    motor.speed(RIGHT_MOTOR, 0);
//  } else {
//    speed = 110 ;
//  } 

  if (inMenu) {
    displayMenu();
  } else if (!stopped) {
    pid();
  }
}

uint16_t readEEPROM(int addressNum) {
  uint16_t* address = (uint16_t*)(2 * addressNum);
  return eeprom_read_word(address);
}

void writeEEPROM(int addressNum, uint16_t val) {
  uint16_t* address = (uint16_t*)(2 * addressNum);
  eeprom_write_word(address, val);
}

void printDistance(double distance) {
  LCD.clear();
   LCD.print("Dis: ");
  LCD.print(distance);
//  LCD.print("R: ");
//  LCD.print(INT_1);
//  LCD.setCursor(0, 1);
//  LCD.print("L: ");
//  LCD.print(INT_2);
}

void pid() {
  int error = 0;

  int left = analogRead(LEFT_QRD);
  int right = analogRead(RIGHT_QRD);

  LCD.setCursor(0, 1);
  LCD.print("L:");
  LCD.print(left);
  LCD.print(" R:");
  LCD.print(right);

  if (left > thresh && right > thresh) error = 0;
  if (left < thresh && right > thresh) error = -1;
  if (left > thresh && right < thresh) error = 1;
  if (left < thresh && right < thresh)  {
    if (last_error < 0) error = -OFF_TAPE_ERROR;
    if (last_error >= 0) error = OFF_TAPE_ERROR;
  }
 
  if (error != last_error) {
    recent_error = last_error;
    last_time = current_time;
    current_time = 1;
  }

  int prop = k * kp * error;
  int deriv = k * (int) (kd * (float) (error - recent_error) / (float) (last_time + current_time));
  int integral = k * (int) (0.5 * ki * (error - recent_error) * (last_time + current_time) );

  if (integral > INT_THRESH) {
    integral = INT_THRESH;
  } else if (integral < - INT_THRESH) {
    integral = - INT_THRESH;
  }

  current_time++;
  last_error = error;
  int control = prop + deriv + integral; 

  motor.speed(LEFT_MOTOR, speed - control);
  motor.speed(RIGHT_MOTOR, speed + control);
  delay(10);

  if (stopbutton() || startbutton()) {
    inMenu = true;
    INT_2 = 0; // reset distance
    motor.speed(LEFT_MOTOR, 0);
    motor.speed(RIGHT_MOTOR, 0);
    delay(500);
  }
}

/*  Enables an external interrupt pin
  INTX: Which interrupt should be configured?
    INT0    - will trigger ISR(INT0_vect)
    INT1    - will trigger ISR(INT1_vect)
    INT2    - will trigger ISR(INT2_vect)
    INT3    - will trigger ISR(INT3_vect)
  mode: Which pin state should trigger the interrupt?
    LOW     - trigger whenever pin state is LOW
    FALLING - trigger when pin state changes from HIGH to LOW
    RISING  - trigger when pin state changes from LOW  to HIGH
*/
void enableExternalInterrupt(unsigned int INTX, unsigned int mode) {
  if (INTX > 3 || mode > 3 || mode == 1) return;
  cli();
  /* Allow pin to trigger interrupts        */
  EIMSK |= (1 << INTX);
  /* Clear the interrupt configuration bits */
  EICRA &= ~(1 << (INTX * 2 + 0));
  EICRA &= ~(1 << (INTX * 2 + 1));
  /* Set new interrupt configuration bits   */
  EICRA |= mode << (INTX * 2);
  sei();
}

void disableExternalInterrupt(unsigned int INTX) {
  if (INTX > 3) return;
  EIMSK &= ~(1 << INTX);
}

// right wheel
ISR(INT1_vect) {
  INT_1++;
  delay(10);
}

// left wheel 
ISR(INT2_vect) {
  long currentTime = millis();
  leftWheelSpeed = WHEEL_CIRCUMFERENCE / WHEEL_DIVS / (float) (currentTime - lastLeftWheelTime) / 1000.0;
  lastLeftWheelTime = currentTime;
  INT_2++;
  delay(10); 
}

// ##### MENU FUNCTIONS #####
int getValInt(int i) {
  if (options[i] == "Speed") {
    return speed;
  } else if (options[i] == "k") {
    return k;
  } else if (options[i] == "kp") {
    return kp;
  } else if (options[i] == "kd") {
    return kd;
  } else if (options[i] == "ki") {
    return ki;
  } else if (options[i] == "Thresh") {
    return thresh;
  } else {
    return 0;
  }
}
void setValInt(int i, int val) {
  if (options[i] == "Speed") {
    speed = val;
    writeEEPROM(SPEED_ADDR, speed);
  } else if (options[i] == "k") {
    k = val;
    writeEEPROM(K_ADDR, k);
  } else if (options[i] == "kp") {
    kp = val;
    writeEEPROM(KP_ADDR, kp);
  } else if (options[i] == "kd") {
    kd = val;
    writeEEPROM(KD_ADDR, kd);
  } else if (options[i] == "ki") {
    ki = val;
    writeEEPROM(KI_ADDR, ki);
  } else if (options[i] == "Thresh") {
    thresh = val;
    writeEEPROM(THRESH_ADDR, thresh);
  }
}
double getValDouble(int i) {
  if (options[i] == "Distance") {
    return distance;
  } else {
    return 0.0;
  }
}

void setValDouble(int i, double val) {
  if (options[i] == "Distance") {
    INT_2 = val;
    INT_1 = val;
    distance = 0;
  }
}
boolean getValBool(int i) {
  return true;
}
void setValBool(int i, boolean val) {

}

void displayMenu() {
  // populate the LCD
  int menuReading = abs(knob(KNOB) - MAX); // to make motion more intuitive
  menuPos = getMenuPos(menuReading);
  populateMenuLCD();

  boolean select = startbutton();
  if (select) {
    String action = actions[menuPos];

    if (action == "QUIT") {
      inMenu = false;
    } else if (action == "EDIT") {
      // edit variable with knob
      delay(500);
      while (true) {
        LCD.clear();
        LCD.print(options[menuPos]);
        LCD.setCursor(0, 1);
        LCD.print("Old:");
        LCD.print(getValInt(menuPos));

        int newVal = knob(KNOB);
        int scale = getScale(knob(SCALE_KNOB));
        newVal /= scale;
        LCD.print(" New:");
        LCD.print(newVal);

        if (stopbutton()) {
          break;
        }
        if (startbutton()) {
          setValInt(menuPos, newVal);
        }
        delay(50);
      }
    } else if (action == "TOGGLE") {
      // toggle boolean
      if (getValBool(menuPos)) {
        setValBool(menuPos, false);
      } else {
        setValBool(menuPos, true);
      }
    } else if (action == "DRESET") {
      // reset double value to zero
      setValDouble(menuPos, 0.0);
    } else if (action == "IRESET") {
      // reset integer value to zero
      setValInt(menuPos, 0);
    }
    delay(300 - MENU_REFRESH);
  }
  delay(MENU_REFRESH);
}

int getMenuPos(int menuReading) {
  int pos = 0;
  double bound = MENU_KNOB_DIV;

  while (pos < MENU_OPTIONS - 1) {
    if (menuReading < bound) {
      break;
    }
    pos += 1;
    bound += MENU_KNOB_DIV;
  }
  return pos;
}

int getScale(int scaleReading) {
  if (scaleReading < MAX / 3.0) {
    return 1;
  } else if (scaleReading < MAX * 2.0 / 3.0) {
    return 10;
  } else {
    return 100;
  }
}

void populateMenuLCD() { // TODO: add values to the menu
  LCD.clear();
  if (menuPos == MENU_OPTIONS - 1) {
    LCD.print(options[menuPos - 1] + " ");
    LCD.print(getMenuVal(menuPos - 1));
    LCD.setCursor(0, 1);
    LCD.print("*" + options[menuPos] + " ");
    LCD.print(getMenuVal(menuPos));
  } else {
    LCD.print("*" + options[menuPos] + " ");
    LCD.print(getMenuVal(menuPos));
    LCD.setCursor(0, 1);
    LCD.print(options[menuPos + 1] + " ");
    LCD.print(getMenuVal(menuPos + 1));
  }
}

String getMenuVal(int i) {
  if (actions[i] == "QUIT") {
    return "";
  } else if (actions[i] == "EDIT" || actions[i] == "IRESET") {
    return String(getValInt(i)); 
  } else if (actions[i] == "TOGGLE") {
    if (getValBool(i)) {
      return "T";
    } else {
      return "F";
    }
  } else if (actions[i] == "DRESET") {
    return String(getValDouble(i));
  }
}
// ##### END MENU FUNCTIONS #####
