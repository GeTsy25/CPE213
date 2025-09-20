#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <ESP32Servo.h>

// ===== CONFIG =====
#define DHTPIN 15
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SERVO1_PIN 14   // Servo1
#define BTN_UP 32
#define BTN_DOWN 33

// ===== OBJECTS =====
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo servo1;

// ===== VARIABLES =====
int humidityThreshold = 60;
int currentHumidity = 0;
unsigned long lastRead = 0;

void setup() {
  Serial.begin(115200);

  // DHT
  dht.begin();

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;); // ถ้าไม่เจอจอให้ค้าง
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  // Servo setup (min/max pulse)
  servo1.attach(SERVO1_PIN, 500, 2400); 
  servo1.write(90);  // stop (สำหรับ servo 360°)

  // Buttons
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  // Welcome screen
  display.setCursor(15, 20);
  display.println("Humidity");
  display.setCursor(20, 45);
  display.println("System");
  display.display();
  delay(2000);
}

void loop() {
  if (millis() - lastRead >= 2000) {
    lastRead = millis();
    currentHumidity = dht.readHumidity();

    // ปุ่มปรับ threshold
    if (digitalRead(BTN_UP) == LOW) {
      humidityThreshold++;
      delay(200);
    }
    if (digitalRead(BTN_DOWN) == LOW) {
      humidityThreshold--;
      delay(200);
    }

    // OLED display
    display.clearDisplay();
    display.setTextSize(2);
    if (isnan(currentHumidity)) {
      display.setCursor(10, 20);
      display.println("Sensor Err");
    } else {
      display.setCursor(0, 0);
      display.print("H:");
      display.print(currentHumidity);
      display.println("%");

      display.setCursor(0, 35);
      display.print("Set:");
      display.print(humidityThreshold);
      display.println("%");
    }
    display.display();

    // Logic Servo
    if (!isnan(currentHumidity) && currentHumidity < humidityThreshold) {
      // Servo1 (360°) หมุน 5 วิ
      servo1.write(180); 
      delay(5000);

      // อ่านค่าซ้ำ
      currentHumidity = dht.readHumidity();
      if (currentHumidity < humidityThreshold) {
        servo1.write(180);
        delay(3000);
      }

      // ปิด servo
      servo1.write(90);  // stop
    }
  }
}
