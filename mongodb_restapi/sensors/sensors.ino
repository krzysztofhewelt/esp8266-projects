#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

// WiFi network credentials
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"

// Digital pin connected to the DHT sensor
#define DHTPIN D2
#define RED_LED D5
#define YELLOW_LED D6
#define GREEN_LED D7

// SET DHT device TYPE
#define DHTTYPE DHT22

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);


// Variables to hold sensor readings
float temp;
float hum;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time measurements was published
const long interval = 5000;        // Interval at which to publish sensor readings
const String rest_api_server = "http://192.168.0.100:8000/api";  // REST API server location
const String device_name = "ESP8266";  // device name

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  wifiReconnectTimer.once(2, connectToWifi);
}

void get_http_led_request() {
  WiFiClient client;
  HTTPClient http;

  String server = rest_api_server + "/leds/" + String(ESP.getChipId());

  http.begin(client, server);

  int httpResponseCode = http.GET();
  String payload = "{}";

  if (httpResponseCode == 200) {
    payload = http.getString();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    for(int i = 0; i <= 2; i++) {
      set_led(doc[i]["led_name"], doc[i]["status"]);
    }
  } else {
    Serial.printf("Cannot get data from the REST API server. Error code: %d\n", httpResponseCode);
  }
}

void set_led(String led_name, String status) {
  if(led_name == "green") {
    if(status == "0") {
      digitalWrite(GREEN_LED, LOW);
    } else {
      digitalWrite(GREEN_LED, HIGH);
    }
  }

  if(led_name == "red") {
    if(status == "0") {
      digitalWrite(RED_LED, LOW);
    } else {
      digitalWrite(RED_LED, HIGH);
    }
  }

  if(led_name == "yellow") {
    if(status == "0") {
      digitalWrite(YELLOW_LED, LOW);
    } else {
      digitalWrite(YELLOW_LED, HIGH);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  dht.begin();

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  connectToWifi();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
}

void loop() {
  if(WiFi.isConnected()) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // DHT sensor readings
      hum = dht.readHumidity();
      temp = dht.readTemperature();

      StaticJsonDocument<500> doc;
      doc["IP"] = WiFi.localIP().toString();
      doc["name"] = device_name;
      doc["humidity"] = hum;
      doc["temperature"] = temp;
      doc["wifi_rssi"] = WiFi.RSSI();

      String json;
      serializeJson(doc, json);

      WiFiClient client;
      HTTPClient http;

      String server = rest_api_server + "/measurements/" + String(ESP.getChipId());

      http.begin(client, server);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(json);
      if(httpResponseCode == 200) {
        Serial.println(json);
      } else {
        Serial.printf("Cannot send data to the REST API server. Error code: %d\n", httpResponseCode);
      }

      get_http_led_request();
    }
  }
}
