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
//  long currentTime = millis();
//  rightWheelSpeed = WHEEL_CIRCUMFERENCE / WHEEL_DIVS / (float) (currentTime - lastRightWheelTime) / 1000.0;
//  lastRightWheelTime = currentTime;
//  if (rightWheelSample % SPEED_SAMPLES == 0) {
//    rightWheelSample = 1;
//    lastLastRightWheelAvg = lastRightWheelAvg;
//    lastRightWheelAvg = rightWheelAvg / SPEED_SAMPLES;
//    rightWheelAvg = 0;
//  }
//  rightWheelAvg += rightWheelSpeed;
//  rightWheelSample++;
//  INT_1++;
//  delay(10);
}

// left wheel
ISR(INT2_vect) {
//  long currentTime = millis();
//  leftWheelSpeed = WHEEL_CIRCUMFERENCE / WHEEL_DIVS / (float) (currentTime - lastLeftWheelTime) / 1000.0;
//  lastLeftWheelTime = currentTime;
//  if (leftWheelSample % SPEED_SAMPLES == 0) {
//    leftWheelSample = 1;
//    lastLastLeftWheelAvg = lastLeftWheelAvg;
//    lastLeftWheelAvg = leftWheelAvg / SPEED_SAMPLES;
//    leftWheelAvg = 0;
//  }
//  leftWheelAvg += leftWheelSpeed;
//  leftWheelSample++;
//  INT_2++;
//  delay(10);
}
