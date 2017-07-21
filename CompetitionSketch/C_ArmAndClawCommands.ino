//ArmAndClawCommands

boolean searchAlpha(int startAlpha, int endAlpha, float r, float z, float zGrabOffset) {
  moveArmCyl(startAlpha, r, z);
  delay(200);
  readyClaw();
  
  if (endAlpha < startAlpha) {
    endAlpha--;
  } else {
    endAlpha++;
  }
  
  for (int alpha = startAlpha; alpha != endAlpha; alpha += sign(endAlpha-startAlpha)) {
    moveAlpha(alpha);
    if (checkForObject()) {
      moveArmCyl(alpha, r, z-zGrabOffset);
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

boolean searchTankArc (int startAlpha, int endAlpha, float R, float z, float r0, float alpha0, float zGrabOffset) {
  float r = getRCircularArc(startAlpha, r0, alpha0, R);
  moveArmCyl(startAlpha, r, z);
  delay(200);
  readyClaw();
  
  if (endAlpha < startAlpha) {
    endAlpha--;
  } else {
    endAlpha++;
  }
  
  for (int alpha = startAlpha; alpha != endAlpha; alpha += sign(endAlpha-startAlpha)) {
    r = getRCircularArc(alpha, r0, alpha0, R);
    moveArmCyl(alpha, r, z);
    if (checkForObject()) {
      moveArmCyl(alpha, r, z-zGrabOffset);
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

float getRCircularArc (int alpha, float r0, float alpha0, float R) {
  float tempCos = cos( (alpha-alpha0) * M_PI/180.0 );
  return r0 * tempCos - sqrt(r0 * r0 * (tempCos * tempCos - 1) + R * R);
}

int sign(double x) {
  if (x >= 0.0)
    return 1;
  else
    return -1;
}
