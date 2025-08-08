#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "wifi_ssid";
const char* password = "wifi_password";

// --- MQTT Broker ---
const char* mqtt_server = "localhost_of_your_MQTT";  // IP of Docker host running Mosquitto.
const int mqtt_port = 1883; // Edit if yoou are using a different port.
const char* mqtt_user = "your_username";
const char* mqtt_pass = "your_password";
const char* topic = "home/esp32/sensors";

// --- DHT22 Setup ---
#define DHTPIN SDA       // GPIO21
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --- Light Sensor ---
#define LIGHT_PIN 34   // ADC1_CH6 (GPIO34) - input only

// --- Clients ---
WiFiClient espClient;
PubSubClient client(espClient);

// --- Functions ---
void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      // Connected
    } else {
      delay(5000);
    }
  }
}

// --- Publish Data ---
void publishData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int lightRaw = analogRead(LIGHT_PIN);   // 0–4095
  float lightVoltageRaw = (analogRead(LIGHT_PIN) / 4095.0) * 3.3;
  float lightPercentRaw = (lightRaw / 4095.0) * 100.0; // Convert to %

  float lightVoltage = roundf(lightVoltageRaw * 10) / 10.0;
  float lightPercent = roundf(lightPercentRaw * 10) / 10.0;

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Create JSON with both temperature and humidity
  StaticJsonDocument<128> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;
  doc["lightP"] = lightPercent;
  doc["lightV"] = lightVoltage;

  char jsonBuffer[320];
  serializeJson(doc, jsonBuffer);

  client.publish(topic, jsonBuffer);

  Serial.print("Published JSON: ");
  Serial.println(jsonBuffer);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

// --- Main Loop ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  publishData();
  delay(60000);  // Send every 1 minutes
}