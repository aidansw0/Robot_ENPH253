/*
 * A menu for the TINAH which can be used to edit constants and start code.
 * Author: Sonny Cervienka
 * Date: July 14, 2017
 */
#include <phys253.h>
#include <LiquidCrystal.h>

// ##### YOUR VARIABLES #####
int var1 = 100;
int var2 = 780;
boolean bool1 = true;
boolean bool2 = false;
double reset1 = 56794.7;
int reset2 = 1024;

// ##### MENU CONSTANTS #####
#define MENU_OPTIONS 7 // number of options in the menu
#define KNOB 6 // the knob to use for scrolling and setting variables
#define SCALE_KNOB 7 // used to scale the first knob's input to help with selecting a variable
#define BOOT_DELAY 500 // gives the user time to set the TINAH down before menu starts
#define MENU_REFRESH 100 // menu refresh delay
#define MAX 1023 // analogRead maximum
#define MENU_KNOB_DIV ((double) (MAX + 1) / MENU_OPTIONS)

// BE SURE TO CHANGE THE MENU_OPTIONS VARIABLE ABOVE
String options[] = {"Start", "Var1", "Var2", "Bool1", "Bool2", "Reset1", "Reset2"};
/*
 * Each option must have an action associated with it. Each action results
 * in different menu behaviour.
 * 
 * QUIT - Exit the menu and start some other code.
 * EDIT - Adjust an integer variable using a knob.
 * TOGGLE - Simply change from true to false without entering a sub-menu.
 * DRESET - Sets a double value back to zero without entering a sub-menu.
 * IRESET - Sets a double value back to zero without entering a sub-menu.
 */
String actions[] = {"QUIT", "EDIT", "EDIT", "TOGGLE", "TOGGLE", "DRESET", "IRESET"};

boolean inMenu = true;
int menuPos;
// ##### END MENU CONSTANTS #####

// ##### MENU FUNCTIONS #####
int getValInt(int i) {
  if (options[i] == "Var1") {
    return var1;
  } else if (options[i] == "Var2") {
    return var2;
  } else if (options[i] == "Reset2") {
    return reset2;
  } else {
    return 0;
  }
}
void setValInt(int i, int val) {
  if (options[i] == "Var1") {
    var1 = val;
  } else if (options[i] == "Var2") {
    var2 = val;
  } else if (options[i] == "Reset2") {
    reset2 = val;
  }
}
double getValDouble(int i) {
  if (options[i] == "Reset1") {
    return reset1;
  } else {
    return 0.0;
  }
}
void setValDouble(int i, double val) {
  if (options[i] == "Reset1") {
    reset1 = val;
  }
}
boolean getValBool(int i) {
  if (options[i] == "Bool1") {
    return bool1;
  } else if (options[i] == "Bool2") {
    return bool2;
  } else {
    return false;
  }
}
void setValBool(int i, boolean val) {
  if (options[i] == "Bool1") {
    bool1 = val;
  } else if (options[i] == "Bool2") {
    bool2 = val;
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
        int scale = getScale(knob(SCALE_KNOB));
        newVal /= scale;
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

int getScale(int scaleReading) {
  if (scaleReading < MAX / 3.0) {
    return 1;
  } else if (scaleReading < MAX * 2.0 / 3.0) {
    return 10;
  } else {
    return 100;
  }
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
// ##### END MENU FUNCTIONS #####

void setup() {
  #include <phys253setup.txt>

  // ##### MENU CODE #####
  LCD.print("Booting...");
  delay(BOOT_DELAY);
  // ##### END MENU CODE #####
}

void loop() {
  if (inMenu) {
    displayMenu();
  } else {
    // YOUR CODE TO RUN HERE
    delay(100);
    LCD.clear();
    LCD.print("Running...");
    if (stopbutton()) {
      inMenu = true;
    }
  }
}

