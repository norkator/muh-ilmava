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
#include <ArduinoJson.h>
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <dht.h>

// --------------------------------------------------------------------------------
// ## IP ADDRESS ##
const char* serverAddress = "192.168.2.35";

// --------------------------------------------------------------------------------
// ## ETHERNET CONFIG ##
EthernetClient client;
EthernetServer ethernetServer(3800); // this specifies port
byte mac[] = { 0xFA, 0xF4, 0x9A, 0xC7, 0xC6, 0xC2 }; // Mac address for ethernet nic
const char* server  = serverAddress;
const char* api     = "/device/v1/measurements"; // API route
const int port      = 3800; // API listening port
const unsigned long HTTP_TIMEOUT = 10000; // Timeout for http call
const size_t MAX_CONTENT_SIZE = 124; // Must be incremented if http response comes out as null


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
int httpSkip = 0;


void setup() {  
  Serial.begin(9600);

  // init ethernet
  if (!Ethernet.begin(mac)) { // DHCP
    Serial.println("Ethernet configure failed!");
  } else {
    Serial.println("Ethernet init ok.");
    Serial.println(Ethernet.localIP());
  }
}


void loop() {
  if (millis() - timePreviousMeassure > 10000) {
    timePreviousMeassure = millis();
    dhtRead();
    httpPostMeasurements();
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


// --------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------- H T T P - A P I - C A L L - T A S K --------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------

// Http post operation
void httpPostMeasurements() {
    if (httpSkip == 0) {
      Serial.print("Http callback");
      if (connect(server)) {
        Serial.print(server); Serial.print(":"); Serial.print(port); Serial.println(api);
        if (sendRequest(server, api) && skipResponseHeaders()) {
        }
        disconnect();
      }
    }
    httpSkip = httpSkip + 1;
    if (httpSkip == 6) {
      httpSkip = 0; 
    };
}

// Open connection to the HTTP server
bool connect(const char* hostName) {
  Serial.println("");
  Serial.print("Connecting to: ");
  Serial.print(hostName); Serial.print(":"); Serial.print(port);
  Serial.println("");
  bool ok = client.connect(hostName, port);
  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP POST request to the server, same time GET needed response data
bool sendRequest(const char* host, const char* api) {
  
  // Construct json parameter object
  StaticJsonBuffer<1200> jsonBuffer; // This seems not to affect anywhere
  StaticJsonBuffer<500> jsonBuffer2; // Increment more based on Object count
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& data = root.createNestedArray("data");

  JsonObject& incomingAirObj = jsonBuffer2.createObject();
  incomingAirObj["name"] = "incoming_air";
  incomingAirObj["temp"] = incomingAirTemp;
  incomingAirObj["hum"] = incomingAirHumidity;

  JsonObject& outgoingAirToRoomsObj = jsonBuffer2.createObject();
  incomingAirObj["name"] = "outgoing_air_to_rooms";
  incomingAirObj["temp"] = outgoingAirToRoomsTemp;
  incomingAirObj["hum"] = outgoingAirToRoomsHumidity;

  JsonObject& returningRoomsAirObj = jsonBuffer2.createObject();
  incomingAirObj["name"] = "returning_rooms_air";
  incomingAirObj["temp"] = returningRoomsAirTemp;
  incomingAirObj["hum"] = returningRoomsAirHumidity;

  JsonObject& afterHeatingCoilObj = jsonBuffer2.createObject();
  incomingAirObj["name"] = "after_heating_coil";
  incomingAirObj["temp"] = afterHeatingCoilTemp;
  incomingAirObj["hum"] = afterHeatingCoilHumidity;

  // Append to data array
  data.add(incomingAirObj);
  data.add(outgoingAirToRoomsObj);
  data.add(returningRoomsAirObj);
  data.add(afterHeatingCoilObj);

  String postData = "";
  root.printTo(postData);
  Serial.println(postData);

  // Http post
  Serial.print("POST ");
  Serial.println(api);
  client.print("POST ");
  client.print(api);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(server);
  client.println("Content-Type: application/json");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(postData.length());
  client.println();
  client.print(postData);
  client.println();
  
  return true;
}

bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);
  if (!ok) {
    Serial.println("No response or invalid response!");
  }
  return ok;
}

void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}
