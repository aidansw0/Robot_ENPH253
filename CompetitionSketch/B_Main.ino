// Don't add any functions here. setup and loop only.

void setup() {
  #include "phys253setup-modified.cpp"
  Serial.begin(9600);
//  enableExternalInterrupt(INT2, FALLING);
//  enableExternalInterrupt(INT1, FALLING);

  LCD.print("Booting...");
  delay(BOOT_DELAY);

  speed = readEEPROM(SPEED_ADDR);
  kp = readEEPROM(KP_ADDR);
  kd = readEEPROM(KD_ADDR);
  ki = readEEPROM(KI_ADDR);
  k = readEEPROM(K_ADDR);
  thresh = readEEPROM(THRESH_ADDR);
}
 
void loop() {
  //Switch PID after ramp
  if (gatePassed && millis() >= timerPID + /*5*/000) {
    timerPID += 200000;
    kp = 20;
    kd = 20;
    ki = 0;
    speed = 110;
  }

  //Wait at IR gate for a cycle
  while (!gatePassed && millis() >= timerPID + 1600) {
    stopped = true;
    motor.speed(LEFT_MOTOR, 0);
    motor.speed(RIGHT_MOTOR, 0);
    int readingIR = analogRead(IR);
    LCD.clear();
    LCD.print(readingIR);
    if (!newCycle) {
      if (readingIR > GATE_IR_THRESH) {
        newCycle = true;
      }
    }
    else if (readingIR < GATE_IR_THRESH) {
      stopped = false;
      gatePassed = true;
      timerPID = millis();
    }
  }

  if (inMenu) {
    displayMenu();
  } else if (!stopped) {
    hashmark();
    pid();
  }
}
