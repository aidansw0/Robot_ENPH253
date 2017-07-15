#include <phys253.h>
#include <avr/interrupt.h> // enable interrupts
#include <LiquidCrystal.h>
#include <avr/EEPROM.h>

// odometer
#define OD_TIMEOUT 40
#define WHEEL_DIAMETER 6.5 // in cm
#define WHEEL_DIVS 8 // a division is a black section followed by a white section on the encoder.
// assumes equal spacing and widths of divisions
#define WHEEL_CIRCUMFERENCE (PI * WHEEL_DIAMETER)
#define DISTANCE_COMPENSATOR 1.0

// PID
#define SETTINGS 4
#define LEFT_QRD A1
#define RIGHT_QRD A0
#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1
#define INT_THRESH 100
#define RAMP_LENGTH 125 // cm
#define TO_RAMP 285 // cm, distance to ramp from start
#define CHASSIS_LENGTH 30 // cm
#define OFF_TAPE_ERROR 5 // absolute value of error when neither QRD sees tape

// menu
#define KNOB_N 6
#define MENU_OPTIONS 6

// PID
int speed = 60;
int kp = 2;
int kd = 100;
int ki = 0;
int k = 2;
int thresh = 100;
int last_error = 0;
int recent_error = last_error;
int current_time = 0;
int last_time = 0;

// menu
boolean inMenu = true;
int menuPos = 0;
String options[] = {"Start", "Speed", "k", "kp", "ki", "kd", "Thresh"};

// interrupts
volatile unsigned int INT_2 = 0;
int interrupt_count = 0;

int od_time = 0;
bool stopped = false;

void setup() {
#include <phys253setup.txt>
  Serial.begin(9600);
  enableExternalInterrupt(INT2, FALLING);

  speed = readEEPROM(1);
}

void loop() {
  double distance;
  if (!inMenu) {
    distance = (double) WHEEL_CIRCUMFERENCE / WHEEL_DIVS * ((double) INT_2 / 2.0) * DISTANCE_COMPENSATOR;
    printDistance(distance);
  }

  if (stopped || inMenu) {
    motor.speed(LEFT_MOTOR, 0);
    motor.speed(RIGHT_MOTOR, 0);
  } else if (distance < TO_RAMP - CHASSIS_LENGTH) {
    speed = 70;
  } else if (distance >= TO_RAMP && distance < (TO_RAMP + RAMP_LENGTH + CHASSIS_LENGTH)) {
    speed = 120;
  } else if (distance >= 510.0) {
    stopped = true;
    motor.speed(LEFT_MOTOR, 0);
    motor.speed(RIGHT_MOTOR, 0);
  } else {
    speed = 70;
  }

  //  if (od_time % OD_TIMEOUT == 0) {
  //    od_time = 0;
  //    printDistance(distance);
  //  }
  //
  //  od_time += 1;

  if (inMenu) {
    menuDisplay();
  } else {
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

  //  LCD.clear();
  //  LCD.setCursor(0, 1);
  //  LCD.print("Error: ");
  //  LCD.print(error);
}

void menuDisplay() {
  // put your main code here, to run repeatedly:
  boolean select = startbutton();
  boolean next = stopbutton(); // cycle through menu options

  if (select) { // check menuPos, go to correct place
    next = false;
    String option = options[menuPos];
    boolean back = false;
    delay(500);
    if (option == "Start") {
      inMenu = false;
      //      kp = P;
      //      kd = D;
      //      ki = I;
      //      gain = G;
      //      threshold = Tape;
      delay(100);
    } else if (option == "Speed") {
      while (!back) {
        int reading = knob(KNOB_N);
        LCD.clear();
        LCD.print("Old: ");
        LCD.print(speed);
        LCD.setCursor(0, 1);
        LCD.print("New: ");
        LCD.print(reading);

        if (startbutton()) {
          speed = reading;
        }
        back = stopbutton();
        delay(100);
      }
      writeEEPROM(1, speed);

    } else if (option == "k") {
      while (!back) {
        int reading = knob(KNOB_N) / 10;
        LCD.clear();
        LCD.print("Old: ");
        LCD.print(k);
        LCD.setCursor(0, 1);
        LCD.print("New: ");
        LCD.print(reading);

        if (startbutton()) {
          k = reading;
        }
        back = stopbutton();
        delay(100);
      }

    } else if (option == "kp") {
      while (!back) {
        int reading = knob(KNOB_N) / 10;
        LCD.clear();
        LCD.print("Old: ");
        LCD.print(kp);
        LCD.setCursor(0, 1);
        LCD.print("New: ");
        LCD.print(reading);

        if (startbutton()) {
          kp = reading;
        }
        back = stopbutton();
        delay(100);
      }

    } else if (option == "ki") {
      while (!back) {
        int reading = knob(KNOB_N) / 10;
        LCD.clear();
        LCD.print("Old: ");
        LCD.print(ki);
        LCD.setCursor(0, 1);
        LCD.print("New: ");
        LCD.print(reading);

        if (startbutton()) {
          ki = reading;
        }
        back = stopbutton();
        delay(100);
      }

    } else if (option == "kd") {
      while (!back) {
        int reading = knob(KNOB_N) / 10;
        LCD.clear();
        LCD.print("Old: ");
        LCD.print(kd);
        LCD.setCursor(0, 1);
        LCD.print("New: ");
        LCD.print(reading);

        if (startbutton()) {
          kd = reading;
        }
        back = stopbutton();
        delay(100);
      }

    } else if (option == "thresh") {
      while (!back) {
        int reading = knob(KNOB_N);
        LCD.clear();
        LCD.print("Old: ");
        LCD.print(thresh);
        LCD.setCursor(0, 1);
        LCD.print("New: ");
        LCD.print(reading);

        if (startbutton()) {
          thresh = reading;
        }
        back = stopbutton();
        delay(100);
      }
    }
  }

  if (next) {
    if (menuPos >= MENU_OPTIONS) {
      menuPos = 0;
    } else {
      menuPos += 1;
    }
    delay(200);
  }

  int option2;
  if (menuPos + 1 >= MENU_OPTIONS) {
    option2 = 0;
  } else {
    option2 = menuPos + 1;
  }

  String val1 = "";
  if (options[menuPos] == "k") {
    val1 = (String) k;
  } else if (options[menuPos] == "kp") {
    val1 = (String) kp;
  } else if (options[menuPos] == "ki") {
    val1 = (String) ki;
  } else if (options[menuPos] == "kd") {
    val1 = (String) kd;
  } else if (options[menuPos] == "thresh") {
    val1 = (String) thresh;
  }

  String val2 = "";
  if (options[option2] == "k") {
    val2 = (String) k;
  } else if (options[option2] == "kp") {
    val2 = (String) kp;
  } else if (options[option2] == "ki") {
    val2 = (String) ki;
  } else if (options[option2] == "kd") {
    val2 = (String) kd;
  } else if (options[option2] == "thresh") {
    val2 = (String) thresh;
  }

  LCD.clear();
  LCD.print('*' + options[menuPos] + " " + val1);
  LCD.setCursor(0, 1);
  LCD.print(options[option2] + " " + val2);
  delay(100);
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

// Interrupt routine
ISR(INT2_vect) {
  INT_2++;
  delay(10);
}
