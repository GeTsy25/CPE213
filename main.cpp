#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <ESP32Servo.h>

#define DHTPIN 15
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SERVO1_PIN 14
#define BTN_UP 32
#define BTN_DOWN 33

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo servo1;

int humidityThreshold = 65;
float currentHumidity = 0;
unsigned long lastRead = 0;
bool btnUpPrev = HIGH;
bool btnDownPrev = HIGH;

// state servo: 0 = ปิด, 1 = เปิด
bool servoOpen = false;

void setup() {
  Serial.begin(115200);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  servo1.attach(SERVO1_PIN, 500, 2400);
  servo1.write(0); // เริ่มปิด

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  display.setCursor(20, 20);
  display.println("System Init");
  display.display();
  delay(2000);
}

void loop() {
  // ----- Button Handling -----
  bool btnUpNow = digitalRead(BTN_UP);
  bool btnDownNow = digitalRead(BTN_DOWN);

  if (btnUpPrev == HIGH && btnUpNow == LOW) {
    humidityThreshold++;
    Serial.printf("Threshold UP: %d\n", humidityThreshold);
  }
  if (btnDownPrev == HIGH && btnDownNow == LOW) {
    humidityThreshold--;
    Serial.printf("Threshold DOWN: %d\n", humidityThreshold);
  }
  btnUpPrev = btnUpNow;
  btnDownPrev = btnDownNow;

  // ----- Sensor Read every 2s -----
  if (millis() - lastRead >= 2000) {
    lastRead = millis();
    float h = dht.readHumidity();

    if (isnan(h)) {
      Serial.println("Failed to read from DHT!");
      display.clearDisplay();
      display.setCursor(10, 20);
      display.println("Sensor Err");
      display.display();
      return;
    }

    currentHumidity = h;

    // OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("H:");
    display.print(currentHumidity, 1);
    display.println("%");

    display.setCursor(0, 35);
    display.print("Set:");
    display.print(humidityThreshold);
    display.println("%");
    display.display();

    // ----- Stable Servo Logic (with hysteresis) -----
    if (!servoOpen && currentHumidity < humidityThreshold - 3) {
      // ปิดอยู่ และความชื้นต่ำเกิน → เปิด
      servo1.write(90);
      servoOpen = true;
      Serial.println("Servo -> OPEN (90°)");
    } 
    else if (servoOpen && currentHumidity >= humidityThreshold ) {
      // เปิดอยู่ และความชื้นสูงเกิน → ปิด
      servo1.write(0);
      servoOpen = false;
      Serial.println("Servo -> CLOSE (0°)");
    } 
    else {
      // อยู่ในช่วง threshold ±3 → คงสถานะเดิม
      Serial.println("Humidity stable, no change");
    }
  }
}
