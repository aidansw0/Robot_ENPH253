// TINAHMenu

uint16_t readEEPROM(int addressNum) {
  uint16_t* address = (uint16_t*)(2 * addressNum);
  return eeprom_read_word(address);
}

void writeEEPROM(int addressNum, uint16_t val) {
  uint16_t* address = (uint16_t*)(2 * addressNum);
  eeprom_write_word(address, val);
}

int getValInt(int i) {
  if (options[i] == "Speed") {
    return speed;
  } else if (options[i] == "k") {
    return k;
  } else if (options[i] == "kp") {
    return kp;
  } else if (options[i] == "kd") {
    return kd;
  } else if (options[i] == "ki") {
    return ki;
  } else if (options[i] == "Thresh") {
    return thresh;
  } else {
    return 0;
  }
}

void setValInt(int i, int val) {
  if (options[i] == "Speed") {
    speed = val;
    writeEEPROM(SPEED_ADDR, speed);
  } else if (options[i] == "k") {
    k = val;
    writeEEPROM(K_ADDR, k);
  } else if (options[i] == "kp") {
    kp = val;
    writeEEPROM(KP_ADDR, kp);
  } else if (options[i] == "kd") {
    kd = val;
    writeEEPROM(KD_ADDR, kd);
  } else if (options[i] == "ki") {
    ki = val;
    writeEEPROM(KI_ADDR, ki);
  } else if (options[i] == "Thresh") {
    thresh = val;
    writeEEPROM(THRESH_ADDR, thresh);
  }
}

double getValDouble(int i) {
  return 0.0;
}

void setValDouble(int i, double val) {
}

