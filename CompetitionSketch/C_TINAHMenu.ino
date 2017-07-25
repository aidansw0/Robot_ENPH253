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
  if (options[i] == "Distance") {
    return distance;
  } else {
    return 0.0;
  }
}

void setValDouble(int i, double val) {
  if (options[i] == "Distance") {
    INT_2 = val;
    INT_1 = val;
    distance = val;
  }
}

boolean getValBool(int i) {
  return true;
}

void setValBool(int i, boolean val) {

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
  if (actions[i] == "QUIT") {
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
