/*
 * To calibrate the claw:
 *    1. Put in READY mode by pressing the stop button.
 *    2. Grab an agent such that it is completely inside the claw and the claw is completely closed
 *       with the servo off (no buzzing can be heard).
 *    3. Press the start button to save this calibration.
 *    4. Press the stop button to drop the agent. You can now use the claw properly.
 */

#include <phys253.h>
#include <LiquidCrystal.h>

#define QRD_PIN 6
#define GRAB_SENSOR_PIN 5
#define QRD_HISTORY 5
#define GRAB_THRESHOLD 0.1 // if the qrdReading goes below the average history by this fraction
#define GRAB_DELAY 200 // in milliseconds, time between sensing object and closing claw
#define SERVO_CLOSE 110 // angle value to give servo to close it
#define SERVO_OPEN 15  // angle value to give servo to open it

int history[QRD_HISTORY];
int closedReading = 0; // the QRD qrdReading when the claw is closed
int closedVoltage = 0; // the grab sensor voltage when the claw is properly closed
boolean clawOpen;
boolean clawReady;
String statusMsg;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  pinMode(QRD_PIN, INPUT);
  RCServo0.write(SERVO_OPEN);
  RCServo1.write(90);
  RCServo2.write(90);
  clawOpen = true;
  clawReady = false;
  statusMsg = "WAIT";
}

void loop() {
  int qrdReading = analogRead(QRD_PIN);
  int servoReading = analogRead(GRAB_SENSOR_PIN);
  
  if (startbutton()) {
    closedReading = qrdReading;
    closedVoltage = servoReading;
  }
  
  if (clawOpen && stopbutton()) {
    if (clawReady) {
      clawReady = false;
      statusMsg = "WAIT";
    } else {
      fillHistory();
      clawReady = true;
      statusMsg = "READY";
    }
  }
  
  double avg = averageHistory();
  if (qrdReading < avg * (1 - GRAB_THRESHOLD) && clawReady) {
    delay(GRAB_DELAY);
    RCServo0.write(SERVO_CLOSE);
    clawOpen = false;
    clawReady = false;
    statusMsg = "CLOSING";
    delay(GRAB_DELAY);
  } else if (clawReady) {
    addToHistory(qrdReading);
  } else if (!clawOpen) {
    
    double servoAvg = servoReading;
    for (int i = 0; i < 99; i++) {
      servoAvg += analogRead(GRAB_SENSOR_PIN);
    }
    servoAvg /= 100;
    
    if ((qrdReading >= closedReading * 0.5 && qrdReading <= closedReading * 1.5) || servoAvg < closedVoltage - 6) {
      statusMsg = "SUCCESS";
    } else {
      statusMsg = "FAILURE";
    }
  }

  if (!clawOpen && stopbutton()) {
    RCServo0.write(SERVO_OPEN);
    clawOpen = true;
    clawReady = false;
    statusMsg = "WAIT";
    delay(500);
  }
  
  LCD.clear();
  LCD.print("RD:");
  LCD.print(qrdReading);
  LCD.print(" CL:");
  LCD.print(closedReading);
  LCD.setCursor(0, 1);
  LCD.print(statusMsg);

  delay(50);
}

void fillHistory() {
  for (int i = 0; i < QRD_HISTORY; i++) {
    history[i] = analogRead(QRD_PIN);
    delay(50);
  }
}

void addToHistory(int a) {
  for (int i = QRD_HISTORY - 1; i >= 0; i--) {
    history[i+1] = history[i];
  }
  history[0] = a;
}

int averageHistory() {
  double sum = 0.0;
  for (int i = 0; i < QRD_HISTORY; i++) {
    sum += history[i];
  }
  return sum / QRD_HISTORY; 
}

