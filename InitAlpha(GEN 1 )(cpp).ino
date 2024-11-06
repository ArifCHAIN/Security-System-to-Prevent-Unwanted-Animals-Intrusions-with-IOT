#include <ESP8266WiFi.h>
#include <Servo.h>

const char* ssid = "Nasikabu";
const char* password = "0973453602";
const int port = 8888;

WiFiServer server(port);
Servo myServoX;
Servo myServoY;
Servo myServoZ;

void setup() {
  myServoX.attach(2); //D2 ตั้งค่าขาที่ Servo Motor ต่อกับ
  myServoY.attach(0); // ต่อ Servo Motor กับขา D0
  myServoZ.attach(5); // ต่อ Servo Motor กับขา D0
  Serial.begin(9600);
  myServoX.write(90);
  myServoY.write(90);
  myServoZ.write(0);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP()); // แสดง IP ของ ESP8266

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  myServoX.write(90);
  myServoY.write(90);
  myServoZ.write(0);
  myServoX.attach(2);
  myServoY.attach(0);
  myServoZ.attach(5);

  if (client) {
    Serial.println("Client connected");

    client.setTimeout(0);

     while (client.connected()) {
      if (client.available() > 0) {
        int x = client.parseInt(); // รับค่า x จาก Python script
        int servoPosition = 90;
        Serial.print("X = ");
        Serial.println(x);

    // ควบคุม Servo Motor ตามค่า x ที่รับมา
    if (x < 150) {
      for (int angle = myServoX.read(); angle > 0; angle++) {
        myServoX.attach(2);
       // Serial.println("กำลังหันแกน ขวา X");
        myServoX.write(angle); // หมุน Servo Motor ไปทางซ้าย
        delay(15);
        if (client.available() > 0) {
          break; // หยุดลูปภายใน while เมื่อมีข้อมูล x ใหม่มา
        }
      }
    } else if (x > 260) {
      for (int angle = myServoX.read(); angle < 180; angle--) {
        myServoX.attach(2);
       // Serial.println("กำลังหันแกน ซ้าย X");
        myServoX.write(angle); // หมุน Servo Motor ไปทางขวา
        delay(15);
        if (client.available() > 0) {
          break; // หยุดลูปภายใน while เมื่อมีข้อมูล x ใหม่มา
        }
      }
    } else {
      // หากไม่ได้รับค่า x
      Serial.println("เจอเป้าหมาย X");
      myServoX.detach(); // กลับไปที่ตำแหน่งเริ่มต้น (ที่กลาง)
      //delay(1000);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  {
    if (client.available() > 0) {
    int y = client.parseInt(); // รับค่า y จาก Python script
    int servoPositionY = 90;
    Serial.print("Y = ");
    Serial.println(y);
    
    // ควบคุม Servo Motor ของ y ตามค่า y ที่รับมา
    if (y < 200) {
      for (int angleY = myServoY.read(); angleY > 0; angleY--) {
        myServoY.attach(0);
        //Serial.println("กำลังหันแกน ซ้าย Y");
        myServoY.write(angleY); // หมุน Servo Motor ของ y ไปทางซ้าย
        delay(75);
        if (client.available() > 0) {
            break; // หยุดลูปภายใน while เมื่อมีข้อมูล y ใหม่มา
        }
    }
    } else if (y > 227) {
      for (int angleY = myServoY.read(); angleY < 180; angleY++) {
        myServoY.attach(0);
        //Serial.println("กำลังหันแกน ขวา Y");
        myServoY.write(angleY); // หมุน Servo Motor ของ y ไปทางขวา
        delay(75);
        if (client.available() > 0) {
            break; // หยุดลูปภายใน while เมื่อมีข้อมูล y ใหม่มา
        }
    }
    } else {
      // หากไม่ได้รับค่า y
      Serial.println("เจอเป้าหมาย y");
      myServoY.detach(); // กลับไปที่ตำแหน่งเริ่มต้น (ที่กลาง)
      delay(15); // รอ 3 วินาที
      Serial.println("ยิง"); // พิมพ์ "ยิง"

      // หมุน Servo ขาที่ 5 ไปที่ 180 องศา
      myServoZ.attach(5); // เชื่อมต่อ Servo Motor ขาที่ 5
      myServoZ.write(180); // หมุน Servo ไปที่ 180 องศา
      delay(15); // ค้างไว้ 1 วินาที
      myServoY.write(0); // กลับมาที่ 0 องศา
      delay(15); // ค้างไว้ 1 วินาที

      // ทำการ loop ต่อไป
      //myServoY.attach(0); // เชื่อมต่อ Servo motor ของ y กับขา digital ใหม่
    }
}
}
  }
}
}