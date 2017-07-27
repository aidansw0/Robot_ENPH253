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
  LCD.clear();
  LCD.print("Searching");
  
  double r = getRCircularArc(startAlpha, r0, alpha0, R);
  LCD.clear();
  LCD.print(r);
  moveArmCyl(startAlpha, r, z + 30);
  delay(1000);
  LCD.clear();
  LCD.print("ready claw");
  readyClaw();

  if (endAlpha < startAlpha) {
    endAlpha--;
  } else {
    endAlpha++;
  }

  for (int alpha = startAlpha; alpha != endAlpha; alpha += sign(endAlpha - startAlpha)) {
    r = getRCircularArc(alpha, r0, alpha0, R);
    moveArmCyl(alpha, r, z);
    LCD.clear();
    LCD.print("A:");
    LCD.print(alpha);
    LCD.print(" r:");
    LCD.print(r);
    if (checkForObject()) {
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
  delay(500);
  moveArmAng(course * 40, 60, 30);
  delay(500);
  moveAlpha(0);
  delay(500);
  moveArmAng(0, 40, 0);
  closeClaw();
}

void dropInBox (int side) {
  moveArmAng((RCServo2.read() - 90) / 90.0 * 135.0, 60, -30);
  if (side == LEFT) {
    moveAlpha(ALPHA_BOX_LEFT);
    delay(500);
    moveArmCyl(ALPHA_BOX_LEFT, Z_BOX, R_BOX);
  } else if (side == RIGHT) {
    moveAlpha(ALPHA_BOX_RIGHT);
    delay(500);
    moveArmCyl(ALPHA_BOX_RIGHT, Z_BOX, R_BOX);
  }
  delay(1000);
  openClaw();
}

double getRCircularArc (int alpha, double r0, double alpha0, double R) {
  double tempCos = cos( (alpha - alpha0) * M_PI / 180.0 );
  return r0 * tempCos - sqrt(r0 * r0 * (tempCos * tempCos - 1) + R * R);
}

int sign(double x) {
  if (x >= 0.0)
    return 1;
  else
    return -1;
}