boolean getValBool(int i) {
  if (options[i] == "Course") {
    if (course == LEFT) {
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}

void setValBool(int i, boolean val) {
  if (options[i] == "DeployArm") {
    deployArm();
    LCD.clear();
    LCD.print("Arm Deployed");
    delay(1000);
  } else if (options[i] == "StowArm") {
    stowArm();
    LCD.clear();
    LCD.print("Arm Stowed");
    delay(1000);
  } else if (options[i] == "MoveScissor") {
    LCD.clear();
    LCD.print("Moving lift...");
    delay(1000);
    moveLift();
    LCD.clear();
    LCD.print("Done moving!");
    delay(500);
  } else if (options[i] == "DropInBox") {
    LCD.clear();
    LCD.print("Dropping on:");
    LCD.setCursor(0, 1);
    if (course == LEFT) {
      LCD.print("LEFT");
    } else {
      LCD.print("RIGHT");
    }
    dropInBox(course);
    
    LCD.clear();
    LCD.print("Press START");
    LCD.setCursor(0, 1);
    LCD.print("to raise arm.");
    while (!startbutton) {
      delay(1);
    }
    armPID(75);
  } else if (options[i] == "CalibrateClaw") {
    enableClawQrd();
    delay(500);
    boolean quit = false;
    LCD.clear();
    LCD.print("Press START");
    LCD.setCursor(0, 1);
    LCD.print("to open claw.");
    while (!startbutton()) {
      if (stopbutton()) {
        quit = true;
        break;
      }
      delay(1);
    }

    if (!quit) {
      delay(500);
      openClaw();
      LCD.clear();
      LCD.print("Put toy in claw");
      LCD.setCursor(0, 1);
      LCD.print("fully and START.");
      while (!startbutton()) {
        delay(1);
      }
      delay(1000);
      closeClaw();
      LCD.clear();
      LCD.print("Calibrating to:");
      LCD.setCursor(0, 1);
      delay(500);
      calibrateClawQRD(true);
      
      delay(1000);
      openClaw();
      LCD.clear();
      LCD.print("Empty claw");
      LCD.setCursor(0, 1);
      LCD.print("and press START.");
      while (!startbutton()) {
        delay(1);
      }
      delay(500);
      closeClaw();
      LCD.clear();
      LCD.print("Calibrating to:");
      LCD.setCursor(0, 1);
      delay(1000);
      calibrateClawGrabEmpty(true);

      delay(1000);
      openClaw();
      LCD.clear();
      LCD.print("Put toy part way");
      LCD.setCursor(0, 1);
      LCD.print("and press START.");
      while (!startbutton()) {
        delay(1);
      }
      delay(500);
      closeClaw();
      LCD.clear();
      LCD.print("Calibrating to:");
      LCD.setCursor(0, 1);
      delay(1000);
      calibrateClawGrabStress(true);

      delay(1000);
      openClaw();
    }
    delay(500);
    closeClaw();
    disableClawQrd();
  } else if (options[i] == "Course") {
    if (course == LEFT) {
      course = RIGHT;
    } else {
      course = LEFT;
    }
  }
}

void displayMenu() {
  // populate the LCD
  int menuReading = abs(knob(KNOB) - MAX); // to make motion more intuitive
  menuPos = getMenuPos(menuReading);
  populateMenuLCD();

  boolean select = startbutton();
  if (select) {
    String action = actions[menuPos];

    if (action == "QUIT") {
      inMenu = false;
      timerPID = millis();
    } else if (action == "EDIT") {
      // edit variable with knob
      delay(500);
      while (true) {
        LCD.clear();
        LCD.print(options[menuPos]);
        LCD.setCursor(0, 1);
        LCD.print("Old:");
        LCD.print(getValInt(menuPos));

        int newVal = knob(KNOB);
        LCD.print(" New:");
        LCD.print(newVal);

        if (stopbutton()) {
          break;
        }
        if (startbutton()) {
          setValInt(menuPos, newVal);
        }
        delay(50);
      }
    } else if (action == "TOGGLE") {
      // toggle boolean
      if (getValBool(menuPos)) {
        setValBool(menuPos, false);
      } else {
        setValBool(menuPos, true);
      }
    } else if (action == "DRESET") {
      // reset double value to zero
      setValDouble(menuPos, 0.0);
    } else if (action == "IRESET") {
      // reset integer value to zero
      setValInt(menuPos, 0);
    }
    delay(300 - MENU_REFRESH);
  }
  delay(MENU_REFRESH);
}

int getMenuPos(int menuReading) {
  int pos = 0;
  double bound = MENU_KNOB_DIV;
  
  while (pos < MENU_OPTIONS - 1) {
    if (menuReading < bound) {
      break;
    }
    pos += 1;
    bound += MENU_KNOB_DIV;
  }
  return pos;
}

void populateMenuLCD() { // TODO: add values to the menu
  LCD.clear();
  if (menuPos == MENU_OPTIONS - 1) {
    LCD.print(options[menuPos - 1] + " ");
    LCD.print(getMenuVal(menuPos - 1));
    LCD.setCursor(0, 1);
    LCD.print("*" + options[menuPos] + " ");
    LCD.print(getMenuVal(menuPos));
  } else {
    LCD.print("*" + options[menuPos] + " ");
    LCD.print(getMenuVal(menuPos));
    LCD.setCursor(0, 1);
    LCD.print(options[menuPos + 1] + " ");
    LCD.print(getMenuVal(menuPos + 1));
  }
}

String getMenuVal(int i) {
  if (options[i] == "Course") {
    if (course == LEFT) {
      return "LEFT";
    } else {
      return "RIGHT";
    }
  } else if (options[i] == "DeployArm" || options[i] == "StowArm" || options[i] == "CalibrateClaw" || options[i] == "MoveScissor") {
    return "";
  } else if (actions[i] == "QUIT") {
    return "";
  } else if (actions[i] == "EDIT" || actions[i] == "IRESET") {
    return String(getValInt(i));
  } else if (actions[i] == "TOGGLE") {
    if (getValBool(i)) {
      return "T";
    } else {
      return "F";
    }
  } else if (actions[i] == "DRESET") {
    return String(getValDouble(i));
  }
}
