#include <LiquidCrystal.h>
#include <AccelStepper.h>
#include <Wire.h>
#include "Menu_Characters.h"

//----- Bipolar Stepper Motor 42SHDDC4043Z-22B -------//
const int EN_MOT = 26;
const int STEPS  = 24;
const int DIR    = 22;
const int motorInterfaceType = 1;
AccelStepper stepper = AccelStepper(motorInterfaceType, STEPS, DIR);
//----------------------------------------------------//


//------- Sensor FC-123 --------//
const int IR     = 28;
const int enable = 29;
//-----------------------------//


// ------Switch Limit---------//
const int IR0    = 42;
//---------------------------//


//--------- LEDs -------------//
const int LED_INF  = 48;
const int LED_SU   = 36;
const int LED_Stop = 37;
const int LED      = 38;
//Delay
const int delay_LED = 10;
const int  delay_IR = 50; 
//
float count   = 0;
//Current state of LED
int         a = 1;
//LED max value
int led   = 126;
//----------------------------//


//-----LCD pin to Arduino------//
const int pin_RS = 8;  
const int pin_EN = 9;  
const int pin_d4 = 4;  
const int pin_d5 = 5;  
const int pin_d6 = 6;  
const int pin_d7 = 7;  
const int pin_BL = 10;
//----------------------------//


//------ Button value --------//
int button;
int cursorPosition = 0;
int read_key;
//---------------------------//


//-----Control parameters-----//
unsigned int syringe   = 0;
       float rate      = 0;
         int dir       = 1;
unsigned int occlusion = 0;
//---------------------------//


//---- Function prototypes-----//
void menuItem1();
void menuItem2();
void menuItem3();
void menuItem4();
void menuItem5();
void menuItem6();
//-----------------------------//


//----------------- Array of pointers to function --------------------------------------//
void (*menuItem[])() = {menuItem1, menuItem2, menuItem3, menuItem4, menuItem5, menuItem6};
String menuItems[] = {"SETTINGS","SYRINGE TYPE", "FLOW RATE", "FLOW DIRECT.", "OCCLUSION DET", "START"};
//--------------------------------------------------------------------------------------//


//----- Navigation button variables -----//
int readKey;
int savedDistance = 0;
//--------------------------------------//


//------ Menu control variables --------//
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
//--------------------------------------//


LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

void setup() {
    
  // Initializes serial communication
  Serial.begin(9600);
  // Set driver stepper motor pins
  pinMode(EN_MOT, OUTPUT);
  pinMode(   DIR, OUTPUT);
  pinMode( STEPS, OUTPUT);
  //
  digitalWrite(EN_MOT,HIGH);
  digitalWrite(   DIR,HIGH);
  // Set max stepper motor speed
  stepper.setMaxSpeed(1000);

  // Initializes and clears the LCD screen
  lcd.begin(16, 2);
  lcd.clear();

  lcd.setCursor(1,0);
  lcd.print("Syringe Driver");
  lcd.setCursor(2,1);
  lcd.print("I==[");
  lcd.createChar(4, menuSyringe);
  lcd.createChar(5, menuFill);
  lcd.setCursor(6,1);
  lcd.write(byte(4));
  lcd.setCursor(7,1);
  lcd.write(byte(4));
  lcd.setCursor(8,1);
  lcd.write(byte(5));
  lcd.setCursor(9,1);
  lcd.write(byte(5));
  lcd.setCursor(10,1);
  lcd.write(byte(5));
  lcd.setCursor(11,1);
  lcd.print("]--");
  
  delay(3000);
  lcd.clear();

  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
}


void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}

