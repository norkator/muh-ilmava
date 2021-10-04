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

// pins
int thermistor_pin_1 = A0;
int thermistor_pin_2 = A1;
int thermistor_pin_3 = A2;
int thermistor_pin_4 = A3;

// variables
double incomingAirTemp = 0.0;
double outgoingAirToRoomsTemp = 0.0;
double returningRoomsAirTemp = 0.0;
double afterHeatingCoilTemp = 0.0;

unsigned long OLED_SCREEN_I2C_ADDRESS = 0x00;
long timePreviousMeassure = 0;
float BValue = 3470;
float R1 = 5000;
float T1 = 298.15;
float e = 2.718281828;


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

    float t1 = analogRead(thermistor_pin_1);
    float t2 = analogRead(thermistor_pin_2);
    float t3 = analogRead(thermistor_pin_3);
    float t4 = analogRead(thermistor_pin_4);

    incomingAirTemp = convertTemp(t1);
    outgoingAirToRoomsTemp = convertTemp(t2);
    returningRoomsAirTemp = convertTemp(t3);
    afterHeatingCoilTemp = convertTemp(t4);
    
    writeOledScreenText();
  }
}


/**
 * Convert read value to °C
 */
double convertTemp(int measurement) {
  float R2 = 1023 - measurement;
  float a = 1/T1;
  float b = log10(R1 / R2);
  float c = b / log10(e);
  float d = c / BValue ;
  float T2 = 1 / (a - d);
  return T2 - 273.15;
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
    display.print("Incmg.air: ");
    display.print((String) incomingAirTemp);
    
    display.setTextSize(1);
    display.setCursor(0,5);
    display.print("Out/In-R.: ");
    display.print((String) outgoingAirToRoomsTemp);
    display.print("/");
    display.print((String) returningRoomsAirTemp);

    display.setTextSize(1);
    display.setCursor(0,10);
    display.print("Aft.Ht.Coil: ");
    display.print((String) afterHeatingCoilTemp);

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
