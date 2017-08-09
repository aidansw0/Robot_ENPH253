void pid() {
  getError();

  int prop = k * kp * error;
  int deriv = k * (int) (kd * (float) (error - recent_error) / (float) (last_time + current_time));
  int integral = k * (int) (0.5 * ki * (error - recent_error) * (last_time + current_time) );
  //if (error == 0) integral = 0;

  if (integral > INT_THRESH) {
    integral = INT_THRESH;
  } else if (integral < - INT_THRESH) {
    integral = - INT_THRESH;
  }

  current_time++;
  last_error = error;
  int control = prop + deriv + integral;

  motor.speed(LEFT_MOTOR , speed - course * turnOffset - control);
  motor.speed(RIGHT_MOTOR, speed + course * turnOffset + control);
  delay(10);

  if (stopbutton() || startbutton()) {
    inMenu = true;
    motor.speed(LEFT_MOTOR, 0);
    motor.speed(RIGHT_MOTOR, 0);
    delay(500);
  }
}

void getError() {
  int left = analogRead(LEFT_QRD);
  int right = analogRead(RIGHT_QRD);

  if (left > thresh && right > thresh) error = 0;
  if (left < thresh && right > thresh) error = -1;
  if (left > thresh && right < thresh) error = 1;
  if (left < thresh && right < thresh)  {
    if (last_error < 0) error = -OFF_TAPE_ERROR;
    if (last_error >= 0) error = OFF_TAPE_ERROR;
  }
  error += errorOffset;

  if (error != last_error) {
    recent_error = last_error;
    last_time = current_time;
    current_time = 1;
  }
}

void hashmark() {
  int leftHash = digitalRead(LEFT_HASH);
  int rightHash = digitalRead(RIGHT_HASH);

  if (hash > 1) {
    enableIR(-course);
    if (analogRead(IR) > ZIPLINE_IR_THRESH) {
      if (hash >= 9) {
        zipline();
      } else {
        detectedIR = true;
      }
    } else {
      enableIR(course);
      if (analogRead(IR) > ZIPLINE_IR_THRESH) {
        if (hash >= 9) {
          //zipline();
        } else {
          detectedIR = true;
        }
      }
    }
  }

  if ((leftHash == LOW || rightHash == LOW) && abs(error) < OFF_TAPE_ERROR - 1) {
    hash++;
    LCD.clear();
    LCD.print(hash);

    if (hash == 2) {
      //First hashmark change PID
      turnOffset = -65;
      errorOffset = course * -1;
      kp = 20;
      kd = 5;
      ki = 0;
      speed = 110;
    } else if (hash == 7) {
      turnOffset = -65;
      kp = 20;
      kd = 5; 
      ki = 0;
      //errorOffset = 0;
      errorOffset = course * -1;
      speed = 110;
    }

    if (hash == 1) {
      //tank T
      RCServo1.write(10);
      errorOffset = course * -1;
      turnOffset = -65;
      speed = 110;
      motor.speed(LEFT_MOTOR, speed - course * 20);
      motor.speed(RIGHT_MOTOR, speed + course * 20);
      waitDistance(6);
      last_error = course * -1;
      kp = 20;
      kd = 5;
      ki = 0;
    } else if ((hash <= 6 || hash == 8 || hash == 9) && hash != 2) {
      //Stop at every hashmark
      motor.speed(LEFT_MOTOR, speed + course * turnOffset);
      motor.speed(RIGHT_MOTOR, speed - course * turnOffset);
      waitDistance(6);
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      last_error = course * -5;
      if (!detectedIR && hash == 3) {
        hash--;
      } else {
        hash3:
        armPID(80);
        moveArmAng(-course * (TANK_ALPHA0 - 8), 80, 0);
        for (int R = AGENT_TANK_R; R >= AGENT_TANK_R - 0; R -= 30) {
          if (searchTankArc(-course * (TANK_ALPHA0 - 8), -course * (TANK_ALPHA0 - getMaxAlphaOffset(TANK_R0, R)), R, agentHeights[hash] + DEFAULT_Z_GRAB_OFFSET, TANK_R0, -course * TANK_ALPHA0)) {
            if (hash % 2 == 0) dropInBox(course);
            else dropInBox(-course);
            break;
          }
          armPID(85);
          moveAlpha(-course * ALPHA_BOX_LEFT);
        }
        armPID(85);
        RCServo1.write(60);
      }
    } else if (hash < 9) {
      //Don't stop
      motor.speed(LEFT_MOTOR, speed - course * turnOffset);
      motor.speed(RIGHT_MOTOR, speed + course * turnOffset);
      waitDistance(6);
    }

    if (hash == 6) {
      double distance = getDistance();
      while (getDistance() - distance < 12) {
        pid();
      }
      speed = 90;
      motor.speed(LEFT_MOTOR, speed - course * -80);
      motor.speed(RIGHT_MOTOR, speed + course * -80);
      waitDistance(12);
      last_error = course * -5;
      hash++;
      speed = 110;
    } else if (hash >= 9) {
      if (detectedIR) {
        zipline();
      }

      motor.speed(LEFT_MOTOR, speed + course * turnOffset);
      motor.speed(RIGHT_MOTOR, speed - course * turnOffset);
      waitDistance(6);
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      last_error = course * -5;
    }

    detectedIR = false; //Clear IR detection after hash;
  }
}

void zipline () {
  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  armPID(90);
  RCServo1.write(120);
  moveAlpha(-course * 90);
  //moveArmAng(course * 90, 90, -45);
  motor.speed(SCISSOR_MOTOR, SCISSOR_UP);
  speed = 100;

  while (true) {
    //enableIR(-course);
    motor.speed(LEFT_MOTOR, speed - course * -35);
    motor.speed(RIGHT_MOTOR, speed + course * -35);
    if (/*analogRead(IR) > 100*/ true) {
      enableIR(course);
      while (analogRead(IR) < 150) {
        if (!digitalRead(UP_SWITCH)) {
          motor.speed(SCISSOR_MOTOR, 0);
        }
        delay(1);
      }
      while (analogRead(IR) > 130) {
        if (!digitalRead(UP_SWITCH)) {
          motor.speed(SCISSOR_MOTOR, 0);
        }
        delay(1);
      }
      //closeClaw();
      //RCServo1.write(-psiCal+70);
      //armPID(70);
      motor.speed(LEFT_MOTOR, -90);
      motor.speed(RIGHT_MOTOR, -90);
      delay(200);
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      while (digitalRead(UP_SWITCH)) delay(1);

      motor.speed(SCISSOR_MOTOR, 0);
      //moveArmAng(course * -90, 70, 0);
      //moveAlpha(course * -30);
      motor.speed(LEFT_MOTOR, 85);
      motor.speed(RIGHT_MOTOR, 85);
      lastInterrupt = millis();
      while (digitalRead(HOOK_SWITCH) && millis() - lastInterrupt < 2000) {
        delay(1);
      }

      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      long timer = millis();
      motor.speed(SCISSOR_MOTOR, SCISSOR_DOWN);
      while (millis() < timer + 4000) delay(1);
      motor.speed(LEFT_MOTOR, -90);
      motor.speed(RIGHT_MOTOR, -90);
      delay(1200);
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      while (digitalRead(DOWN_SWITCH)) delay(1);
      motor.speed(SCISSOR_MOTOR, 0);
      stopped = true;
      inMenu = true;
      LCD.clear();
      LCD.print("RESET ME!");
      while (true) delay(1000);
    }
    if (!digitalRead(UP_SWITCH)) {
      motor.speed(SCISSOR_MOTOR, 0);
    }
  }
}

