// Don't add any functions here. setup and loop only.

void setup() {
#ifndef phys253setup_txt
#define phys253setup_txt
  portMode(0, INPUT) ;      //   ***** from 253 template file
  portMode(1, OUTPUT) ;      //   ***** from 253 template file
  LCD.begin(16, 2) ;
  RCServo0.attach(RCServo0Output) ;    // attaching the digital inputs to the RC servo pins on the board.
  RCServo1.attach(RCServo1Output) ;
  RCServo2.attach(RCServo2Output) ;
#endif

  RCServo0.write(130);
  RCServo1.write(90);
  RCServo2.write(90);

  Serial.begin(9600);
  enableExternalInterrupt(INT2, FALLING);
  enableExternalInterrupt(INT1, FALLING);
  disableClawQrd();

  LCD.print("Booting...");
  delay(BOOT_DELAY);

  speed = readEEPROM(SPEED_ADDR);
  kp = readEEPROM(KP_ADDR);
  kd = readEEPROM(KD_ADDR);
  ki = readEEPROM(KI_ADDR);
  k = readEEPROM(K_ADDR);
  thresh = readEEPROM(THRESH_ADDR);
  closedReading = readEEPROM(CLAW_QRD_CALIBRATION_ADDR);
  closedStressVoltage = readEEPROM(CLAW_GRAB_STRESS_ADDR);
  closedEmptyVoltage = readEEPROM(CLAW_GRAB_EMPTY_ADDR);
}

void loop() {
  //Switch PID after ramp
  if (inMenu) {
    displayMenu();
  } else {
    if (gatePassed) {
      if (getDistance() >= IR_GATE_DISTANCE + GATE_TO_RAMP_DISTANCE + RAMP_LENGTH + POST_RAMP_DISTANCE) {
        kp = 12;
        kd = 9;
        ki = 0;
        speed = 110;
        turnOffset = 20;
      } else if (getDistance() >= IR_GATE_DISTANCE + GATE_TO_RAMP_DISTANCE + RAMP_LENGTH) {
        speed = 255;
      } else if (getDistance() >= IR_GATE_DISTANCE + GATE_TO_RAMP_DISTANCE) {
        speed = 255;
      }
    }

    //Wait at IR gate for a cycle
    int lastReading;
    while (!gatePassed && getDistance() >= IR_GATE_DISTANCE - 15.0) {
      getError();
      int readingIR;

      // enters this part once when first stops
      if (!stopped) {
        while (speed > 30) {
          speed -= 5;
          pid();
          delay(3);
        }
        speed = 255;
        stopped = true;

        // brakes motors
        motor.speed(LEFT_MOTOR, -10);
        motor.speed(RIGHT_MOTOR, -10);
        lastReading = analogRead(IR);
        getError();
        deployArm();
      } else {
        lastReading = analogRead(IR);
        delay(10);
      }
      readingIR = analogRead(IR);

      if (lastReading - readingIR > GATE_IR_THRESH) {
        stopped = false;
        gatePassed = true;
        RCServo2.write(CLAW_SERVO_CLOSE);
        moveArmAng(0, 35, 15);
        delay(100);
        moveArmAng(0, 30, -15);
      }
    }

    if (!stopped) {
      hashmark();
      pid();
    }
  }
}
