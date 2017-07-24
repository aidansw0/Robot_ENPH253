#include <phys253.h> 
#include <LiquidCrystal.h>

#define CLAW_QRD_PIN A1
#define GRAB_SENSOR_PIN A2
#define CLAW_QRD_HISTORY 10 // used to help the claw identify objects with the QRD
#define GRAB_THRESHOLD 0.1 // if the servo reading goes below the average history by this fraction, the claw can grab
#define CLAW_QRD_THRESHOLD 0.5 // if the super QRD is +/- this percent of the calibrated reading, the grab is considered successful
#define GRAB_VOLTAGE_THRESHOLD 8 // if the raw analog reading of the servo voltage goes below the calibrated
                                 // value by this much, the grab is considered successful
#define GRAB_DELAY 1000 // in milliseconds, time between closing claw and checking for a successful grab
#define CLAW_SERVO_CLOSE 110 // angle value to give servo to close it
#define CLAW_SERVO_OPEN 15  // angle value to give servo to open it

int history[CLAW_QRD_HISTORY];
int closedReading = 0; // the QRD reading when the claw is closed
int closedVoltage = 0; // the grab sensor voltage when the claw is properly closed
 
void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
}
 
void loop() {

}

/*
 * Must be called before the claw is used to initiallize
 * its position and sensors.
 */
void setupClaw() {
  pinMode(CLAW_QRD_PIN, INPUT);
  pinMode(GRAB_SENSOR_PIN, INPUT);
  openClaw();
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
  fillHistory();
}

/*
 * Returns: true if the claw detects an object, false otherwise.
 */
boolean checkForObject() {
  int qrdReading = analogRead(CLAW_QRD_PIN);
  double avg = averageHistory();
  if (qrdReading < avg * (1 - GRAB_THRESHOLD)) {
    return true;
  } else {
    addToHistory(qrdReading);
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
  RCServo0.write(SERVO_OPEN);
}

void fillHistory() {
  for (int i = 0; i < CLAW_QRD_HISTORY; i++) {
    history[i] = analogRead(CLAW_QRD_PIN);
    delay(20);
  }
}

void addToHistory(int a) {
  for (int i = CLAW_QRD_HISTORY - 1; i >= 0; i--) {
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
