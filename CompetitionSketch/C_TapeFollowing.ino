void pid() {
  int left = analogRead(LEFT_QRD);
  int right = analogRead(RIGHT_QRD);

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

  motor.speed(LEFT_MOTOR , speed - turnOffset - control);
  motor.speed(RIGHT_MOTOR, speed + turnOffset + control);
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
      turnOffset = 65;
      kp = 11;
      kd = 5;
      deployArm();
    }
    
    if (hash == 1) {
      //tank T
      motor.speed(LEFT_MOTOR, 200);
      motor.speed(RIGHT_MOTOR, -200);
      delay(150);
      last_error = -5;
      speed = 100;
      kp = 11;
      kd = 5;
    } else if (hash == 2 || hash == 4 || hash == 6) {
      //Stop at every other hashmark
      motor.speed(LEFT_MOTOR, speed);
      motor.speed(RIGHT_MOTOR, speed);
      delay(140);
      motor.speed(LEFT_MOTOR, 0);
      motor.speed(RIGHT_MOTOR, 0);
      last_error = 5;
      if (searchTankArc(135, 45, AGENT_TANK_R, agentHeights[hash-1] + DEFAULT_Z_GRAB_OFFSET)) {
        dropInBox(LEFT);
      }
      if (searchTankArc(135, 45, AGENT_TANK_R, agentHeights[hash] + DEFAULT_Z_GRAB_OFFSET)) {
        dropInBox(RIGHT);
      }
    } else if (hash == 10) {
      //Go to zipline at third hash
      long timer = millis();
      motor.speed(SCISSOR_MOTOR, SCISSOR_UP);
      motor.speed(LEFT_MOTOR, 100);
      motor.speed(RIGHT_MOTOR, 100);
      while (millis() < timer + 1000)
        if (digitalRead(!UP_SWITCH)) 
          motor.speed(SCISSOR_MOTOR, 0);
          
      timer = millis();
      motor.speed(LEFT_MOTOR, 200);
      motor.speed(RIGHT_MOTOR, -200);
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
      delay(10000);
    } else {
      //Don't stop
      motor.speed(LEFT_MOTOR, speed - turnOffset);
      motor.speed(RIGHT_MOTOR, speed + turnOffset);
      delay(200);
    }
  }
}

