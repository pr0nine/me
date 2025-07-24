#include "SinricPro.h"
#include "SinricProTemperaturesensor.h"
#include "SinricProSwitch.h"
#include "SinricProDimSwitch.h"
#define APP_KEY           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define APP_SECRET        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define TEMP_SENSOR_ID    "xxxxxxxxxxxxxxxxxxxxxxxx"
#define SWITCH_ID_1       "xxxxxxxxxxxxxxxxxxxxxxxx"
#define RELAYPIN_1        D2
#define DIMSWITCH_ID      "xxxxxxxxxxxxxxxxxxxxxxxx"
#define RELAYPIN_2        D5
int power  = 0;

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SimplePgSQL.h>

#include <ZMPT101B.h>
#define SENSITIVITY 500.0f
float voltage = 0.0;
// ZMPT101B sensor output connected to analog pin A0 and the voltage source frequency is 50 Hz
ZMPT101B voltageSensor(A0, 50.0);

#include <ACS712.h>
ACS712 ACS(A0, 2.82, 1024, 24);  // Parameters: analog pin, Vref, ADC resolution, sensor sensitivity
float current = 0.0;
int calibration_factor = 0;
float watt = 0.0;
float readCurrent() {
  float average = 0;
  ESP.wdtFeed();
  for (int i = 0; i < 100; i++) {
    average += ACS.mA_AC();
    ESP.wdtFeed();
  }
  float mA = (abs(average / 100.0) - calibration_factor);
  if (mA <= 5) mA = 0;
  ESP.wdtFeed();
  return mA;
}

//Setup for DHT======================================
#include <DHT.h>
#define DHTPIN D1  //GPIO5
#define DHTTYPE    DHT22     // For DHT 22 sensor
DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
//Setup for DHT======================================

//Setup connection and Database=====================
const char* ssid = "xxxxxx";
const char* pass = "xxxxxxxxxxxxxxxxx";
WiFiClient client;
IPAddress PGIP(69, 69, 69, 69);     // your PostgreSQL server IP
const char user[] = "xxxxx";       // your database user
const char password[] = "xxxxxxxxxx";   // your database password
const char dbname[] = "xxxxxxxxx";         // your database name
char buffer[1024];
PGconnection conn(&client, 0, 1024, buffer);
char inbuf[128];
int pg_status = 0;

//millis================================
//Set every 60 sec read DHT
unsigned long previousMillis = 0;  // variable to store the last time the task was run
const long interval = 60000;        // time interval in milliseconds (eg 1000ms = 1 second)
//======================================

float lastTemperature;                        // last known temperature (for compare)
float lastHumidity;                           // last known humidity (for compare)
unsigned long lastEvent = (-interval); // last time event has been sent

void handleTemperaturesensor() {
  unsigned long actualMillis = millis();
  if (actualMillis - lastEvent < interval) return; //only check every EVENT_WAIT_TIME milliseconds
  if (SinricPro.isConnected() == false) {
    Serial.printf("Not connected to Sinric Pro...!\r\n");
    return; 
  }
  if (isnan(t) || isnan(h)) { // reading failed... 
    Serial.printf("DHT reading failed!\r\n");  // print error message
    return;                                    // try again next time
  } 
  if (t == lastTemperature || h == lastHumidity) return; // if no values changed do nothing...
  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];  // get temperaturesensor device
  bool success = mySensor.sendTemperatureEvent(t, h); // send event
  if (success) {  // if event was sent successfuly, print temperature and humidity to serial
  } else {  // if sending event failed, print error message
    Serial.printf("Something went wrong...could not send Event to server!\r\n");
  }
  lastTemperature = t;  // save actual temperature for next compare
  lastHumidity = h;        // save actual humidity for next compare
  lastEvent = actualMillis;       // save actual time for next compare
}

bool onPowerState1(const String &deviceId, bool &state) {
 Serial.printf("Device 1 turned %s", state?"on":"off");
 digitalWrite(RELAYPIN_1, state ? HIGH:LOW);
 return true; // request handled properly
}

// we use a struct to store all states and values for our dimmable switch
struct {
  bool powerState = false;
  int powerLevel = 0;
} device_state;

void setPWM(int powerLevel) {
   power = map(powerLevel, 0, 100, 0, 255); // Map power level from 0 to 100, to a value between 0 to 255
   analogWrite(RELAYPIN_2,(power));     
   delay(30);
} 

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  device_state.powerState = state;
  if (state) {
    setPWM(device_state.powerLevel);
  } else {
    setPWM(0);
  }
  return true; // request handled properly
}

bool onPowerLevel(const String &deviceId, int &powerLevel) {
  device_state.powerLevel = powerLevel; 
  setPWM(powerLevel);
  Serial.printf("Device %s power level changed to %d\r\n", deviceId.c_str(), device_state.powerLevel);
  return true;
}

bool onAdjustPowerLevel(const String &deviceId, int &levelDelta) {
  device_state.powerLevel += levelDelta;
  Serial.printf("Device %s power level changed about %i to %d\r\n", deviceId.c_str(), levelDelta, device_state.powerLevel);
  levelDelta = device_state.powerLevel;
  return true;
}

