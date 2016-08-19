#include <ESP8266WiFi.h>
//#include <WiFiClient.h> //could be used for provisioning wifi
extern "C" {
  #include "user_interface.h" 
}

#include "DHT.h"

#define DHTPIN D4       // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Existing WiFi network
const char* ssid     = "THE SSDI";
const char* password = "THE KEY";

//emoncoms
const char* host = "HOSTNAME";
const char* nodeId   = "UNIQUENODEID";
const char* privateKey = "SECRET KEY";

int ledState = LOW;

float temp, hum;
int vcc;

// sleep for this many seconds
const int sleepSeconds = 240;

DHT dht(DHTPIN, DHTTYPE);
ADC_MODE(ADC_VCC);

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
  delay(2100);
  dht.read();
  DEBUG_PRINTLN("Stop reading temp/hum");

  vcc = ESP.getVcc();
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  DEBUG_PRINT("VCC: ");
  DEBUG_PRINTLN(vcc);
  DEBUG_PRINT("HUM: ");
  DEBUG_PRINTLN(hum);
  DEBUG_PRINT("TEMP: ");
  DEBUG_PRINTLN(temp);
  
  if (!isnan(hum) && !isnan(temp)) {
      delay(10);
      sendData();
      delay(10);
    }
  delay(sleepSeconds * 1000);

}





