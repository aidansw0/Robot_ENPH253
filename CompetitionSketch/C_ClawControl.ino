// ClawControl

/*
 * Must be called before the claw is used to initiallize
 * its position and sensors.
 */
void setupClaw() {
  pinMode(CLAW_QRD_PIN, INPUT);
  pinMode(GRAB_SENSOR_PIN, INPUT);
}

/*
 * Call this function to calibrate a successful grab. Close
 * the claw with an agent completely inside the arms such that
 * the servo is not drawing any current to maintain its position,
 * then call this function.
 */
void calibrateClawQRD(boolean LCDprint) {
  double avgQrd = 0.0;
  for (int i = 0; i < 100; i++) {
    avgQrd += analogRead(CLAW_QRD_PIN);
  }
  avgQrd /= 100.0;
  closedReading = avgQrd;
  if (LCDprint) {
    LCD.print("QRD: ");
    LCD.print(closedReading);
    LCD.print(" ");
  }
  writeEEPROM(CLAW_QRD_CALIBRATION_ADDR, closedReading);
}
/*
 * Call to calibrate a successful grab. Place an agent such
 * that the servo is drawing current then call this function.
 */
void calibrateClawGrabStress(boolean LCDprint) {
  double avgGrab = 0.0;
  for (int i = 0; i < 100; i++) {
    avgGrab += analogRead(GRAB_SENSOR_PIN);
  }
  avgGrab /= 100.0;
  closedStressVoltage = avgGrab;
  if (LCDprint) {
    LCD.print("Stress: ");
    LCD.print(closedStressVoltage);
    LCD.print(" ");
  }
  writeEEPROM(CLAW_GRAB_STRESS_ADDR, closedStressVoltage);
}

/*
 * Call to calibrate a successful grab. Place an agent such
 * that the servo is NOT drawing current then call this function.
 */
void calibrateClawGrabEmpty(boolean LCDprint) {
  double avgGrab = 0.0;
  for (int i = 0; i < 100; i++) {
    avgGrab += analogRead(GRAB_SENSOR_PIN);
  }
  avgGrab /= 100.0;
  closedEmptyVoltage = avgGrab;
  if (LCDprint) {
    LCD.print("Empty: ");
    LCD.print(closedEmptyVoltage);
    LCD.print(" ");
  }
  writeEEPROM(CLAW_GRAB_EMPTY_ADDR, closedEmptyVoltage);
}

/*
 * Call this before making a search pass with the claw.
 */
void readyClaw() {
  openClaw();
  enableClawQrd();
  fillHistory();
}

void enableClawQrd() {
  digitalWrite(CLAW_QRD_ENABLE, HIGH);
}

void disableClawQrd() {
  digitalWrite(CLAW_QRD_ENABLE, LOW);
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
  RCServo2.write(CLAW_SERVO_CLOSE);
  delay(GRAB_DELAY);

  double servoAvg = 0.0;
  for (int i = 0; i < 100; i++) {
    servoAvg += analogRead(GRAB_SENSOR_PIN);
  }
  servoAvg /= 100.0;

  int qrdReading = analogRead(CLAW_QRD_PIN);
  float servoRange = abs(closedStressVoltage - closedEmptyVoltage) * 0.25;
  if ((qrdReading >= closedReading * CLAW_QRD_THRESHOLD && qrdReading <= closedReading * (1 + CLAW_QRD_THRESHOLD)) || 
        servoAvg < closedEmptyVoltage - servoRange) {
    return true;
  } else {
    return false;
  }
}

void openClaw() {
  RCServo2.write(CLAW_SERVO_OPEN);
  delay(GRAB_DELAY);
}

void fillHistory() {
  for (int i = 0; i < CLAW_QRD_HISTORY; i++) {
    history[i] = analogRead(CLAW_QRD_PIN);
    delay(5);
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
