#include <ESP8266WiFi.h>  // ไลบรารีสำหรับเชื่อมต่อ WiFi
#include <Servo.h>  // ไลบรารีสำหรับควบคุม Servo motor

const char* ssid = "Nasiminyak";  // ชื่อเครือข่าย WiFi
const char* password = "0973453602";  // รหัสผ่าน WiFi
const int port = 8888;  // พอร์ตสำหรับเชื่อมต่อ

WiFiServer server(port);  // สร้างเซิร์ฟเวอร์  WiFi
Servo myServoX;  // สร้างวัตถุ Servo สำหรับแกน X
Servo myServoY;  // สร้างวัตถุ Servo สำหรับแกน Y
Servo myServoZ;  // สร้างวัตถุ Servo สำหรับแกน Z

void setup() {
  myServoX.attach(2);  // กำหนดขา D4 สำหรับ Servo X
  myServoY.attach(0);  // กำหนดขา D3 สำหรับ Servo Y
  myServoZ.attach(5);  // กำหนดขา D1 สำหรับ Servo Z
  Serial.begin(9600);  // เริ่มการสื่อสารผ่าน Serial port
  WiFi.begin(ssid, password);  // เริ่มการเชื่อมต่อ WiFi

  while (WiFi.status() != WL_CONNECTED) {  // รอจนกว่าเชื่อมต่อ WiFi สำเร็จ
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");  // แสดงข้อความเมื่อเชื่อมต่อ WiFi สำเร็จ
  Serial.println(WiFi.localIP());  // แสดง IP ของ ESP8266
  //myServoX.write(90);
  //delay(1000);
  //myServoY.write(90);
  //delay(1000);
  //myServoZ.write(0);
  //delay(1000);

  server.begin();  // เริ่มเซิร์ฟเวอร์ WiFi
}

unsigned long previousMillis = 0;  // เก็บค่าเวลาล่าสุดที่ใช้ในการหมุน Servo
const long interval = 10;  // ระยะเวลาระหว่างการหมุน Servo (100 milliseconds)

void loop() {
  WiFiClient client = server.available();  // รอการเชื่อมต่อจากไคลเอนต์

  if (client) {
    Serial.println("Client connected");
    client.setTimeout(0);
    Serial.print("กำลังเซ็ตแกน...");
    myServoX.write(90);  // ตั้งค่า Servo X ไปที่ 90 องศา
    delay(1000);
    myServoY.write(90);  // ตั้งค่า Servo Y ไปที่ 90 องศา
    delay(1000);
    myServoZ.write(0);  // ตั้งค่า Servo Z ไปที่ 0 องศา
    delay(1000);
    Serial.println("เซ็ตสำเร็จ");

    while (client.connected()) {  // ตรวจสอบว่าคลายเอนต์ยังเชื่อมต่ออยู่หรือไม่
      if (client.available() > 0) {  // ตรวจสอบว่ามีข้อมูลจากคลายเอนต์
        int x = client.parseInt();  // รับค่าพิกัด x จาก Python script
        client.read();  // อ่านตัวคั่น (',')
        int y = client.parseInt();  // รับค่าพิกัด y จาก Python script
        Serial.print("X = ");
        Serial.print(x);
        Serial.print(", Y = ");
        Serial.println(y);

        if (x < 200) {  // ถ้าค่าพิกัด x น้อยกว่า 200
          myServoX.attach(2);
          unsigned long currentMillis = millis();  // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันซ้าย");
            myServoX.write(myServoX.read() + 3);  // เพิ่มค่า angle ขึ้น 3
            previousMillis = currentMillis;  // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        } else if (x > 250) {  // ถ้าค่าพิกัด x มากกว่า 250
          myServoX.attach(2);
          unsigned long currentMillis = millis();  // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันขวา");
            myServoX.write(myServoX.read() - 3);  // ลดค่า angle ลง 3
            previousMillis = currentMillis;  // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        }
        
        // ควบคุมแกน y
        if (y > 200) {  // ถ้าค่าพิกัด y มากกว่า 200
          myServoY.detach();  // ถอด Servo motor ของแกน y
          unsigned long currentMillis = millis();  // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันลง");
            myServoY.attach(0);  // Attach Servo motor ของแกน y กลับ
            myServoY.write(myServoY.read() + 1);  // เพิ่มค่า angle ขึ้น 1
            previousMillis = currentMillis;  // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        } else if (y < 180) {  // ถ้าค่าพิกัด y น้อยกว่า 180
          myServoY.detach();  // ถอด Servo motor ของแกน y
          unsigned long currentMillis = millis();  // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันขึ้น");
            myServoY.attach(0);  // Attach Servo motor ของแกน y กลับ
            myServoY.write(myServoY.read() - 1);  // ลดค่า angle ลง 1
            previousMillis = currentMillis;  // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        } else {  // ถ้าค่าพิกัด x และ y อยู่ในช่วงที่กำหนด
          myServoZ.attach(5);  // Attach Servo motor ของแกน z
          Serial.println("ยิง!!");
          myServoZ.write(180);  // หมุน Servo ไปที่ 180 องศา
          delay(500);
          myServoZ.write(180);
          delay(100);
          myServoZ.write(0);  // หมุน Servo กลับไปที่ 0 องศา
          delay(500);

          //myServoZ.detach();
        }
      }
    }
  }
}
