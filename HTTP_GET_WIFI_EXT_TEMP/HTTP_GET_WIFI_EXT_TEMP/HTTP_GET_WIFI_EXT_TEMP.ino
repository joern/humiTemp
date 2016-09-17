#include <ESP8266WiFi.h>
//#include <WiFiClient.h> //could be used for provisioning wifi
extern "C" {
  #include "user_interface.h" 
}

#include "DHT.h"
#include <OneWire.h>

#define DHTPIN D4       // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)


// Existing WiFi network
const char* ssid     = "SSID";
const char* password = "PASSWD";

//emoncoms
const char* host = "URL";
const char* nodeId   = "UNIQUE_ID";
const char* privateKey = "KEY";

int ledState = LOW;

float temp, hum, extTemp;
int vcc;

// sleep for this many seconds
const int sleepSeconds = 240;

DHT dht(DHTPIN, DHTTYPE);
ADC_MODE(ADC_VCC);

OneWire  ds(D1);  // on pin D2 (a 4.7K resistor to VCC is necessary)


//WiFiClient client;

#define DEBUGPRINT

#ifdef DEBUGPRINT
#define DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_PRINTLN(x)
#endif

#ifdef DEBUGPRINT
#define DEBUG_PRINT(x)  Serial.print (x)
#else
#define DEBUG_PRINT(x)
#endif


void sendData() {
  DEBUG_PRINT("connecting to ");
  DEBUG_PRINTLN(host);

  WiFiClient emoClient;

  const int httpPort = 80;
  if (!emoClient.connect(host, httpPort)) {
    DEBUG_PRINTLN("connection failed");
    return;
  }

  String json = "{temperature:";
  json += temp;
  json += ",humidity:";
  json += hum;  
  json += ",vcc:";
  json += vcc;
  json += ",extTemp:";
  json += extTemp;
  json += "}";

  String url = "/input/post.json?node=";
  url += nodeId;
  url += "&apikey=";
  url += privateKey;
  url += "&json=";
  url += json;

  DEBUG_PRINT("Requesting URL: ");
  DEBUG_PRINTLN(url);

  // This will send the request to the server
  emoClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  while (emoClient.available()) {
    String line = emoClient.readStringUntil('\r');
    DEBUG_PRINTLN(line);
  }

  DEBUG_PRINTLN();
  DEBUG_PRINTLN("closing connection");

}

void readExtTemperatur() {
  /*
  DEBUG_PRINTLN("Start reading one wire temp...");
  //read external sensor
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  extTemp = (float)raw / 16.0;

  DEBUG_PRINT("EXT TEMP: ");
  DEBUG_PRINTLN(extTemp);
  DEBUG_PRINTLN();
  */

  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  //float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    DEBUG_PRINTLN("No more addresses.");
    DEBUG_PRINTLN();
    ds.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    DEBUG_PRINT(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      DEBUG_PRINTLN("CRC is not valid!");
      return;
  }
  DEBUG_PRINTLN();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      DEBUG_PRINTLN("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      DEBUG_PRINTLN("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      DEBUG_PRINTLN("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      DEBUG_PRINTLN("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  DEBUG_PRINT("  Data = ");
  Serial.print(present, HEX);
  DEBUG_PRINT(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  DEBUG_PRINT(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  DEBUG_PRINTLN();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  extTemp = (float)raw / 16.0;
  //fahrenheit = celsius * 1.8 + 32.0;
  DEBUG_PRINT("  Temperature = ");
  DEBUG_PRINTLN(extTemp);
}


void setup() {
  dht.begin();

  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  
  byte led_status = 0;
  pinMode(BUILTIN_LED, OUTPUT);
  
  // Open the Arduino IDE Serial Monitor to see what the code is doing
  Serial.begin(9600);

  DEBUG_PRINTLN("----- Starting WLAN -----");
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); // Enable light sleep mode
  WiFi.mode(WIFI_STA);

    // Connect to your WiFi network
  WiFi.begin(ssid, password);
  DEBUG_PRINT("Connecting");

  // Wait for successful connection
  while (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINT(".");
    digitalWrite(BUILTIN_LED, ledState);
    ledState = !ledState;
    delay(250);
  }
  DEBUG_PRINTLN(".");

}

void loop() {
  DEBUG_PRINTLN("Start reading temp/hum");
  dht.read();
  pinMode(BUILTIN_LED, OUTPUT);
  for (int i = 0; i < 22; i++)
  {
    digitalWrite(BUILTIN_LED, LOW);
    delay(10);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(10);
  }
  delay(2100); //should wait 2,1 s
  dht.read();

  vcc = ESP.getVcc();
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  readExtTemperatur();
  
  DEBUG_PRINT("VCC: ");
  DEBUG_PRINTLN(vcc);
  DEBUG_PRINT("HUM: ");
  DEBUG_PRINTLN(hum);
  DEBUG_PRINT("TEMP: ");
  DEBUG_PRINTLN(temp);
  DEBUG_PRINT("EXT TEMP: ");
  DEBUG_PRINTLN(extTemp);
  
  DEBUG_PRINTLN("Stop reading temp/hum");
  
  if (!isnan(hum) && !isnan(temp)) {
      delay(10);
      sendData();
      delay(10);
    }
  delay(sleepSeconds * 1000);
  
}





