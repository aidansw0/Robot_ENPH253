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