void mainMenuDraw() {
  Serial.print(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
} 

// Erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {     // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  // The menu is set up to be progressive: menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4, 
  
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // menu -> even && cursor position -> even => cursor -> line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // menu -> even && cursor position -> odd => cursor -> line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // menu -> odd && cursor position -> even => cursor -> line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // menu -> odd && cursor position -> odd => cursor -> line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}

void operateMainMenu() {
  while(1)
      {
        read_key = analogRead(0);
        delay(200);
        if(read_key < 790) break;
      }

  button = key_press(read_key);
  
  switch(button){
  case 0:
        button = 0;
        switch (cursorPosition) {
          case 0:
            menuItem1();
            break;
          case 1:
            menuItem2();
            break;
          case 2:
            menuItem3();
            break;
          case 3:
            menuItem4();
            break;
          case 4:
            menuItem5();
            break;
          case 5:
            menuItem6();
        }
        mainMenuDraw();
        drawCursor();
        break;
  case 1:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        break;    
  
  case 2:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        break;
  }
}

int key_press(int key_val){
 
    return (key_val < 60 )? 0 :      // RIGHT
           (key_val < 200)? 1 :      // UP
           (key_val < 400)? 2 :      // DOWN
           (key_val < 600)? 3 : 4;   // LEFT : SELECT
}

//----------- SETTINGS----------------//
void menuItem1() { // case 0 from main menu
  }          
//-----------------------------------//

//-------- SYRINGE TTYPE ------------//
void menuItem2() { // case 1 from main menu
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("> SYRINGE:   ");
    lcd.setCursor(13,0);
    lcd.print(syringe,DEC);
    lcd.setCursor(0,1);
    lcd.print("              OK");
    syringe = submenu(syringe, 10, 3, 0, 0, 100, 2);
} 
//-----------------------------------//

//----------- FLOW RATE -------------//
void menuItem3() { // case 2 from main menu
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("> FLOW RATE:   ");
    lcd.setCursor(13,0);
    lcd.print(rate,DEC);
    lcd.setCursor(0,1);
    lcd.print("  ml/min      OK");
    rate = submenu(rate, 0.5, 6, 3, 0, 100, 3);
  }
//-----------------------------------//

//--------- FLOW DIRECTION ----------//
void menuItem4() { // case 3 from main menu
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("> FLOW DIR.:   ");
    lcd.setCursor(13,0);
    lcd.print(dir,DEC);
    lcd.setCursor(0,1);
    lcd.print("              OK");
    dir = submenu(dir, 2, 2, 0, -1, 1, 4);
  }
//-----------------------------------//

//----------- TEMPERATURE -----------//
void menuItem5() { // case 4 from main menu
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("> OCC. DET.:   ");
    lcd.setCursor(13,0);
    lcd.print(occlusion,DEC);
    lcd.setCursor(0,1);
    lcd.print("              OK");
    occlusion = submenu(occlusion, 5, 3, 0, 0, 30, 5);
  }
//-----------------------------------//


//--------------START----------------//
void menuItem6() { // case 5 from main menu
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Calibration....");
    light();
    int i = motor(rate,syringe,dir);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("End");
    delay(2000);
  }
//-------------------------------------//

  
//-------------Light System--------------//
void light()
{
while(1)
  {
  read_key = analogRead(0);
  if(read_key < 790) break;
  
 // Read data from sensor 
  int ir_read = digitalRead(IR);
  
  // Print data to the serial port in monitor
 // Serial.println(ir_read);
  delay(delay_IR);

  while(!ir_read)
  {
    delay(50);
    ir_read = digitalRead(IR);
    Serial.println(ir_read);
  }
  
  while(ir_read && (count <= 1))
  {
    delay(delay_IR);
    count = count + 0.05;
    Serial.println(ir_read);
    Serial.println(count);
    ir_read = digitalRead(IR);
  }

  if(count < 1)
  {
    if(a)
    {
       analogWrite(LED,126);
       count = 0;
    }
    else
    {
       analogWrite(LED,0);  
       count = 0;
    }
  }
  else
  {
    if(a)
    {
      led = 0;
      for( /*no initialization */; led < 126; led++) 
        {
          analogWrite(LED, led);
          delay(delay_LED); 
          count = 0;
        }
      }
     else
     {
      for( /*no initialization */; led >= 0; led--) 
        {
          analogWrite(LED, led);
          delay(delay_LED); 
          count = 0;
        }
      }
  }
  while(ir_read)
  {
    delay(50);
    ir_read = digitalRead(IR);
    Serial.println(ir_read);
    }
  a = !a;
//  Serial.println("a = ");
//  Serial.println(a);
}
}


//----------------STEPPER MOTOR-------------------//
int motor(int rate, int syringe, int dir)
{
  int number_of_steps;
  float radius =  (syringe == 10)? 7.25  :
                  (syringe == 20)? 9.565 :
                  (syringe == 30)? 10.85 :
                  (syringe == 40)? 10.85 : 13.35;
                      
  int speed_mot = 1000 * (rate/(1.884*radius*radius));
  
  int plunger   = (syringe == 20)? 70 :
                  (syringe == 30)? 75 :
                  (syringe == 40)? 80 :
                  (syringe == 50)? 87 :
                  (syringe == 60)? 102:
                  (syringe == 70)? 102:
                  (syringe == 80)? 102:
                  (syringe == 90)? 102:
                  (syringe == 100)? 102: 0;
                  
  digitalWrite(EN_MOT, LOW);
  if(dir > 0)
  {
    number_of_steps = (113 - plunger)/2.0 * 200; 
  }
  else 
  {
    number_of_steps = 113/2.0 * 200;
  }
  
  while (digitalRead(IR0)) {  // Do this until the switch is activated   
    stepper.setSpeed(-400);
    stepper.runSpeed();
  }
  Serial.println(digitalRead(IR0));
  delay(2000);
 
  stepper.setCurrentPosition(0);
  
  while(stepper.currentPosition() != number_of_steps)
  {
    stepper.setSpeed(400);
    stepper.runSpeed();
  }
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("If it's ready to");
  lcd.setCursor(0,1);
  lcd.print("use press SELECT");

  while(1)
      {
        read_key = analogRead(0);
        delay(200);
        if(read_key < 790) break;
      }
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("rate: ");
    lcd.setCursor(5,1);
    lcd.print(speed_mot,DEC);
    lcd.setCursor(9,1);
    lcd.print(" srn: ");
    lcd.setCursor(14,1);
    lcd.print(plunger,DEC);

    if(dir == 1)
    {
      digitalWrite(LED_INF, HIGH);
      lcd.setCursor(0,0);  
      lcd.print("> Infusion pump  ");
      number_of_steps = plunger/2.0 * 200;
      stepper.setCurrentPosition(0);
      while(stepper.currentPosition() != number_of_steps)
      {
      stepper.setSpeed(speed_mot);
      stepper.runSpeed();
      }    
    }
    else if(dir == -1)
    {
      digitalWrite(LED_SU, HIGH);
      lcd.setCursor(0,0);  
      lcd.print("> Suction pump  ");
      number_of_steps = plunger/2.0 * 200;
      stepper.setCurrentPosition(0);
      while(stepper.currentPosition() != -number_of_steps)
      {
      stepper.setSpeed(-speed_mot);
      stepper.runSpeed();
      }    
    }
    digitalWrite(LED_INF, LOW);
    digitalWrite(LED_SU,  LOW);
    digitalWrite(EN_MOT,  HIGH);
  return 0;
}
//------------------------------------------------//


//-------------------SubMenu----------------------//
float submenu(float x, float y, int pos, int precision, int min_constrain, int max_constrain, int i)
{
    int a = 1;
    char str[7];
    while(a)
      {
        read_key = analogRead(0);
        delay(200);
        if(read_key < 790)
        {
        button = key_press(read_key);
        switch(button)
        {
        case 0:
            x = x + y;
            x = constrain(x, min_constrain, max_constrain);
            dtostrf(x, pos, precision, str);
            lcd.setCursor(12,0);
            lcd.print("     ");
            lcd.setCursor(12,0);
            lcd.print(str);
            break;
        case 3:
            x = x - y;
            x = constrain(x, min_constrain, max_constrain);
            dtostrf(x, pos, precision, str);
            lcd.setCursor(12,0);
            lcd.print("     ");
            lcd.setCursor(12,0);
            lcd.print(str);
            break;
        case 2:
            lcd.setCursor(0, 1);
            lcd.write(byte(0));
            lcd.setCursor(0, 0);
            lcd.print(" ");
            while(1)
            {
              read_key = analogRead(0);
              delay(200);
              if(read_key < 790) break;
            }
            button = key_press(read_key);
            switch(button)
            {
              case 1:
                  lcd.clear();
                  menuItem[i-1]();
                  a = 0;
                  break;
              case 0:
                  //lcd.clear();
                  //mainMenuDraw();
                  //drawCursor();
                  //operateMainMenu();
                  a = 0;
                  break;
            }
            break;
        }
        } 
        if(a == 0) break;
      }
      return x;

}
