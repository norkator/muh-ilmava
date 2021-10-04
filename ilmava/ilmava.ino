/**
 * MUH Ilmava Temperature monitoring screen
 *  
 *  Oled screen support
 *  To enable following screen support you need to install libs: "Adafruit BusIO", "Adafruit-GFX-Library" and "Adafruit_SSD1306"
 *  Display pins:
 *     SDA -> A4 (i2c)
 *     SCL -> A5 (i2c)
 *     GND -> Ground
 *     VDD -> 5v (Screen pin labeled as VCC or VDD)
 *
 */
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Wire.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


// variables
double incomingAirTemp = 0.0;
double afterHeatingCoilAirTemp = 0.0;
double heatRecoveryCellAirTemp = 0.0;

unsigned long OLED_SCREEN_I2C_ADDRESS = 0x00;
long timePreviousMeassure = 0;


void setup() {  
  Serial.begin(9600);
  delay(2000);
  OLED_SCREEN_I2C_ADDRESS = findOledScreen();
  if (OLED_SCREEN_I2C_ADDRESS != 0x00) {
    Serial.println("--> " + OLED_SCREEN_I2C_ADDRESS);
    display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);
    display.clearDisplay();
  }
}


void loop() {
  if (millis() - timePreviousMeassure > 10000) {
    // Todo.. read temperature probes here
    writeOledScreenText();
  }
}


/**
 * Write oled screen content
 * Good tutorial: https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/
 */
void writeOledScreenText() {
  if (OLED_SCREEN_I2C_ADDRESS != 0x00) {
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Incmg. air: ");
    display.print((String) incomingAirTemp);
    
    display.setTextSize(1);
    display.setCursor(0,5);
    display.print("Aft. heCoil: ");
    display.print((String) afterHeatingCoilAirTemp);

    display.setTextSize(1);
    display.setCursor(0,10);
    display.print("Aft. ht cell: ");
    display.print((String) heatRecoveryCellAirTemp);

    display.display();
  }
}


/**
 * Find first responding i2c address
 * because we only have one i2c device connected (hopefully)
 */
long findOledScreen() {
  Serial.println ("Scanning for I2C Oled screen.");
  byte count = 0;
  Wire.begin();
  for (byte i = 1; i < 127; i++) {
    // Serial.println("Scanning " + i);
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0) {
           
      Serial.print("Found i2c address: ");
      Serial.print(i, DEC);
      Serial.print(" | ");
      Serial.println(i, HEX);
      
      return i;

      count++;
      delay (1); 
      } 
  }
  return 0x00;
}
