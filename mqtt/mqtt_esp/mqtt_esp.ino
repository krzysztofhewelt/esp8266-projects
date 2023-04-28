#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

// WiFi network credentials
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"

// MQTT server
#define MQTT_HOST IPAddress(192, 168, 0, 100)
#define MQTT_PORT 1883

// Temperature MQTT Topics
#define MQTT_PUB_TEMP "esp/dht/temperature"
#define MQTT_PUB_HUM "esp/dht/humidity"
#define MQTT_SUB_RED_LED "esp/led/red"
#define MQTT_SUB_YELLOW_LED "esp/led/yellow"
#define MQTT_SUB_GREEN_LED "esp/led/green"

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

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time measurements was published
const long interval = 5000;        // Interval at which to publish sensor readings

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());

  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  setLed((String) topic, (char) payload[0]);
}

void setLed(String led, char status) {
  Serial.printf("Topic: %s, Status: %c\n", led.c_str(), status);

  if(led == "esp/led/red") {
    if(status == '0') {
      digitalWrite(RED_LED, LOW);
    } else {
      digitalWrite(RED_LED, HIGH);
    }
  }

  if(led == "esp/led/yellow") {
    if(status == '0') {
      digitalWrite(YELLOW_LED, LOW);
    } else {
      digitalWrite(YELLOW_LED, HIGH);
    }
  }

  if(led == "esp/led/green") {
    if(status == '0') {
      digitalWrite(GREEN_LED, LOW);
    } else {
      digitalWrite(GREEN_LED, HIGH);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  dht.begin();
  
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  
  connectToWifi();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);

  // Subscribe MQTT LEDs topics
  mqttClient.subscribe(MQTT_SUB_RED_LED, 1);
  mqttClient.subscribe(MQTT_SUB_YELLOW_LED, 1);
  mqttClient.subscribe(MQTT_SUB_GREEN_LED, 1);
}

void loop() {
  if(WiFi.isConnected() && mqttClient.connected()) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // DHT sensor readings
      hum = dht.readHumidity();
      temp = dht.readTemperature();
      
      // Publish an MQTT message on topic esp/dht/temperature
      uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(temp).c_str());                            
      Serial.printf("Publishing on topic %s at QoS 1, packetId: %i ", MQTT_PUB_TEMP, packetIdPub1);
      Serial.printf("Message: %.2f \n", temp);

      // Publish an MQTT message on topic esp/dht/humidity
      uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_HUM, 1, true, String(hum).c_str());                            
      Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_HUM, packetIdPub2);
      Serial.printf("Message: %.2f \n", hum);
    }
  }
}