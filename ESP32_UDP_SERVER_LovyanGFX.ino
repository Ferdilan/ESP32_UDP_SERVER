#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AsyncUDP.h"
#include <vector>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define CHUNK_LENGTH  1204

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// const char* ssid = "PONOROGO HEBAT_plus";
// const char* password = "ponorogohebat";
const char* ssid = "ESP32_Server";
const char* password = "12345678";
const int udpPort = 8000;

AsyncUDP udp;
std::vector<uint8_t> byte_vector;

void setup() {
    Serial.begin(115200);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println("Starting WiFi...");
    display.display();

    WiFi.softAP(ssid, password);
    delay(1000); 
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP Address: ");
    Serial.println(myIP);
    
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("IP: ");
    display.println(myIP);
    display.display();

    if (udp.listen(udpPort)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        // udp.onPacket([](AsyncUDPPacket packet) {
        //     Serial.printf("Packet Length: %d\n", packet.length());

        //     std::vector<uint8_t> temp_vector(&packet.data()[0], &packet.data()[packet.length()]);

        //     if (packet.length() == CHUNK_LENGTH && 
        //         packet.data()[0] == 255 && 
        //         packet.data()[1] == 216 && 
        //         packet.data()[2] == 255) { // FF D8 FF (Start of JPEG)
        //         byte_vector.clear();
        //     }
            
        //     byte_vector.insert(byte_vector.end(), temp_vector.begin(), temp_vector.end());

        //     if (packet.length() != CHUNK_LENGTH && 
        //         packet.data()[packet.length() - 2] == 255 && 
        //         packet.data()[packet.length() - 1] == 217) { // FF D9 (End of JPEG)
        //         displayImage();
        //     }
        // });  
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.printf("Packet Length: %d\n", packet.length());

            if (packet.length() == 1024) {
                // Salin data langsung
                byte_vector.clear();
                byte_vector.insert(byte_vector.end(), packet.data(), packet.data() + packet.length());

                displayImage();
            } else {
                Serial.printf("Ukuran tidak cocok: %d byte\n", packet.length());
            }
        });
    }
}

void loop() {
}
    
void displayImage() {
    if (byte_vector.size() == 1024) {
        display.clearDisplay();
        display.drawBitmap(0, 0, byte_vector.data(), 128, 64, SSD1306_WHITE);
        display.display();
        Serial.println("Gambar ditampilkan.");
    } else {
        Serial.printf("Ukuran byte_vector salah: %d byte\n", byte_vector.size());
    }
}


