# 🖨️ Automatic Paper Printing Kiosk

A smart, self-service paper printing kiosk developed during a hackathon by **Team Thinkspire**. The system integrates hardware design, embedded systems, a touch-screen UI, cloud-based document handling, and MATLAB-based simulation to offer a seamless 24x7 automated printing solution.

## 🔧 Features

### 🖥️ User Interface
- File uploads via web interface or email (PDF, JPG, PNG, etc.)
- Live file preview and manual delete options
- Touchscreen input for number of copies, paper size/type selection
- QR code scanner integration
- Page counter with real-time status display

### 📤 File Upload System
- **IMAP** protocol to fetch attachments from user email
- **SMTP** for sending confirmation emails
- Works in both offline and online modes

### 💳 Payment Integration
- Simulated PayPal webhook for payment verification
- Uses **ESP32** with **PubNub MQTT** to demonstrate payment reception

### 🛠️ Hardware Overview
- CAD-designed kiosk with paper trays, ink tank access, ventilation
- Powered by **ESP32**, **Raspberry Pi 5**
- 6 stepper motors, 4 ultrasonic sensors (paper trays), 4 ink sensors (CMYK)
- LCD touchscreen for interactive control

### 📊 Sensor Monitoring
- Real-time ink & paper monitoring via **ThingSpeak**
- Data analytics and trend visualization using **MATLAB**

### 🧪 MATLAB Simulink Simulation
- Image acquisition & preprocessing in MATLAB
- Printhead control simulation using ESP32 logic
- Tests control signal accuracy and actuator timing

## 👨‍💻 Team Thinkspire
- **Parth Patel** – UI Design, MATLAB Simulation
- **Taral Pawar** – CAD Model, PCB Design
- **Partha Pratim Roy** – UI, MATLAB Simulation, PCB Design


### 💡 Inspiration
Creating a 24x7, self-sufficient print kiosk for remote campuses, libraries, and public places to simplify the document printing process.