// setup function for SinricPro
void setupSinricPro() {
  pinMode(RELAYPIN_1, OUTPUT);
  pinMode(RELAYPIN_2, OUTPUT);
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];

  // set callback function to device
  myDimSwitch.onPowerState(onPowerState);
  myDimSwitch.onPowerLevel(onPowerLevel);
  myDimSwitch.onAdjustPowerLevel(onAdjustPowerLevel);
  
  // add device to SinricPro
  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);
  
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
     
  SinricPro.begin(APP_KEY, APP_SECRET);  
}

void setup() {

  setupSinricPro();
  
  voltageSensor.setSensitivity(SENSITIVITY);
  
  pinMode(D7,OUTPUT);
  pinMode(D6,OUTPUT);
  digitalWrite(D7,HIGH);
  digitalWrite(D6,HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  //For DHT sensor
  dht.begin();

  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void doPg(void) {
  char *msg;
  int rc;
  if (!pg_status) {
    conn.setDbLogin(PGIP,
                    user,
                    password,
                    dbname,
                    "utf8");
    pg_status = 1;
    return;
  }
  if (pg_status == 1) {
    rc = conn.status();
    if (rc == CONNECTION_BAD || rc == CONNECTION_NEEDED) {
      char *c = conn.getMessage();
      if (c) Serial.println(c);
      pg_status = -1;
    }
    else if (rc == CONNECTION_OK) {
      pg_status = 2;
      Serial.println("Starting query");
    }
    return;
  }
  
  if (pg_status == 2 && strlen(inbuf) > 0) {
    if (conn.execute(inbuf)) goto error;
    Serial.println("Working...");
    pg_status = 3;
    memset(inbuf, 0, sizeof(inbuf));
  }
  
  if (pg_status == 3) {
    rc = conn.getData();
    int i;
    if (rc < 0) goto error;
    if (!rc) return;
    if (rc & PG_RSTAT_HAVE_COLUMNS) {
      for (i = 0; i < conn.nfields(); i++) {
        if (i) Serial.print(" | ");
        Serial.print(conn.getColumn(i));
      }
      Serial.println("\n==========");
    }
    else if (rc & PG_RSTAT_HAVE_ROW) {
      for (i = 0; i < conn.nfields(); i++) {
        if (i) Serial.print(" | ");
        msg = conn.getValue(i);
        if (!msg) msg = (char *)"NULL";
        Serial.print(msg);
      }
      Serial.println();
    }
    else if (rc & PG_RSTAT_HAVE_SUMMARY) {
      Serial.print("Rows affected: ");
      Serial.println(conn.ntuples());
    }
    else if (rc & PG_RSTAT_HAVE_MESSAGE) {
      msg = conn.getMessage();
      if (msg) Serial.println(msg);
    }
    if (rc & PG_RSTAT_READY) {
      pg_status = 2;
      Serial.println("Waiting query");
    }
  }
  return;
error:
  msg = conn.getMessage();
  if (msg) Serial.println(msg);
  else Serial.println("UNKNOWN ERROR");
  if (conn.status() == CONNECTION_BAD) {
    Serial.println("Connection is bad");
    pg_status = -1;
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  SinricPro.handle();
  delay(70);
  handleTemperaturesensor();
  delay(70);
  doPg();
  unsigned long currentMillis = millis();
  // Checks whether it is time to run the task
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the task was run
    previousMillis = currentMillis;
    if (WiFi.status() == WL_CONNECTED) {
      //read DHT-22---------------------------------------
      t = dht.readTemperature();
      h = dht.readHumidity();
      Serial.print("Humidity = ");
      Serial.print(h);
      Serial.print("% ");
      Serial.print("Temperature = ");
      Serial.print(t);
      Serial.print(" C      ");
      Serial.print("Voltage: ");
      Serial.print(voltage);
      Serial.print(" v      ");
      Serial.print("Current: ");
      Serial.print(current);
      Serial.println(" mA");
      run_bitch();
    }
    else {
      Serial.println("WiFi Disconnected");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1500);
    }
    
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void run_bitch() {
    ESP.wdtFeed();
    digitalWrite(D7,HIGH); //pnp high to turn off current sensor
    digitalWrite(D6,LOW); //pnp low to turn on voltage sensor
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    voltage = (voltageSensor.getRmsVoltage()*1.465);
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println(" v");
    ESP.wdtFeed();
    digitalWrite(D6,HIGH);//pnp high to turn off voltage sensor
    digitalWrite(D7,LOW); //pnp low to turn on current sensor
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    ESP.wdtFeed();
    delay(50);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(50);
    ESP.wdtFeed();
    delay(50);
    ESP.wdtFeed();
    delay(25);
    ESP.wdtFeed();
    delay(50);
    current = readCurrent();
    delay(20);
    digitalWrite(D7,HIGH);
    watt = voltage*(current/1000);
    //Send data to PostgreSQL
    sprintf(inbuf, "insert into esp (temp,humidity,voltage,current,watt) values(%.2f,%.2f,%.2f,%.2f,%.2f)", t, h, voltage, current, watt);
    ESP.wdtFeed();
}
