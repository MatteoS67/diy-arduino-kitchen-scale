#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"


const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;


const int TARE_BUTTON_PIN = 13; 
const int UNIT_BUTTON_PIN = 12; 


float calibration_factor = 103.17; 

HX711 scale;


LiquidCrystal_I2C lcd(0x27, 16, 2);

int unitState = 0; // 0 = g, 1 = oz, 2 = ml
bool lastUnitButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  
  pinMode(TARE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UNIT_BUTTON_PIN, INPUT_PULLUP);
  
  lcd.init(); 
  lcd.backlight(); 
  lcd.setCursor(0, 0); 
  lcd.print("Booting"); 

  unsigned long previousMillis = millis();
  int dotCount = 0;
  
  while (dotCount < 3) {
    if (millis() - previousMillis >= 300) {
      lcd.print(".");
      dotCount++;
      previousMillis = millis(); // Reset the clock for the next dot
    }
  }
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); 
  
  delay(500);
  lcd.clear();
}


void loop() {
  //Tare button
  if (digitalRead(TARE_BUTTON_PIN) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Taring");

    unsigned long previousMillis = millis();
   int dotCount = 0;
  
   while (dotCount < 3) {
    if (millis() - previousMillis >= 300) {
      lcd.print(".");
      dotCount++;
      previousMillis = millis(); 
    }
  }
    scale.tare();
    delay(500); 
    lcd.clear();
  }

  //Unit toggle button
  bool currentUnitButtonState = digitalRead(UNIT_BUTTON_PIN);
  if (currentUnitButtonState == LOW && lastUnitButtonState == HIGH) {
    unitState++;
    if (unitState > 2) {
      unitState = 0; 
    }
    lcd.clear(); 
    delay(200); 
  }
  lastUnitButtonState = currentUnitButtonState;

  //Read and display
  if (scale.is_ready()) {
    float weight = scale.get_units(5); 
    
    lcd.setCursor(0, 0);
    lcd.print("Weight:");

    lcd.setCursor(0, 1);
    if (unitState == 0) {
      lcd.print(weight, 0); 
      lcd.print(" g      ");
      
    } else if (unitState == 1) {
      float oz = weight * 35.274; 
      lcd.print(oz, 1); 
      lcd.print(" oz      ");
    } else if (unitState == 2) {
      float ml = weight ; 
      lcd.print(ml, 0); 
      lcd.print(" ml      ");
    }
  } 
   
}