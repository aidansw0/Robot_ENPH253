#include <phys253.h>          
#include <LiquidCrystal.h>    

#define SETTINGS 4

int speed, theta, phi;
int last_error = 0;
int recent_error = last_error;
int current_time = 0;
int last_time = 0;

void setup()
{
    #include <phys253setup.txt>
    Serial.begin(9600);  
}
 
void loop()
{
    theta = knob(6)/5.8;
    speed = (knob(7)-511.5)/1023.0*510.0;
  //Checks every 40 cycles
  if(current_time % 40 == 0) {
    LCD.clear();
    LCD.print("Theta: ");
    LCD.print(theta);
    LCD.setCursor(0,1);
    LCD.print("Spd: ");
    LCD.print(speed);
  }
  
  RCServo0.write(theta);
  motor.speed(0,speed);

  current_time += 1;

  delay(10);

  }
  
