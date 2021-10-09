/**
 * MUH Ilmava Temperature monitoring screen
 * Arduino: Mega2560
 * Temperature sensors: 4 x DHT22
 * + Ethernet Card
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
// LIBRARIES
// #include <ArduinoJson.h>
// #include <SPI.h>
// #include <Dhcp.h>
// #include <Dns.h>
// #include <Ethernet.h>
// #include <EthernetClient.h>
// #include <EthernetServer.h>
// #include <EthernetUdp.h>
#include <dht.h>
// #include <Adafruit_SSD1306.h>
// #include <splash.h>
// #include <Wire.h>
// #define OLED_RESET 4
// Adafruit_SSD1306 display(OLED_RESET);

// --------------------------------------------------------------------------------
// ## IP ADDRESS ##
const char* serverAddress = "192.168.1.10";

// --------------------------------------------------------------------------------
// ## PINS CONFIGURATION ##
#define DHT22_1_PIN 50
#define DHT22_2_PIN 51
#define DHT22_3_PIN 52
#define DHT22_4_PIN 53


// --------------------------------------------------------------------------------
// ## VARIABLES ##
dht DHT;
double incomingAirTemp = 0.0;
int incomingAirHumidity = 0;
double outgoingAirToRoomsTemp = 0.0;
int outgoingAirToRoomsHumidity = 0;
double returningRoomsAirTemp = 0.0;
int returningRoomsAirHumidity = 0;
double afterHeatingCoilTemp = 0.0;
int afterHeatingCoilHumidity = 0;
// unsigned long OLED_SCREEN_I2C_ADDRESS = 0x00;
long timePreviousMeassure = 0;


void setup() {  
  Serial.begin(9600);
  delay(2000);
  // OLED_SCREEN_I2C_ADDRESS = findOledScreen();
  // if (OLED_SCREEN_I2C_ADDRESS != 0x00) {
  //  Serial.println("--> " + OLED_SCREEN_I2C_ADDRESS);
  //  display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_I2C_ADDRESS);
  //  display.clearDisplay();
  // }
}


void loop() {
  if (millis() - timePreviousMeassure > 10000) {
    timePreviousMeassure = millis();
    dhtRead(); 
    // writeOledScreenText();
  }
}



// --------------------------------------------------------------------------------------------------------------------------
// -------------------------------------A L L - S E N S O R - R E A D I N G - T A S K S -------------------------------------
// --------------------------------------------------------------------------------------------------------------------------

// Read DHT temperature and humidity
void dhtRead() {  
  int chk = DHT.read22(DHT22_1_PIN);  
  incomingAirTemp = DHT.temperature;
  incomingAirHumidity = (DHT.humidity * 2);
  Serial.print("Incoming Air temp: ");
  Serial.print(incomingAirTemp);
  Serial.println("°C");
  Serial.print("Incoming Air humidity: ");
  Serial.print(incomingAirHumidity);
  Serial.println("rH");

  chk = DHT.read22(DHT22_2_PIN);  
  outgoingAirToRoomsTemp = DHT.temperature;
  outgoingAirToRoomsHumidity = (DHT.humidity * 2);
  Serial.print("Outgoing Air To Rooms temp: ");
  Serial.print(outgoingAirToRoomsTemp);
  Serial.println("°C");
  Serial.print("Outgoing Air To Rooms humidity: ");
  Serial.print(outgoingAirToRoomsHumidity);
  Serial.println("rH");

  chk = DHT.read22(DHT22_3_PIN);  
  returningRoomsAirTemp = DHT.temperature;
  returningRoomsAirHumidity = (DHT.humidity * 2);
  Serial.print("Returning Rooms Air temp: ");
  Serial.print(returningRoomsAirTemp);
  Serial.println("°C");
  Serial.print("Returning Rooms Air humidity: ");
  Serial.print(returningRoomsAirHumidity);
  Serial.println("rH");

  chk = DHT.read22(DHT22_4_PIN);  
  afterHeatingCoilTemp = DHT.temperature;
  afterHeatingCoilHumidity = (DHT.humidity * 2);
  Serial.print("After Heating Coil temp: ");
  Serial.print(afterHeatingCoilTemp);
  Serial.println("°C");
  Serial.print("After Heating Coil humidity: ");
  Serial.print(afterHeatingCoilHumidity);
  Serial.println("rH");

  Serial.println("------------------------------------");
}


/**
 * Write oled screen content
 * Good tutorial: https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/
 */
 /*
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
*/


/**
 * Find first responding i2c address
 * because we only have one i2c device connected (hopefully)
 */
 /*
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
*/
