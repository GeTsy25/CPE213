# ESP32 Humidity Control System

โปรเจกต์นี้ใช้ **ESP32 + DHT11 + Servo Motor + OLED Display**  
เพื่อควบคุมการเปิด/ปิด Servo ตามค่าความชื้นที่ผู้ใช้กำหนดผ่านปุ่มกด

## Features
- อ่านค่าความชื้นจาก DHT11
- แสดงค่าปัจจุบัน + ค่า Setpoint บน OLED
- ปุ่มกดเพิ่ม/ลดค่า Threshold แบบ real-time
- Servo เปิดค้างที่ 90° จนกว่าความชื้นจะกลับมาในช่วง Threshold ±3
- ป้องกันอาการสวิงด้วย hysteresis logic
