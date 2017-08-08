#include <phys253.h>
#include <LiquidCrystal.h>
#include <math.h>
#include <avr/EEPROM.h>
#include <avr/interrupt.h>

void setup() {
#ifndef phys253setup_txt
#define phys253setup_txt
  portMode(0, INPUT) ;      //   ***** from 253 template file
  portMode(1, OUTPUT) ;      //   ***** from 253 template file
  LCD.begin(16, 2) ;
  RCServo0.attach(RCServo0Output) ;    // attaching the digital inputs to the RC servo pins on the board.
  RCServo1.attach(RCServo1Output) ;
  RCServo2.attach(RCServo2Output) ;
#endif

  RCServo0.write(90);
  RCServo1.write(90);
  RCServo2.write(90);

  Serial.begin(9600);
  enableExternalInterrupt(INT2, FALLING);
  enableExternalInterrupt(INT1, FALLING);
  //  disableExternalInterrupt(INT2);
  //  disableExternalInterrupt(INT1);
}

#define WHEEL_DIAMETER 6.51
#define WHEEL_DIVS 9.0
#define WHEEL_CIRCUMFERENCE WHEEL_DIAMETER * M_PI
#define GO_DISTANCE 500 //Sonny

volatile double leftDistance = 0;
volatile double rightDistance = 0;
bool stopped = true;

void loop() {
  if (startbutton()) {
    stopped = false;
    leftDistance = 0;
    rightDistance = 0;
  }
  if (stopbutton() || leftDistance > GO_DISTANCE || rightDistance > GO_DISTANCE) {
    stopped = true;
  }
  if (!stopped) {
    motor.speed(0, 100);
    motor.speed(1, 95);
  } else {
    motor.speed(0, -20);
    motor.speed(1, -20);
  }
  LCD.clear();
  LCD.print("L:");
  LCD.print(leftDistance);
  LCD.setCursor(0, 1);
  LCD.print("R:");
  LCD.print(rightDistance);
  delay(10);
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
  rightDistance += WHEEL_CIRCUMFERENCE / WHEEL_DIVS;
}

// left wheel
ISR(INT2_vect) {
  leftDistance += WHEEL_CIRCUMFERENCE / WHEEL_DIVS;
}


