/**
 * MUH Ilmava Temperature measuring and sender device
 * Arduino: Mega2560
 * Temperature sensors: 4 x DHT22
 * + Ethernet Card (NOTE: 50, 51, 52, 53 are somehow used by wiznet)
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
// --------------------------------------------------------------------------------
// ## Libraries ##
#include <ArduinoJson.h>
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <ArduinoJson.h>
#include <dht.h>
#include <TaskScheduler.h>
#include <MQ135.h>

// --------------------------------------------------------------------------------
// ## Pins config ##
#define DHT22_1_PIN 46
#define DHT22_2_PIN 47
#define DHT22_3_PIN 48
#define DHT22_4_PIN 49
// const int MQ135_1_PIN = A8;
const int MQ135_2_PIN = A9;
const int MQ135_3_PIN = A10;
// const int MQ135_4_PIN = A11;

// --------------------------------------------------------------------------------
// ## Variables ##
dht DHT;
MQ135 mq2 = MQ135(MQ135_2_PIN, 80.00);
MQ135 mq3 = MQ135(MQ135_3_PIN, 56.39);

double incomingAirTemp = 0.0;
int incomingAirHumidity = 0;
double incomingAirDioxide = 0.0;
double incomingAirMonoxide = 0.0;

double outgoingAirToRoomsTemp = 0.0;
int outgoingAirToRoomsHumidity = 0;
double outgoingAirToRoomsDioxide = 0.0;
double outgoingAirToRoomsMonoxide = 0.0;

double returningRoomsAirTemp = 0.0;
int returningRoomsAirHumidity = 0;
double returningRoomsAirDioxide = 0.0;
double returningRoomsAirMonoxide = 0.0;

double wasteAirOutTemp = 0.0;
int wasteAirOutHumidity = 0;
double wasteAirOutDioxide = 0.0;
double wasteAirOutMonoxide = 0.0;

long timePreviousMeassure = 0;

// --------------------------------------------------------------------------------
// ## "Calibration" ##
static double INCOMING_AIR_T_CORRECTION = 1.2;
static double OUTGOING_AIR_TO_ROOMS_T_CORRECTION = 0.9;
static double RETURNING_ROOMS_AIR_T_CORRECTION = -0.6;
static double WASTE_AIR_OUT_T_CORRECTION = 0.0;

// --------------------------------------------------------------------------------
// ## Ethernet config ##
EthernetClient client;
byte mac[] = { 0x2E, 0xA8, 0xA9, 0xB5, 0xDA, 0xF6 };
const char* server  = "192.168.2.35"; // Where stuff is sent at
const char* api     = "/device/v1/measurements";
const int port      = 8080;
const unsigned long HTTP_TIMEOUT = 10000;
const size_t MAX_CONTENT_SIZE = 1024; // Must be incremented if http response comes out as null

// --------------------------------------------------------------------------------
// ## Task scheduler config ##
Scheduler runner;
void dhtRead();
void dioxideRead();
void monoxideRead();
void httpPostRequest();
Task t1(11 * 1000, TASK_FOREVER, &dhtRead);
Task t2(11 * 1000, TASK_FOREVER, &dioxideRead);
Task t3(11 * 1000, TASK_FOREVER, &monoxideRead);
Task t4(30 * 1000, TASK_FOREVER, &httpPostRequest);    int httpSkip = 0; // Skip some

// --------------------------------------------------------------------------------

// SETUP
void setup() {
  Serial.begin(9600);

  // init ethernet
  if (!Ethernet.begin(mac)) { // DHCP
    Serial.println("Ethernet configure failed!");
  } else {
    Serial.println("Ethernet init ok.");
    Serial.println(Ethernet.localIP());
  }

  // check rzeros for mq sensors 
  float rzero_2 = mq2.getRZero();
  float rzero_3 = mq3.getRZero();
  Serial.println("MQ135 Rzeros");
  Serial.println(rzero_2);
  Serial.println(rzero_3);

  // init task scheduler
  runner.init();
  runner.addTask(t1); // dht
  runner.addTask(t2); // dioxide
  runner.addTask(t3); // monoxide
  runner.addTask(t4); // http call
  t1.enable();
  t2.enable();
  t3.enable();
  t4.enable();

  delay(5 * 1000);
}

// --------------------------------------------------------------------------------

// LOOP
void loop() {
  runner.execute(); // Runner is responsible for all tasks
}

// --------------------------------------------------------------------------------------------------------------------------
// -------------------------------------A L L - S E N S O R - R E A D I N G - T A S K S -------------------------------------
// --------------------------------------------------------------------------------------------------------------------------

// Read DHT temperature and humidity
void dhtRead() {  
  int chk = DHT.read22(DHT22_1_PIN);  
  incomingAirTemp = DHT.temperature + INCOMING_AIR_T_CORRECTION;
  incomingAirHumidity = DHT.humidity;
  Serial.print("Incoming Air temp: ");
  Serial.print(incomingAirTemp);
  Serial.println("°C");
  Serial.print("Incoming Air humidity: ");
  Serial.print(incomingAirHumidity);
  Serial.println("rH");

  chk = DHT.read22(DHT22_2_PIN);  
  outgoingAirToRoomsTemp = DHT.temperature + OUTGOING_AIR_TO_ROOMS_T_CORRECTION;
  outgoingAirToRoomsHumidity = DHT.humidity;
  Serial.print("Outgoing Air To Rooms temp: ");
  Serial.print(outgoingAirToRoomsTemp);
  Serial.println("°C");
  Serial.print("Outgoing Air To Rooms humidity: ");
  Serial.print(outgoingAirToRoomsHumidity);
  Serial.println("rH");

  chk = DHT.read22(DHT22_3_PIN);  
  returningRoomsAirTemp = DHT.temperature + RETURNING_ROOMS_AIR_T_CORRECTION;
  returningRoomsAirHumidity = DHT.humidity;
  Serial.print("Returning Rooms Air temp: ");
  Serial.print(returningRoomsAirTemp);
  Serial.println("°C");
  Serial.print("Returning Rooms Air humidity: ");
  Serial.print(returningRoomsAirHumidity);
  Serial.println("rH");

  chk = DHT.read22(DHT22_4_PIN);  
  wasteAirOutTemp = DHT.temperature + WASTE_AIR_OUT_T_CORRECTION;
  wasteAirOutHumidity = DHT.humidity;
  Serial.print("Waste Air Out temp: ");
  Serial.print(wasteAirOutTemp);
  Serial.println("°C");
  Serial.print("Waste Air Out humidity: ");
  Serial.print(wasteAirOutHumidity);
  Serial.println("rH");

  Serial.println("------------------------------------");
}

// Read carbon dioxide sensors (MQ-135)
void dioxideRead() {
  // outgoingAirToRoomsDioxide = analogRead(MQ135_2_PIN);
  outgoingAirToRoomsDioxide = mq2.getCorrectedPPM(outgoingAirToRoomsTemp, returningRoomsAirHumidity);
  Serial.print("Outgoing Air To Rooms carbon dioxide: ");
  Serial.print(outgoingAirToRoomsDioxide);
  Serial.println(" PPM");
  
  // returningRoomsAirDioxide = analogRead(MQ135_3_PIN);
  returningRoomsAirDioxide = mq3.getCorrectedPPM(returningRoomsAirTemp, returningRoomsAirHumidity);
  Serial.print("Returning Rooms Air carbon dioxide: ");
  Serial.print(returningRoomsAirDioxide);
  Serial.println(" PPM");

  Serial.println("------------------------------------");
}

// Read carbon monoxide sensors (MQ-9)
void monoxideRead() {
  // currently don't have any
}


// --------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------- H T T P - A P I - C A L L - T A S K --------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------

// Http post operation
void httpPostRequest() {
    if (httpSkip == 0) {
      Serial.print("Http callback");
      if (connect(server)) {
        Serial.print(server); Serial.print(":"); Serial.print(port); Serial.println(api);
        if (sendRequest(server, api) && skipResponseHeaders()) {
          char response[MAX_CONTENT_SIZE];
          readReponseContent(response, sizeof(response));
          parseResponseData(response);
        }
        disconnect();
      }
    }
    httpSkip = httpSkip +1;
    if (httpSkip == 2) {
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
  Serial.println(client.remoteIP());
  return ok;
}

// Send the HTTP POST request to the server, same time GET needed response data
bool sendRequest(const char* host, const char* api) {

  // Construct json parameter object
  StaticJsonBuffer<1200> jsonBuffer; // This seems not to affect anywhere
  StaticJsonBuffer<500> jsonBuffer2; // Increment more based on Object count
  JsonObject& root = jsonBuffer.createObject();
  // root["firmwareBuild"] = firmware_build;
  JsonArray& data = root.createNestedArray("data");

  JsonObject& incomingAirObj = jsonBuffer2.createObject();
  incomingAirObj["name"] = "incoming_air";
  incomingAirObj["temp"] = incomingAirTemp;
  incomingAirObj["hum"] = incomingAirHumidity;
  // incomingAirObj["dioxide"] = incomingAirDioxide;
  // incomingAirObj["monoxide"] = incomingAirMonoxide;

  JsonObject& outgoingAirToRoomsObj = jsonBuffer2.createObject();
  outgoingAirToRoomsObj["name"] = "outgoing_air_to_rooms";
  outgoingAirToRoomsObj["temp"] = outgoingAirToRoomsTemp;
  outgoingAirToRoomsObj["hum"] = outgoingAirToRoomsHumidity;
  outgoingAirToRoomsObj["dioxide"] = outgoingAirToRoomsDioxide;
  // outgoingAirToRoomsObj["monoxide"] = outgoingAirToRoomsMonoxide;

  JsonObject& returningRoomsAirObj = jsonBuffer2.createObject();
  returningRoomsAirObj["name"] = "returning_rooms_air";
  returningRoomsAirObj["temp"] = returningRoomsAirTemp;
  returningRoomsAirObj["hum"] = returningRoomsAirHumidity;
  returningRoomsAirObj["dioxide"] = returningRoomsAirDioxide;
  // returningRoomsAirObj["monoxide"] = returningRoomsAirMonoxide;
  

  JsonObject& wasteAirOutObj = jsonBuffer2.createObject();
  wasteAirOutObj["name"] = "waste_air_out";
  wasteAirOutObj["temp"] = wasteAirOutTemp;
  wasteAirOutObj["hum"] = wasteAirOutHumidity;
  wasteAirOutObj["dioxide"] = wasteAirOutDioxide;
  // wasteAirOutObj["monoxide"] = wasteAirOutMonoxide;

  // Append to data array
  data.add(incomingAirObj);
  data.add(outgoingAirToRoomsObj);
  data.add(returningRoomsAirObj);
  data.add(wasteAirOutObj);

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
  //client.println("Content-Type: application/x-www-form-urlencoded");
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

// --------------------------------------------------------------------------------

// Skip HTTP headers so that we are at the beginning of the response's body
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

// --------------------------------------------------------------------------------

// Read the body of the response from the HTTP server
void readReponseContent(char* content, size_t maxSize) {
  size_t length = client.readBytes(content, maxSize);
  content[length] = 0;
}

// --------------------------------------------------------------------------------

// Parse response data (commands) and apply them
void parseResponseData(char* content) {
  StaticJsonBuffer<MAX_CONTENT_SIZE> jsonBuffer; // Increment MAX_CONTENT_SIZE constant variable if repsonse is null
  JsonObject& root = jsonBuffer.parseObject(content);

  Serial.println("---------- HTTP RESPONSE ----------");
  String responseData = "";
  Serial.println(responseData);
  Serial.println("-----------------------------------");

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return;
  }

}

// --------------------------------------------------------------------------------

// Close the connection with the HTTP server
void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}
