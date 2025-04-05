#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// ThingSpeak settings
#define CHANNEL_ID  2907076
#define CHANNEL_API_KEY "U8RBAIGRTPMXID7J"

// Water level sensor pin
#define WATER_SENSOR_PIN 34  // Use an appropriate analog pin on your ESP32

WiFiClient client;

// WiFi settings
#define WIFI_TIMEOUT_MS 20000
#define WIFI_NETWORK "Oneplus"
#define WIFI_PASSWORD "PaSSworD"

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
    Serial.print(".");
    delay(100);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed");
  } else {
    Serial.print("Connected: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  Serial.begin(115200);
  // Set water level sensor pin as input
  pinMode(WATER_SENSOR_PIN, INPUT);
  connectToWiFi();
  ThingSpeak.begin(client);
}

void loop() {
  // Check if WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }
  
  // Read water level sensor value
  int waterLevel = analogRead(WATER_SENSOR_PIN);
  
  // Print reading to serial monitor for debugging
  Serial.print("Water Level: ");
  Serial.println(waterLevel);
  
  // Write data to ThingSpeak
  ThingSpeak.setField(1, waterLevel); // Water level in field 1
  ThingSpeak.setField(2, WiFi.RSSI()); // WiFi signal strength in field 2
  
  // Capture the return code from writeFields
  int httpCode = ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY);
  
  if (httpCode == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code: " + String(httpCode));
  }
  
  delay(15000);  // ThingSpeak limits updates to once every 15 seconds
}