void pid() {
  int left = analogRead(LEFT_QRD);
  int right = analogRead(RIGHT_QRD);

  LCD.setCursor(0, 1);
  LCD.print("L:");
  LCD.print(left);
  LCD.print(" R:");
  LCD.print(right);

  if (left > thresh && right > thresh) error = 0;
  if (left < thresh && right > thresh) error = -1;
  if (left > thresh && right < thresh) error = 1;
  if (left < thresh && right < thresh)  {
    if (last_error < 0) error = -OFF_TAPE_ERROR;
    if (last_error >= 0) error = OFF_TAPE_ERROR;
  }

  if (error != last_error) {
    recent_error = last_error;
    last_time = current_time;
    current_time = 1;
  }

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
    INT_2 = 0; // reset distance
    motor.speed(LEFT_MOTOR, 0);
    motor.speed(RIGHT_MOTOR, 0);
    delay(500);
  }
}

void hashmark() {
  int leftHash = digitalRead(LEFT_HASH);
  int rightHash = digitalRead(RIGHT_HASH);

  if ((leftHash == LOW || rightHash == LOW) && abs(error) < OFF_TAPE_ERROR) {
    hash++;
    LCD.clear();
    LCD.print(hash);

    if (hash == 2) {
      //First hashmark change PID
      turnOffset = 60;
      kp = 11;
      kd = 5;
    }

    if (hash == 1) {
      //tank T
      motor.speed(LEFT_MOTOR, course * 200);
      motor.speed(RIGHT_MOTOR, course * -200);
      delay(150);
      last_error = course * -5;
      speed = 100;
      kp = 12;
      kd = 5;
    } else if (/*hash == 2 || hash == 4 || hash == 6*/ hash <= 6 || hash == 8) {
      //Stop at every other hashmark
      motor.speed(LEFT_MOTOR, speed);
      motor.speed(RIGHT_MOTOR, speed);
      delay(140);
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      last_error = course * 5;
      /*for (int R = AGENT_TANK_R; R >= AGENT_TANK_R - 30; R -= 30) {
        if (searchTankArc(course * (TANK_ALPHA0 + getMaxAlphaOffset(TANK_R0, R) - 1), course * TANK_ALPHA0, R, agentHeights[hash - 1] + DEFAULT_Z_GRAB_OFFSET, TANK_R0, course * TANK_ALPHA0)) {
          dropInBox(LEFT);
          break;
        }
        moveBaseArmRel(20);
      }*/
      for (int R = AGENT_TANK_R; R >= AGENT_TANK_R - 30; R -= 30) {
        if (searchTankArc(course * TANK_ALPHA0, course * (TANK_ALPHA0 - getMaxAlphaOffset(TANK_R0, R)), R, agentHeights[hash] + DEFAULT_Z_GRAB_OFFSET, TANK_R0, course * TANK_ALPHA0)) {
          dropInBox(RIGHT);
          break;
        }
        moveBaseArmRel(20);
      }
      /*if (searchAlpha(course * 120, course * 45, 250, agentHeights[hash-1] + DEFAULT_Z_GRAB_OFFSET)) {
        dropInBox(LEFT);
        }
        if (searchAlpha(course * 120, course * 45, 250, agentHeights[hash-1] + DEFAULT_Z_GRAB_OFFSET)) {
        dropInBox(LEFT);
        }*/
      armPID(75);
    } else if (hash == 10) {
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      delay(100000);
      //Go to zipline at third hash
      zipline();
    } else {
      //Don't stop
      motor.speed(LEFT_MOTOR, speed - course * turnOffset);
      motor.speed(RIGHT_MOTOR, speed + course * turnOffset);
      delay(200);
    }
  }
}

void zipline () {
  long timer = millis();
  motor.speed(SCISSOR_MOTOR, SCISSOR_UP);
  motor.speed(LEFT_MOTOR, 100);
  motor.speed(RIGHT_MOTOR, 100);
  while (millis() < timer + 1000)
    if (digitalRead(!UP_SWITCH))
      motor.speed(SCISSOR_MOTOR, 0);

  timer = millis();
  motor.speed(LEFT_MOTOR, course * 200);
  motor.speed(RIGHT_MOTOR, course * -200);
  while (millis() < timer + 450)
    if (digitalRead(!UP_SWITCH))
      motor.speed(SCISSOR_MOTOR, 0);

  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  while (digitalRead(UP_SWITCH)) delay(1);

  motor.speed(SCISSOR_MOTOR, 0);
  motor.speed(LEFT_MOTOR, 50);
  motor.speed(RIGHT_MOTOR, 50);
  while (digitalRead(HOOK_SWITCH)) delay(1);

  timer = millis();
  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  motor.speed(SCISSOR_MOTOR, SCISSOR_DOWN);
  while (digitalRead(DOWN_SWITCH) && millis() < timer + 2000) delay(1);

  motor.speed(SCISSOR_MOTOR, 0);
  stopped = true;
  delay(100000);
}

