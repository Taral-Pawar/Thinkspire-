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
                      ".container { background: white; margin: 50px auto; padding: 30px 40px; border-radius: 12px; box-shadow: 0px 10px 25px rgba(0,0,0,0.2); display: inline-block; width: 90%; max-width:500px; }"
                      "h1 { color: #007bff; margin-bottom: 10px; }"
                      "h2,h3 { margin-bottom: 20px; }"
                      "input[type=file] { margin: 10px 0; padding:6px; border:1px solid #ccc; border-radius: 6px; width:100%; }"
                      ".upload-btn { background-color: #007bff; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; }"
                      ".upload-btn:hover { background-color: #0056b3; }"
                      ".file-item { display: flex; justify-content: space-between; align-items: center; margin-top: 12px; padding: 10px 15px; border-radius: 6px; background-color: #f2f9ff; border: 1px solid #d9ecff }"
                      ".view-btn { background-color: #17a2b8; color: white; border: none; padding: 6px 12px; cursor: pointer; border-radius: 5px; cursor: pointer; transition: background 0.3s ease; margin: 0px 100px }"
                      ".view-btn:hover { background-color: #117a8b; }"
                      ".delete-btn { background-color: #dc3545; color: white; border: none; padding: 10px 12px; cursor: pointer; border-radius: 5px; cursor: pointer; transition: background 0.3s ease; margin: 0px 100px}"
                      ".delete-btn:hover { background-color: #c82333; }"
                      ".button-group { display: flex; gap: 8px; }"
                      ".quantity-input { padding: 8px; border-radius: 5px; border: 1px solid #ced4da; width: 80px; text-align: center; margin: 0 10px; }"
                      ".payment-btn { background-color: #007bff; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; }"
                      ".payment-btn:hover { background-color: #0056b3; }"
                      ".print-btn { background-color: #28a745; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; }"
                      ".print-btn:hover { background-color: #218838; }"
                      "</style></head><body>"

                      "<div class='container'>"
                      "<h1>TEAM THINKSPIRE</h1>"
                      "<h2>Please upload your files below</h2>"
                      "<form method='POST' action='/upload' enctype='multipart/form-data'>"
                      "<input type='file' name='file'>"
                      "<br><br>"
                      "<input type='submit' value='Upload' class='upload-btn'>"
                      "<input type='button' value='Delete' class='delete-btn'>"
                      "</form><br><br>"
                      "<h3>Uploaded Files</h3>"  + fileList + 
                      "<br><br>"
                      "<div>"
                      "<label for='printQuantity'>Number of Prints:</label>"
                      "<input type='number' id='printQuantity' class='quantity-input' min='1' value='1'>"
                      "</div>"
                      "<br>"
                      "<button onclick='proceedToPayment()' class='payment-btn'>Proceed to Payment</button>"            
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




















// "<!DOCTYPE html><html><head><title>ESP32 File Upload</title>
// <style>
// body { font-family: Segoe UI, sans-serif; text-align: center; background:linear gradient(135deg, #007bff, #45b6fe);  padding: 20px; }
// h2 { color: #333; }
// .container { background: white; margin: 50px auto; padding: 30px 40px; border-radius: 12px; box-shadow: 0px 10px 25px rgba(0,0,0,0.2); display: inline-block; width: 90%; max-width:500px; }
// h1 { color: #007bff; margin-bottom: 10px; }
// h2,h3 { margin-bottom: 20px; }
// input[type=file] { margin: 10px 0; padding:6px; border:1px solid #ccc; border-radius: 6px; width:100%; }
// .upload-btn { background-color: #007bff; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; }
// .upload-btn:hover { background-color: #0056b3; }
// .file-item { display: flex; justify-content: space-between; align-items: center; margin-top: 12px; padding: 10px 15px; border-radius: 6px; background-color: #f2f9ff; border: 1px solid #d9ecff }
// .view-btn { background-color: #17a2b8; color: white; border: none; padding: 6px 12px; cursor: pointer; border-radius: 5px; cursor: pointer; transition: background 0.3s ease; }
// .view-btn:hover { background-color: #117a8b; }
// .delete-btn { background-color: #dc3545; color: white; border: none; padding: 6px 12px; cursor: pointer; border-radius: 5px; transition: background 0.3s ease; margin-left: 8px; }
// .delete-btn:hover { background-color: #c82333; }
// .button-group { display: flex; gap: 8px; }
// .quantity-input { padding: 8px; border-radius: 5px; border: 1px solid #ced4da; width: 80px; text-align: center; margin: 0 10px; }
// .payment-btn { background-color: #007bff; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; }
// .payment-btn:hover { background-color: #0056b3; }
// .print-btn { background-color: #28a745; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; }
// .print-btn:hover { background-color: #218838; }
// </style></head><body>
// <div class='container'>
// <h1>TEAM THINKSPIRE</h1>
// <h2>Please upload your files below</h2>
// <form method='POST' action='/upload' enctype='multipart/form-data'>
// <input type='file' name='file'>
// <br><br>
// <input type='submit' value='Upload' class='upload-btn'>
// </form><br><br>
// <h3>Uploaded Files</h3>" + fileList + "
// <br><br>
// <div>
// <label for='printQuantity'>Number of Prints:</label>
// <input type='number' id='printQuantity' class='quantity-input' min='1' value='1'>
// </div>
// <br>
// <button onclick='proceedToPayment()' class='payment-btn'>Proceed to Payment</button>
// </div>

