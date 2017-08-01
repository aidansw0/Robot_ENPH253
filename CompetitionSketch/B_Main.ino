// Don't add any functions here. setup and loop only.

  void setup() {
  #ifndef phys253setup_txt
  #define phys253setup_txt
    portMode(0, INPUT) ;      //   ***** from 253 template file
    portMode(1, OUTPUT) ;      //   ***** from 253 template file
    LCD.begin(16,2) ;
    RCServo0.attach(RCServo0Output) ;    // attaching the digital inputs to the RC servo pins on the board.  
    RCServo1.attach(RCServo1Output) ;
    RCServo2.attach(RCServo2Output) ;
  #endif

  RCServo0.write(120);
  RCServo1.write(90);
  RCServo2.write(90);
  
  Serial.begin(9600);
//  enableExternalInterrupt(INT2, FALLING);
//  enableExternalInterrupt(INT1, FALLING);
  disableExternalInterrupt(INT2);
  disableExternalInterrupt(INT1);
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
    if (gatePassed && millis() >= timerPID + 5800) {
      timerPID += 200000;
      kp = 13;
      kd = 5;
      ki = 0;
      speed = 110;
    }
  
    //Wait at IR gate for a cycle
    while (!gatePassed && millis() >= timerPID + 2000) {
      if (!stopped) {
        stopped = true;
        motor.speed(LEFT_MOTOR, 0);
        motor.speed(RIGHT_MOTOR, 0);
        deployArm();
        newCycle = false;
      }
      
      int readingIR = analogRead(IR);
      delay(10);
      if (!newCycle) {
        if (readingIR > GATE_IR_THRESH) {
          newCycle = true;
        }
      } else if (readingIR < GATE_IR_THRESH) {
        stopped = false;
        gatePassed = true;
        moveArmAng(0, 35, -15);
        timerPID = millis();
      }
    }
  
    if (!stopped) {
      hashmark();
      pid();
    }
  }
}
