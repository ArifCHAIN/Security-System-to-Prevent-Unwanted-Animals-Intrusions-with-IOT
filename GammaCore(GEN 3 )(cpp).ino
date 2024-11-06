#include <ESP8266WiFi.h>
#include <Servo.h>

const char* ssid = "Nasiminyak";
const char* password = "0973453602";
const int port = 8888;

WiFiServer server(port);
Servo myServoX;
Servo myServoY;
Servo myServoZ;

void setup() {
  myServoX.attach(2); // Pin D4
  myServoY.attach(0); // Pin D3
  myServoZ.attach(5); // Pin D1
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP()); // Display ESP8266's IP
    //myServoX.write(90);
    //delay(1000);
    //myServoY.write(90);
    //delay(1000);
    //myServoZ.write(0);
    //delay(1000);

  server.begin();
}

unsigned long previousMillis = 0; // เก็บค่าเวลาล่าสุดที่ใช้ในการหมุน Servo
const long interval = 10; // ระยะเวลาระหว่างการหมุน Servo (100 milliseconds)

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected");
    client.setTimeout(0);
    Serial.print("กำลังเซ็ตแกน...");
    myServoX.write(90);
    delay(1000);
    myServoY.write(90);
    delay(1000);
    myServoZ.write(0);
    delay(1000);
    Serial.println("เซ็ตสำเร็จ");

    while (client.connected()) {
      if (client.available() > 0) {
        int x = client.parseInt(); // Receive x value from Python script
        client.read(); // Read separator (',')
        int y = client.parseInt(); // Receive y value from Python script
        Serial.print("X = ");
        Serial.print(x);
        Serial.print(", Y = ");
        Serial.println(y);

        if (x < 200) {
          myServoX.attach(2);
          unsigned long currentMillis = millis(); // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันซ้าย");
            myServoX.write(myServoX.read() + 3); // เพิ่มค่า angle ขึ้น 3
            previousMillis = currentMillis; // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        } else if (x > 250) {
          myServoX.attach(2);
          unsigned long currentMillis = millis(); // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันขวา");
            myServoX.write(myServoX.read() - 3); // ลดค่า angle ลง 3
            previousMillis = currentMillis; // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        }
        
        // ควบคุมแกน y
        if (y > 200) {
          myServoY.detach(); // ถอด Servo motor ของแกน y
          unsigned long currentMillis = millis(); // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันลง");
            myServoY.attach(0); // Attach Servo motor ของแกน y กลับ
            myServoY.write(myServoY.read() + 1); // เพิ่มค่า angle ขึ้น 3
            previousMillis = currentMillis; // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        } else if (y < 180) {
          myServoY.detach(); // ถอด Servo motor ของแกน y
          unsigned long currentMillis = millis(); // จับเวลาปัจจุบัน
          if (currentMillis - previousMillis >= interval) {
            Serial.println("หันขึ้น");
            myServoY.attach(0); // Attach Servo motor ของแกน y กลับ
            myServoY.write(myServoY.read() - 1); // ลดค่า angle ลง 1
            previousMillis = currentMillis; // บันทึกค่าเวลาปัจจุบันเป็นค่าเวลาล่าสุด
          }
        } else {
          myServoZ.attach(5);
          Serial.println("ยิง!!");
          myServoZ.write(180);
          delay(500);
          myServoZ.write(180);
          delay(100);
          myServoZ.write(0);
          delay(500);

          //myServoZ.detach();
        }
      }
    }
  }
}
