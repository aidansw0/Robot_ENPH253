//ClawControl

/*
 * Must be called before the claw is used to initiallize
 * its position and sensors.
 */
void setupClaw() {
  pinMode(CLAW_QRD_PIN, INPUT);
  pinMode(GRAB_SENSOR_PIN, INPUT);
}

/*
 * Call this function to calibrate a successful grab. CLose
 * the claw with an agent completely inside the arms such that
 * the servo is not drawing any current to maintain its position,
 * then call this function.
 */
void calibrateClaw() {
  double avgQrd = 0.0;
  for (int i = 0; i < 100; i++) {
    avgQrd += analogRead(CLAW_QRD_PIN);
  }
  avgQrd /= 100.0;
  closedReading = avgQrd;

  double avgGrab = 0.0;
  for (int i = 0; i < 100; i++) {
    avgGrab += analogRead(GRAB_SENSOR_PIN);
  }
  avgGrab /= 100.0;
  closedVoltage = avgGrab;
}

/*
 * Call this before making a search pass with the claw.
 */
void readyClaw() {
  openClaw();
  fillHistory();
}

/*
 * Returns: true if the claw detects an object, false otherwise.
 */
boolean checkForObject() {
  double avg  ; 
  int qrdReading ;
  qrdReading = analogRead(CLAW_QRD_PIN);
  avg = averageHistory();
  if (qrdReading < avg * (1 - GRAB_THRESHOLD)) {
    return true;
  } else {
    //addToHistory(qrdReading);
    return false;
  }

}

/*
 * Returns: true if the claw successfully grabbed an object,
 *          false otherwise.
 */
boolean closeClaw() {
  RCServo0.write(CLAW_SERVO_CLOSE);
  delay(GRAB_DELAY);

  double servoAvg = 0.0;
  for (int i = 0; i < 100; i++) {
    servoAvg += analogRead(GRAB_SENSOR_PIN);
  }
  servoAvg /= 100.0;

  int qrdReading = analogRead(CLAW_QRD_PIN);
  if ((qrdReading >= closedReading * CLAW_QRD_THRESHOLD && qrdReading <= closedReading * (1 + CLAW_QRD_THRESHOLD)) || 
        servoAvg < closedVoltage - GRAB_VOLTAGE_THRESHOLD) {
    return true;
  } else {
    return false;
  }
}

void openClaw() {
  RCServo0.write(CLAW_SERVO_OPEN);
  delay(GRAB_DELAY);
}

void fillHistory() {
  for (int i = 0; i < CLAW_QRD_HISTORY; i++) {
    history[i] = analogRead(CLAW_QRD_PIN);
    delay(20);
  }
}

void addToHistory(int a) {
  for (int i = CLAW_QRD_HISTORY - 2; i >= 0; i--) {
    history[i+1] = history[i];
  }
  history[0] = a;
}

int averageHistory() {
  double sum = 0.0;
  for (int i = 0; i < CLAW_QRD_HISTORY; i++) {
    sum += history[i];
  }
  return sum / CLAW_QRD_HISTORY; 
}
