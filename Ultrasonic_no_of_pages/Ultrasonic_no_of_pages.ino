#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// ThingSpeak settings
#define CHANNEL_ID  2907239
#define CHANNEL_API_KEY "YCDTAOPXLUVOAF9W"

// Water level sensor pin
#define WATER_SENSOR_PIN 34  // Use an appropriate analog pin on your ESP32

// Ultrasonic sensor pins
#define ULTRASONIC_TRIGGER_PIN 12  // GPIO pin for trigger
#define ULTRASONIC_ECHO_PIN 14     // GPIO pin for echo

// Constants for page counting calculation
#define TRAY_EMPTY_DISTANCE 7.5   // Distance in cm when tray is empty
#define TRAY_FULL_DISTANCE 1.0     // Distance in cm when tray is full
#define MAX_PAGES 750              // Maximum number of pages the tray can hold

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

// Function to measure distance using ultrasonic sensor
float measureDistance() {
  // Clear the trigger pin
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  
  // Set the trigger pin high for 10 microseconds
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  
  // Read the echo pin, returns the sound wave travel time in microseconds
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  
  // Calculate the distance in centimeters
  // Speed of sound is approximately 343 meters per second or 0.0343 cm/microsecond
  // Distance = (Time x Speed of sound) / 2
  float distance = duration * 0.0343 / 2;
  
  return distance;
}

// Function to calculate pages left based on distance
int calculatePagesLeft(float distance) {
  // If distance is greater than or equal to empty tray distance, return 0 pages
  if (distance >= TRAY_EMPTY_DISTANCE) {
    return 0;
  }
  
  // If distance is less than or equal to full tray distance, return max pages
  if (distance <= TRAY_FULL_DISTANCE) {
    return MAX_PAGES;
  }
  
  // Linear interpolation to estimate pages based on distance
  float percentage = (TRAY_EMPTY_DISTANCE - distance) / (TRAY_EMPTY_DISTANCE - TRAY_FULL_DISTANCE);
  return round(percentage * MAX_PAGES);
}

void setup() {
  Serial.begin(115200);
  
  // Set water level sensor pin as input
  pinMode(WATER_SENSOR_PIN, INPUT);
  
  // Set ultrasonic sensor pins
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  
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
  
  // Measure distance with ultrasonic sensor
  float distance = measureDistance();
  
  // Calculate pages left based on the measured distance
  int page_level = calculatePagesLeft(distance);
  
  // Print readings to serial monitor for debugging
  Serial.print("Water Level: ");
  Serial.println(waterLevel);
  Serial.print("Distance (cm): ");
  Serial.println(distance);
  Serial.print("Pages Left: ");
  Serial.println(page_level);
  
  // Write data to ThingSpeak
  ThingSpeak.setField(1, waterLevel); // Water level in field 1
  ThingSpeak.setField(2, page_level); // Pages left in field 2
  
  // Capture the return code from writeFields
  int httpCode = ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY);
  
  if (httpCode == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code: " + String(httpCode));
  }
  
  delay(15000);  // ThingSpeak limits updates to once every 15 seconds
}