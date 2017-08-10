void enableIR (int side) {
  if (side != irSide) {
    if (side == LEFT) {
      digitalWrite(IR_SWITCH, HIGH);
      irSide = LEFT;
    } else if (side == RIGHT) {
      digitalWrite(IR_SWITCH, LOW);
      irSide = RIGHT;
    }
    //delay(10);
  }
}
