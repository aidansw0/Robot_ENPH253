void enableIR (int side) {
  if (side != irSide) {
    if (side == LEFT) {
      digitalWrite(IR_SWITCH, HIGH);
      irSide = LEFT;
    } else if (side == RIGHT) {
      digitalWrite(IR_SWITCH, LOW);
      irSide = RIGHT;
    }
    delay(10);
  }
}

int compareIR () {
  enableIR(LEFT);
  int leftReading = analogRead(IR);
  enableIR(RIGHT);
  int rightReading = analogRead(IR);

  if (leftReading - IR_COMP_THRESH > rightReading)
    return LEFT;
  else if (rightReading - IR_COMP_THRESH > leftReading)
    return RIGHT;
  else
    return 0;
}

