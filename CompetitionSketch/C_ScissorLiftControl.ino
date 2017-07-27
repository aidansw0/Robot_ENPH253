void moveUp() {
  motor.speed(SCISSOR_MOTOR, SCISSOR_UP);
  while(digitalRead(UP_SWITCH)) {
    delay(1);
  }
  motor.speed(SCISSOR_MOTOR, 0);
}

void moveDown() {
  motor.speed(SCISSOR_MOTOR, SCISSOR_DOWN);
  while(digitalRead(DOWN_SWITCH)) {
    delay(1);
  }
  motor.speed(SCISSOR_MOTOR, 0);
}

/*
 * Chooses a direction based off of the current position of the lift.
 * Moves down by default.
 */
void moveLift() {
  if (digitalRead(DOWN_SWITCH)) {
    moveUp();
  } else {
    moveDown();
  }
}

