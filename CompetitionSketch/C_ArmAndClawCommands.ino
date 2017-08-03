// ArmAndClawCommands

boolean searchAlpha(int startAlpha, int endAlpha, double r, double z, double zGrabOffset) {
  moveArmCyl(startAlpha, r, z);
  delay(500);
  readyClaw();

  if (endAlpha < startAlpha) {
    endAlpha--;
  } else {
    endAlpha++;
  }

  for (int alpha = startAlpha; alpha != endAlpha; alpha += sign(endAlpha - startAlpha)) {
    moveAlpha(alpha);
    if (checkForObject()) {
      delay(200);
      moveArmCyl(alpha, r, z - zGrabOffset);
      if (closeClaw()) {
        moveArmCyl(alpha, r, z);
        return true;
      } else {
        moveArmCyl(alpha, r, z);
        openClaw();
      }
    }
    delay(SWEEP_DELAY);
  }


  return false;
}

boolean searchTankArc (int startAlpha, int endAlpha, double R, double z, double r0, double alpha0, double zGrabOffset) {
  double r = getRCircularArc(startAlpha, r0, alpha0, R);
  openClaw();
  moveArmCyl(startAlpha, r, z + 40);
  readyClaw();

  if (endAlpha < startAlpha) {
    endAlpha--;
  } else {
    endAlpha++;
  }

  for (int alpha = startAlpha; alpha != endAlpha; alpha += sign(endAlpha - startAlpha)) {
    long timer = millis();
    r = getRCircularArc(alpha, r0, alpha0, R);
    moveArmCyl(alpha, r, z);
    if (checkForObject()) {
      moveArmCyl(alpha, r, z - zGrabOffset);
      delay(50);
      if (closeClaw()) {
        moveArmCyl(alpha, r, z);
        return true;
      } else {
        moveArmCyl(alpha, r, z);
        openClaw();
        delay(200);
      }
    }
    while (millis() < timer + SWEEP_DELAY) delay(1);
  }

  return false;
}

void stowArm() {
  armPID(60);
  moveAlpha(course * 40);
  delay(500);
  moveArmAng(course * 40, 10, -35);
  delay(500);
  moveAlpha(course * 20);
  delay(50);
  moveAlpha(course * 18);
}

void deployArm () {
  moveAlpha(course * 40);
  moveArmAng(course * 40, 60, 30); 
  moveAlpha(0);
  moveArmAng(0, 45, 0);
  closeClaw();
}

void dropInBox (int side) {
  RCServo1.write(50);
  armPID(80);
  //moveArmAng((RCServo2.read() - 90) / 90.0 * 135.0, 80, 45);
  if (side == LEFT) {
    moveAlpha(ALPHA_BOX_LEFT + 10);
    delay(300);
    moveArmCyl(ALPHA_BOX_LEFT + 10, R_BOX, Z_BOX);
    moveAlpha(ALPHA_BOX_LEFT);
    //moveArmCyl(ALPHA_BOX_LEFT, R_BOX, Z_BOX);
  } else if (side == RIGHT) {
    moveAlpha(ALPHA_BOX_RIGHT - 10);
    delay(300);
    moveArmCyl(ALPHA_BOX_RIGHT - 10, R_BOX, Z_BOX);
    moveAlpha(ALPHA_BOX_RIGHT);
    //moveArmCyl(ALPHA_BOX_RIGHT, R_BOX, Z_BOX);
  }
  delay(600);
  openClaw();
}

double getRCircularArc (int alpha, double r0, double alpha0, double R) {
  double tempCos = cos( (alpha - alpha0) * M_PI / 180.0 );
  return r0 * tempCos - sqrt(r0 * r0 * (tempCos * tempCos - 1) + R * R);
}

double getMaxAlphaOffset (double r0, double R) {
  return floor(acos(sqrt(1 - R * R / r0 / r0)) * 180.0 / M_PI);
}

int sign(double x) {
  if (x >= 0.0)
    return 1;
  else
    return -1;
}

