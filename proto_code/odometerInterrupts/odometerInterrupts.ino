#include <phys253.h>
#include <avr/interrupt.h> // enable interrupts
#include <LiquidCrystal.h>

#define WHEEL_DIAMETER 6 // in cm
#define WHEEL_DIVS 8 // a division is a black section followed by a white section on the encoder.
                     // assumes equal spacing and widths of divisions
#define WHEEL_CIRCUMFERENCE (PI * WHEEL_DIAMETER)

volatile unsigned int INT_2 = 0;

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
void enableExternalInterrupt(unsigned int INTX, unsigned int mode)
{
  if (INTX > 3 || mode > 3 || mode == 1) return;
  cli();
  /* Allow pin to trigger interrupts        */
  EIMSK |= (1 << INTX);
  /* Clear the interrupt configuration bits */
  EICRA &= ~(1 << (INTX*2+0));
  EICRA &= ~(1 << (INTX*2+1));
  /* Set new interrupt configuration bits   */
  EICRA |= mode << (INTX*2);
  sei();
}
 
void disableExternalInterrupt(unsigned int INTX)
{
  if (INTX > 3) return;
  EIMSK &= ~(1 << INTX);
}

int interrupt_count = 0;
// Interrupt routine
ISR(INT2_vect) {
  INT_2++;
  delay(10);
}

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  enableExternalInterrupt(INT2, FALLING);
}

void loop() {
  double distance = WHEEL_CIRCUMFERENCE / WHEEL_DIVS * (INT_2/2);
  LCD.clear();
  LCD.print("DISTANCE:");
  LCD.setCursor(0, 1);
  LCD.print(distance);
  LCD.print(" cm");
  delay(100);
}
