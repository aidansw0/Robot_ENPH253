#include <phys253.h>          
#include <LiquidCrystal.h>    
#include <avr/interrupt.h> // enable interrupts

#define WHEEL_DIAMETER 1.0 // in cm
#define WHEEL_DIVS 18.0
#define WHEEL_CIRCUMFERENCE 1.0 //(PI * WHEEL_DIAMETER)

volatile unsigned int INT_1 = 0;

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

ISR(INT1_vect) {
  INT_1++;
}
 
void setup() {
  #include <phys253setup.txt> 
  enableExternalInterrupt(INT1, FALLING);
}
 
void loop() {
  if (startbutton() || stopbutton()) {
    INT_1 = 0;
  }

  double distance = INT_1 / WHEEL_DIVS * WHEEL_CIRCUMFERENCE;
  LCD.clear();
  LCD.print("distance:");
  LCD.setCursor(0, 1);
  LCD.print(distance);
  LCD.print(" cm");

  delay(50);
}
