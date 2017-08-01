void enableIR (int side) {
  if (side == LEFT) {
    digitalWrite(IR_SWITCH, HIGH);
  } else if (side == RIGHT) {
    digitalWrite(IR_SWITCH, LOW);
  }
}

