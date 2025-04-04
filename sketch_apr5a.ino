#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Oneplus";
const char* password = "PaSSworD";
AsyncWebServer server(80);

// Function to list files
void listFiles(String &fileList) {
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
        String fileName = file.name();
        fileList += "<div class='file-item'>" 
                    "<span>" + fileName + "</span>"
                    "<a href='" + fileName + "' target='_blank'><button class='view-btn'>View File</button></a>"
                    "</div>";
        file = root.openNextFile();
    }
}

// Function to delete a file
void deleteFile(const char* path) {
    if (SPIFFS.remove(path)) {
        Serial.printf("File %s deleted successfully\n", path);
    } else {
        Serial.printf("Failed to delete file %s\n", path);
    }
}

// Function to format SPIFFS
void formatFS() {
    if (SPIFFS.format()) {
        Serial.println("Filesystem formatted successfully");
    } else {
        Serial.println("Failed to format filesystem");
    }
}

// Web Server Setup
void setupServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String fileList;
        listFiles(fileList);

        String page = "<!DOCTYPE html><html><head><title>ESP32 File Upload</title>"
                      "<style>"
                      "body { font-family: Segoe UI, sans-serif; text-align: center; background:linear gradient(135deg, #007bff, #45b6fe);  padding: 20px; }"
                      "h2 { color: #333; }"
                      ".container { background: white; margin: 50px auto; padding: 30px 40px; border-radius: 12px; box-shadow: 0px 10px 25px rgba(0,0,0,0.2); display: inline-block; width: 90%; max-width:500px }"
                      
                      "input[type=file] { margin: 10px; }"
                      ".upload-btn { background-color: #28a745; color: white; border: none; padding: 10px 15px; cursor: pointer; border-radius: 5px; }"
                      ".upload-btn:hover { background-color: #218838; }"
                      ".file-item { display: flex; justify-content: space-between; align-items: center; margin-top: 10px; padding: 10px; border-bottom: 1px solid #ddd; }"
                      ".view-btn { background-color: #007bff; color: white; border: none; padding: 5px 10px; cursor: pointer; border-radius: 5px; }"
                      ".view-btn:hover { background-color: #0056b3; }"
                      "</style></head><body>"

                      "<div class='container'>"
                      "<h1>TEAM THINKSPIRE</h1>"
                      "<h2>Please upload your files below</h2>"
                      "<form method='POST' action='/upload' enctype='multipart/form-data'>"
                      "<input type='file' name='file'>"
                      "<br><br>"
                      "<input type='submit' value='Upload' class='upload-btn'>"
                      "</form><br><br>"
                      "<h3>Uploaded Files</h3>" + fileList + 
                      "</div></body></html>";

        request->send(200, "text/html", page);
    });

    // File Upload Handler
    server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Upload successful! Refresh to view.");
    }, [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
        static File file;
        String filePath = "/" + filename;

        if (!index) {
            Serial.printf("Uploading: %s\n", filePath.c_str());
            file = SPIFFS.open(filePath, "w");
        }
        if (file) {
            file.write(data, len);
        }
        if (final) {
            file.close();
            Serial.println("Upload Complete");
        }
    });

    // Serve Uploaded Files
    server.onNotFound([](AsyncWebServerRequest *request) {
        String filePath = request->url();
        Serial.println("Requested File: " + filePath);
        if (SPIFFS.exists(filePath)) {
            String contentType = "application/octet-stream";
            if (filePath.endsWith(".jpg")) contentType = "image/jpeg";
            else if (filePath.endsWith(".png")) contentType = "image/png";
            else if (filePath.endsWith(".pdf")) contentType = "application/pdf";

            request->send(SPIFFS, filePath, contentType);
        } else {
            Serial.println("File Not Found: " + filePath);
            request->send(404, "text/plain", "File Not Found");
        }
    });

    server.begin();
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected! ESP32 IP Address: " + WiFi.localIP().toString());

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    Serial.println("SPIFFS Mount Successful");
    setupServer();
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.startsWith("delete ")) {
            String filename = command.substring(7);
            if (!filename.startsWith("/")) filename = "/" + filename; // Ensure correct path
            deleteFile(filename.c_str());
        } else if (command == "format") {
            formatFS();
        } else if (command == "list") {
            Serial.println("Files in SPIFFS:");
            File root = SPIFFS.open("/");
            File file = root.openNextFile();
            while (file) {
                Serial.println(file.name());
                file = root.openNextFile();
            }
        } else {
            Serial.println("Invalid command! Use: \n  delete <filename> \n  format \n  list");
        }
    }
}