#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

const char* ssid = "Oneplus";
const char* password = "PaSSworD";
AsyncWebServer server(80);

// UPI ID for payments
const char* upiId = "7284836978@pthdfcO";

// Function to list files
void listFiles(String &fileList) {
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
        String fileName = file.name();
        fileList += "<div class='file-item'>" 
                    "<span>" + fileName + "</span>"
                    "<div class='button-group'>"
                    "<a href='" + fileName + "' target='_blank'><button class='view-btn'>View</button></a>"
                    "<button class='delete-btn' onclick='deleteFile(\"" + fileName + "\")'>Delete</button>"
                    "</div>"
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

// Save the QR code as a file on SPIFFS
bool saveQRCodeToSPIFFS() {
    // This is a placeholder. In a real implementation, you would:
    // 1. Store the QR code image in SPIFFS during setup
    // 2. Either embed the QR code base64 encoded, or
    // 3. Upload it to SPIFFS using a separate upload tool
    
    // For demonstration, we'll check if a placeholder file exists
    if (!SPIFFS.exists("/qrcode.htm")) {
        File file = SPIFFS.open("/qrcode.htm", "w");
        if (!file) {
            Serial.println("Failed to create QR code file");
            return false;
        }
        
        // Write HTML with the QR code (in reality, you would store the actual image)
        file.println("<html><body style='text-align:center;'>");
        file.println("<h2>Paytm / UPI Payment</h2>");
        file.println("<div style='margin:20px;'>"); 
        file.println("<div style='font-size:24px;margin-bottom:10px;'>UPI ID: 7284836978@pthdfcO</div>");
        file.println("</div></body></html>");
        file.close();
        
        Serial.println("QR code placeholder file created");
    }
    return true;
}

// Web Server Setup
void setupServer() {
    // Save QR code to SPIFFS
    saveQRCodeToSPIFFS();

    // Main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String fileList;
        listFiles(fileList);

        String page = "<!DOCTYPE html><html><head><title>ESP32 File Upload</title>"
                      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                      "<style>"
                      "body { font-family: Segoe UI, sans-serif; text-align: center; background: linear-gradient(135deg, #007bff, #45b6fe); padding: 50px 20px; margin: 0; }"
                      "h2 { color: #333; font-size: 1.8rem; }"
                      ".container { background: white; margin: 20px auto; padding: 50px; border-radius: 15px; box-shadow: 0px 15px 30px rgba(0,0,0,0.25); display: block; width: 95%; max-width: 900px; }"
                      "h1 { color: #007bff; margin-bottom: 20px; font-size: 2.5rem; }"
                      "h3 { margin-bottom: 25px; font-size: 1.6rem; }"
                      "input[type=file] { margin: 15px 0; padding: 12px; border: 2px solid #ccc; border-radius: 8px; width: 100%; font-size: 1.2rem; }"
                      ".upload-btn { background-color: #007bff; color: white; border: none; padding: 15px 30px; cursor: pointer; border-radius: 8px; transition: background 0.3s ease; font-size: 1.3rem; font-weight: bold; width: 100%; max-width: 400px; }"
                      ".upload-btn:hover { background-color: #0056b3; transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.1); }"
                      ".file-item { display: flex; justify-content: space-between; align-items: center; margin-top: 15px; padding: 15px 20px; border-radius: 8px; background-color: #f2f9ff; border: 1px solid #d9ecff; font-size: 1.2rem; }"
                      ".view-btn { background-color: #17a2b8; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; cursor: pointer; transition: background 0.3s ease; font-size: 1.1rem; }"
                      ".view-btn:hover { background-color: #117a8b; transform: translateY(-2px); }"
                      ".delete-btn { background-color: #dc3545; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; margin-left: 10px; font-size: 1.1rem; }"
                      ".delete-btn:hover { background-color: #c82333; transform: translateY(-2px); }"
                      ".button-group { display: flex; gap: 10px; }"
                      ".form-group { margin: 20px 0; display: flex; flex-direction: column; align-items: center; width: 100%; }"
                      ".form-group label { font-size: 1.3rem; margin-bottom: 10px; color: #333; }"
                      ".quantity-input { padding: 12px; border-radius: 8px; border: 2px solid #ced4da; width: 150px; text-align: center; margin: 10px; font-size: 1.2rem; }"
                      ".payment-btn { background-color: #007bff; color: white; border: none; padding: 15px 30px; cursor: pointer; border-radius: 8px; transition: all 0.3s ease; font-size: 1.3rem; font-weight: bold; width: 100%; max-width: 400px; margin-top: 20px; }"
                      ".payment-btn:hover { background-color: #0056b3; transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.1); }"
                      ".select-paper { padding: 12px; border-radius: 8px; border: 2px solid #ced4da; width: 200px; margin: 10px; font-size: 1.2rem; }"
                      ".section { margin: 30px 0; padding: 30px; background-color: #f8f9fa; border-radius: 10px; }"
                      ".divider { height: 2px; background-color: #e9ecef; margin: 30px 0; }"
                      ".input-container { width: 100%; max-width: 500px; }"
                      "@media (max-width: 768px) {"
                      "  .container { padding: 30px 20px; width: 95%; }"
                      "  h1 { font-size: 2rem; }"
                      "  h2 { font-size: 1.5rem; }"
                      "  .form-group { flex-direction: column; }"
                      "  .upload-btn, .payment-btn { padding: 12px 20px; }"
                      "  .file-item { flex-direction: column; gap: 10px; text-align: center; }"
                      "  .button-group { justify-content: center; width: 100%; margin-top: 10px; }"
                      "}"
                      "</style></head><body>"
                      "<div class='container'>"
                      "<h1>TEAM THINKSPIRE</h1>"
                      "<h2>Please upload your files below</h2>"
                      "<div class='section'>"
                      "<form method='POST' action='/upload' enctype='multipart/form-data'>"
                      "<div class='input-container'>"
                      "<input type='file' name='file'>"
                      "</div>"
                      "<br><br>"
                      "<input type='submit' value='Upload' class='upload-btn'>"
                      "</form>"
                      "</div>"
                      "<div class='divider'></div>"
                      "<div class='section'>"
                      "<h3>Uploaded Files</h3>"  + fileList + 
                      "</div>"
                      "<div class='divider'></div>"
                      "<div class='section'>"
                      "<h3>Print Settings</h3>"
                      "<div class='form-group'>"
                      "<label for='printQuantity'>Number of Prints:</label>"
                      "<input type='number' id='printQuantity' class='quantity-input' min='1' value='1'>"
                      "</div>"
                      "<div class='form-group'>"
                      "<label for='paperType'>Paper Type:</label>"
                      "<select id='paperType' class='select-paper'>"
                      "<option value='plain'>Plain</option>"
                      "<option value='glossy'>Glossy</option>"
                      "</select>"
                      "</div>"
                      "<div class='form-group'>"
                      "<label for='paperSize'>Paper Size:</label>"
                      "<select id='paperSize' class='select-paper'>"
                      "<option value='A4'>A4</option>"
                      "<option value='A3'>A3</option>"
                      "</select>"
                      "</div>"
                      "<button onclick='proceedToPayment()' class='payment-btn'>Proceed to Payment</button>"
                      "</div>"
                      "</div>"
                      
                      "<script>"
                      "function deleteFile(fileName) {"
                      "  if (confirm('Are you sure you want to delete this file?')) {"
                      "    fetch('/delete?file=' + encodeURIComponent(fileName), {"
                      "      method: 'GET'"
                      "    })"
                      "    .then(response => {"
                      "      if (response.ok) {"
                      "        alert('File deleted successfully');"
                      "        location.reload();"
                      "      } else {"
                      "        alert('Error deleting file');"
                      "      }"
                      "    })"
                      "    .catch(error => {"
                      "      console.error('Error:', error);"
                      "      alert('An error occurred while deleting the file.');"
                      "    });"
                      "  }"
                      "}"
                      
                      "function proceedToPayment() {"
                      "  const quantity = document.getElementById('printQuantity').value;"
                      "  const paperType = document.getElementById('paperType').value;"
                      "  const paperSize = document.getElementById('paperSize').value;"
                      "  if (quantity < 1) {"
                      "    alert('Please specify at least 1 print.');"
                      "    return;"
                      "  }"
                      "  const paymentURL = '/payment?quantity=' + quantity + '&paperType=' + paperType + '&paperSize=' + paperSize;"
                      
                      "  window.open(paymentURL, '_blank');"
                      "}"
                      "</script></body></html>";

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

    // Delete File Handler
    server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("file")) {
            String fileName = request->getParam("file")->value();
            if (!fileName.startsWith("/")) {
                fileName = "/" + fileName;
            }
            deleteFile(fileName.c_str());
            request->send(200, "text/plain", "File deleted");
        } else {
            request->send(400, "text/plain", "File parameter missing");
        }
    });

    // Payment Page Handler
    server.on("/payment", HTTP_GET, [](AsyncWebServerRequest *request) {
        String quantity = "1";
        String paperType = "plain";
        String paperSize = "A4";  // Default paper size
        
        if (request->hasParam("quantity")) {
            quantity = request->getParam("quantity")->value();
        }
        
        if (request->hasParam("paperType")) {
            paperType = request->getParam("paperType")->value();
        }
        
        if (request->hasParam("paperSize")) {
            paperSize = request->getParam("paperSize")->value();
        }
        
        String paperTypeDisplay = (paperType == "glossy") ? "Glossy" : "Plain";
        String paperSizeDisplay = (paperSize == "A4") ? "A4" : "A3";
        
        String paymentPage = "<!DOCTYPE html><html><head><title>Payment - Team Thinkspire</title>"
                            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                            "<style>"
                            "body { font-family: Segoe UI, sans-serif; text-align: center; background: linear-gradient(135deg, #007bff, #45b6fe); padding: 40px 20px; margin: 0; }"
                            ".container { background: white; margin: 20px auto; padding: 50px; border-radius: 15px; box-shadow: 0px 15px 30px rgba(0,0,0,0.25); display: block; width: 95%; max-width: 900px; }"
                            "h1 { color: #007bff; margin-bottom: 20px; font-size: 2.5rem; }"
                            "h2 { color: #333; font-size: 1.8rem; margin-bottom: 30px; }"
                            ".qr-code { width: 400px; max-width: 100%; margin: 30px auto; padding: 20px; border: 2px solid #eee; border-radius: 10px; }"
                            ".qr-code svg { width: 350px; height: 350px; max-width: 100%; }"
                            ".print-btn { background-color: #28a745; color: white; border: none; padding: 16px 32px; cursor: pointer; border-radius: 8px; transition: all 0.3s ease; font-size: 1.4rem; font-weight: bold; margin-top: 30px; width: 100%; max-width: 400px; }"
                            ".print-btn:hover { background-color: #218838; transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.1); }"
                            ".order-details { background-color: #f8f9fa; border-radius: 10px; padding: 30px; margin-bottom: 30px; font-size: 1.3rem; }"
                            ".order-details p { margin: 10px 0; }"
                            ".payment-logos { display: flex; justify-content: center; margin-bottom: 20px; }"
                            ".payment-logos img { height: 50px; margin: 0 15px; }"
                            ".upi-id { font-size: 1.5rem; font-weight: bold; margin: 25px 0; padding: 15px; background: #f0f0f0; border-radius: 8px; word-break: break-all; }"
                            ".section { margin: 40px 0; padding: 30px; background-color: #f8f9fa; border-radius: 10px; }"
                            "@media (max-width: 768px) {"
                            "  .container { padding: 30px 20px; width: 95%; }"
                            "  h1 { font-size: 2rem; }"
                            "  h2 { font-size: 1.5rem; }"
                            "  .print-btn { padding: 12px 24px; font-size: 1.2rem; }"
                            "  .upi-id { font-size: 1.2rem; padding: 10px; }"
                            "}"
                            "</style></head><body>"
                            "<div class='container'>"
                            "<h1>TEAM THINKSPIRE</h1>"
                            "<h2>Payment</h2>"
                            "<div class='section'>"
                            "<div class='order-details'>"
                            "<p><strong>Order Summary</strong></p>"
                            "<p>Number of prints: " + quantity + "</p>"
                            "<p>Paper type: " + paperTypeDisplay + "</p>"
                            "<p>Paper size: " + paperSizeDisplay + "</p>"
                            "</div>"
                            "</div>"
                            "<div class='section'>"
                            "<p style='font-size: 1.3rem;'>Scan the QR code below to complete your payment</p>"
                            "<div class='qr-code'>"
                            "<svg width='350' height='350' viewBox='0 0 200 200'>"
                            "<rect width='200' height='200' fill='white'/>"
                            "<path d='M40,40 h40 v40 h-40 z M50,50 h20 v20 h-20 z' fill='black'/>"
                            "<path d='M120,40 h40 v40 h-40 z M130,50 h20 v20 h-20 z' fill='black'/>"
                            "<path d='M40,120 h40 v40 h-40 z M50,130 h20 v20 h-20 z' fill='black'/>"
                            "<path d='M90,90 h20 v20 h-20 z' fill='black'/>"
                            "<path d='M120,120 h10 v10 h-10 z M140,120 h10 v10 h-10 z M130,130 h10 v10 h-10 z M150,130 h10 v10 h-10 z M120,140 h10 v10 h-10 z M140,140 h10 v10 h-10 z M120,150 h40 v10 h-40 z' fill='black'/>"
                            "</svg>"
                            "</div>"
                            "<div class='upi-id'>UPI ID: 7284836978@pthdfcO</div>"
                            "</div>"
                            "<button onclick='startPrinting()' class='print-btn'>Print</button>"
                            "</div>"
                            "<script>"
                            "function startPrinting() {"
                            "  fetch('/print', {"
                            "    method: 'POST',"
                            "    headers: {"
                            "      'Content-Type': 'application/json'"
                            "    },"
                            "    body: JSON.stringify({"
                            "      quantity: " + quantity + ","
                            "      paperType: '" + paperType + "',"
                            "      paperSize: '" + paperSize + "'"
                            "    })"
                            "  })"
                            "  .then(response => response.text())"
                            "  .then(data => {"
                            "    alert('Print job started successfully!');"
                            "  })"
                            "  .catch(error => {"
                            "    console.error('Error:', error);"
                            "    alert('An error occurred while sending the print command.');"
                            "  });"
                            "}"
                            "document.addEventListener('DOMContentLoaded', function() {"
                            "  alert('Please note: To continue make the payment to the QR code ');"
                            "});"
                            "</script>"
                            "</body></html>";
        
        request->send(200, "text/html", paymentPage);
    });

    // Print handler
    server.on("/print", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Print job started");
    }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data, len);
        
        int quantity = doc["quantity"];
        String paperType = doc["paperType"];
        String paperSize = doc["paperSize"];
        
        Serial.printf("Print job received: %d copies on %s %s paper\n", quantity, paperSize.c_str(), paperType.c_str());
        // Here you would add code to actually start the print job
        
        request->send(200, "text/plain", "Print job started");
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