// <script>
// document.addEventListener('DOMContentLoaded', function() {
//     const fileItems = document.querySelectorAll('.file-item');
    
//     fileItems.forEach(item => {
//         const fileName = item.querySelector('span').textContent;
//         const btnGroup = item.querySelector('.button-group') || document.createElement('div');
        
//         if (!item.querySelector('.button-group')) {
//             btnGroup.className = 'button-group';
//             item.appendChild(btnGroup);
//         }
        
//         if (!item.querySelector('.delete-btn')) {
//             const deleteBtn = document.createElement('button');
//             deleteBtn.className = 'delete-btn';
//             deleteBtn.textContent = 'Delete';
//             deleteBtn.onclick = function() {
//                 deleteFile(fileName);
//             };
//             btnGroup.appendChild(deleteBtn);
//         }
//     });
// });

// function deleteFile(fileName) {
//     if (confirm('Are you sure you want to delete this file?')) {
//         fetch('/delete?file=' + encodeURIComponent(fileName), {
//             method: 'DELETE'
//         })
//         .then(response => response.json())
//         .then(data => {
//             if (data.success) {
//                 location.reload();
//             } else {
//                 alert('Error deleting file: ' + data.message);
//             }
//         })
//         .catch(error => {
//             console.error('Error:', error);
//             alert('An error occurred while deleting the file.');
//         });
//     }
// }

// function proceedToPayment() {
//     const quantity = document.getElementById('printQuantity').value;
//     if (quantity < 1) {
//         alert('Please specify at least 1 print.');
//         return;
//     }
    
//     const paymentWindow = window.open('', '_blank');
    
//     const paymentContent = `
//         <!DOCTYPE html><html><head><title>Payment - Team Thinkspire</title>
//         <style>
//         body { font-family: Segoe UI, sans-serif; text-align: center; background:linear gradient(135deg, #007bff, #45b6fe);  padding: 20px; }
//         .container { background: white; margin: 50px auto; padding: 30px 40px; border-radius: 12px; box-shadow: 0px 10px 25px rgba(0,0,0,0.2); display: inline-block; width: 90%; max-width:500px; }
//         h1 { color: #007bff; margin-bottom: 10px; }
//         .qr-code { width: 200px; height: 200px; background-color: #f8f9fa; border: 1px solid #dee2e6; margin: 20px auto; padding: 10px; display: flex; align-items: center; justify-content: center; }
//         .print-btn { background-color: #28a745; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 6px; transition: background 0.3s ease; margin-top: 20px; }
//         .print-btn:hover { background-color: #218838; }
//         </style></head><body>
//         <div class='container'>
//         <h1>TEAM THINKSPIRE</h1>
//         <h2>Payment</h2>
//         <p>Number of prints: ${quantity}</p>
//         <p>Scan the QR code below to complete your payment</p>
//         <div class="qr-code">
//             <img src="/api/placeholder/200/200" alt="Payment QR Code">
//         </div>
//         <button onclick="startPrinting()" class="print-btn">Print</button>
//         </div>
//         <script>
//         function startPrinting() {
//             fetch('/print', {
//                 method: 'POST',
//                 headers: {
//                     'Content-Type': 'application/json'
//                 },
//                 body: JSON.stringify({
//                     quantity: ${quantity}
//                 })
//             })
//             .then(response => response.json())
//             .then(data => {
//                 if (data.success) {
//                     alert('Print job started successfully!');
//                 } else {
//                     alert('Error starting print: ' + data.message);
//                 }
//             })
//             .catch(error => {
//                 console.error('Error:', error);
//                 alert('An error occurred while sending the print command.');
//             });
//         }
//         </script>
//         </body></html>
//     `;
    
//     paymentWindow.document.write(paymentContent);
//     paymentWindow.document.close();
// }
// </script>
// </body></html>"