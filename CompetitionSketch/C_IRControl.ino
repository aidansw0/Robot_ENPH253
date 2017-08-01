void enableIR (int side) {
  if (side == LEFT) {
    digitalWrite(IR_SWITCH, HIGH);
  } else if (side == RIGHT) {
    digitalWrite(IR_SWITCH, LOW);
  }
}

int compareIR () {
  enableIR(LEFT);
  leftReading = analogRead(IR);
  enableIR(RIGHT);
  rightReading = analogRead(IR);
  
  if (leftReading - IR_COMP_THRESH > rightReading)       
    return LEFT;
  else if (rightReading - IR_COMP_THRESH > leftReading)  
    return RIGHT;
  else                                 
    return 0;
}

