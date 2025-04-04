#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#define WIFI_SSID "Oneplus"
#define WIFI_PASSWORD "PaSSworD"

// IMAP Configuration
#define IMAP_HOST "imap.gmail.com"
#define IMAP_PORT 993
#define AUTHOR_EMAIL "parthapratim8638@gmail.com"
#define AUTHOR_PASSWORD "uche oxjv fvqw auud"

// SMTP Configuration
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

IMAPSession imap;
SMTPSession smtp;
AsyncWebServer server(80);

void imapCallback(IMAP_Status status);
void SMTP_MSG(String sender_email);
void printMessages(std::vector<IMAP_MSG_Item> &msgItems);
void listFiles();

void setup() {
    Serial.begin(115200);
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
    }
    Serial.println("\nWiFi connected.");
    Serial.println("IP address: " + WiFi.localIP().toString());
    
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = "<h1>Downloaded Attachments</h1><ul>";
        File root = SPIFFS.open("/");
        File file = root.openNextFile();
        while (file) {
            html += "<li><a href='/" + String(file.name()) + "'>" + String(file.name()) + "</a></li>";
            file = root.openNextFile();
        }
        html += "</ul>";
        request->send(200, "text/html", html);
    });

    server.serveStatic("/", SPIFFS, "/");
    server.begin();

    imap.debug(1);
    imap.callback(imapCallback);

    ESP_Mail_Session session;
    session.server.host_name = IMAP_HOST;
    session.server.port = IMAP_PORT;
    session.login.email = AUTHOR_EMAIL;
    session.login.password = AUTHOR_PASSWORD;

    IMAP_Config config;
    config.search.unseen_msg = true;
    config.download.header = true;
    config.download.text = true;
    config.download.attachment = true;
    config.storage.saved_path = "/";
    config.storage.type = esp_mail_file_storage_type_flash;
    config.enable.text = true;
    config.enable.recent_sort = true;
    config.limit.search = 5;

    if (!imap.connect(&session, &config)) return;
    if (!imap.selectFolder("INBOX")) return;

    MailClient.readMail(&imap);
    imap.empty();
}

void loop() {
    // Loop can be used to repeatedly check emails at intervals
}

void imapCallback(IMAP_Status status) {
    Serial.println(status.info());
    if (status.success()) {
        IMAP_MSG_List msgList = imap.data();
        for (size_t i = 0; i < msgList.msgItems.size(); i++) {
            IMAP_MSG_Item msg = msgList.msgItems[i];
            String sender_email = String(msg.from);
            
            int start = sender_email.indexOf('<');
            int end = sender_email.indexOf('>');
            if (start != -1 && end != -1) {
                sender_email = sender_email.substring(start + 1, end);
            }
            
            Serial.println("Extracted Sender Email: " + sender_email);
            SMTP_MSG(sender_email);
        }
        imap.empty();
    }
}

void SMTP_MSG(String sender_email) {
    smtp.debug(1);
    smtp.callback([](SMTP_Status status) {
        Serial.println(status.info());
    });

    ESP_Mail_Session session;
    session.server.host_name = SMTP_HOST;
    session.server.port = SMTP_PORT;
    session.login.email = AUTHOR_EMAIL;
    session.login.password = AUTHOR_PASSWORD;

    SMTP_Message message;
    message.sender.name = "Thinkspire";
    message.sender.email = AUTHOR_EMAIL;
    message.subject = "Auto_Response";
    message.addRecipient("Sender", sender_email.c_str());

    String textMsg = "Thank you for reaching out! We have got your files,please wait while printing in progress.";
    message.text.content = textMsg.c_str();
    message.text.charSet = "us-ascii";

    if (!smtp.connect(&session)) return;
    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Error sending Email: " + smtp.errorReason());
    } else {
        Serial.println("Auto-response sent to: " + sender_email);
    }
}
