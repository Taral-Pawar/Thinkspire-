#include <WiFi.h>
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>

static char ssid[] = "Oneplus";
static char pass[] = "PaSSworD";
const static char pubkey[] = "pub-c-08a8d6f1-e17a-460b-9e4f-e50177468bff";
const static char subkey[] = "sub-c-238ca340-9be4-46b4-bfa4-5c3a7907f1cb";
const static char channel[] = "paymentTrigger";
String message;

void setup() {
    Serial.begin(115200);
    Serial.println("Attempting to connect...");
    WiFi.begin(ssid, pass);
    
    // Wait for connection with timeout
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
        delay(500);
        Serial.print(".");
        timeout++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Couldn't connect to WiFi.");
        while(1) delay(100);
    } else {
        Serial.println("");
        Serial.print("Connected to SSID: ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        PubNub.begin(pubkey, subkey); // Start PubNub.
        Serial.println("PubNub is set up.");
    }
}

void loop() {
    { // Subscribe.
        PubSubClient* sclient = PubNub.subscribe(channel); // Subscribe.
        if (0 == sclient) {
            Serial.println("Error subscribing to channel.");
            delay(1000);
            return;
        }
        while (sclient->wait_for_data()) { // Print messages.
            Serial.write(sclient->read());            
        }
        Serial.println("Payment Received");
        sclient->stop();
    }
    
    delay(1000);
}