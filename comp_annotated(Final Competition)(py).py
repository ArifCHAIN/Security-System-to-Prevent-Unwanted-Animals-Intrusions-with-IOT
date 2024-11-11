import cv2  # ไลบรารีสำหรับการประมวลผลภาพ
from yolo_predictions import YOLO_Pred  # โมดูลสำหรับทำการคาดการณ์ด้วย YOLO
from parinya import LINE  # โมดูลสำหรับส่งข้อความผ่าน LINE
import socket  # ไลบรารีสำหรับการเชื่อมต่อ socket
import time  # ไลบรารีสำหรับจัดการเวลา

# กำหนดค่า IP และ port ของ  Arduino
arduino_ip = "192.168.164.174"
arduino_port = 8888
# สร้าง socket สำหรับการเชื่อมต่อ
arduino_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
arduino_socket.connect((arduino_ip, arduino_port))

# โหลดไฟล์โมเดลและการตั้งค่าสำหรับ YOLO
yolo = YOLO_Pred('my_obj.onnx', 'my_obj.yaml')

# เซต token สำหรับส่งข้อความผ่าน LINE
line = LINE('My Line Token')

# เปิดการทำงานของกล้อง
cap = cv2.VideoCapture(0)
# ส่งข้อความแจ้งเตือนเมื่อเปิดใช้งานเครื่อง
line.sendtext('เปิดใช้งานเครื่อง')

# ตัวแปรสำหรับจัดการเวลา
last_arduino_send_time = time.perf_counter()
last_line_send_time = time.perf_counter()

while True:
    ret, img = cap.read()  # อ่านภาพจากกล้อง
    img = cv2.flip(img, -1)  # พลิกภาพ

    key = cv2.waitKey(1)
    if (key == ord('q')) or (ret == False):  # ถ้ากด 'q' หรือไม่สามารถอ่านภาพได้
        break

    pred_image, obj_box = yolo.predictions(img)  # ทำการคาดการณ์ด้วย YOLO

    # วาดเส้นแบ่งแนวตั้งและแนวนอน
    cv2.line(pred_image, (img.shape[1] // 2, 0), (img.shape[1] // 2, img.shape[0]), (255, 255, 0), 1)
    cv2.line(pred_image, (0, img.shape[0] // 2), (img.shape[1], img.shape[0] // 2), (255, 255, 0), 1)

    # คำนวนพิกัด x y
    height, width, _ = pred_image.shape
    center_x, center_y = width // 2, height // 2

    # วาดจุดเล็งเป้า
    cv2.circle(pred_image, (center_x, center_y), 5, (0, 255, 0), -2)

    cv2.imshow('pred_image', pred_image)  # แสดงภาพที่ทำการคาดการณ์

    if obj_box:  # ถ้ามีวัตถุที่ตรวจพบ
        current_time = time.perf_counter()

        # ส่งข้อมูลไปยัง Arduino ทุก 0.1 วินาที
        if current_time - last_arduino_send_time >= 0.1:
            for box in obj_box:
                x, y, *_ = box  # ดึงพิกัด x, y จากกล่องที่ตรวจพบ
                data = f"{x},{y}"  # รวมข้อมูลเป็นรูปแบบ "x,y"
                print(f'Send x: {x}, y: {y}')  # แสดงพิกัดใน Python
                arduino_socket.send(data.encode('utf-8'))  # ส่งข้อมูลไปยัง Arduino
                last_arduino_send_time = current_time

        # ส่งภาพไปยัง LINE ทุก 30 วินาที
        if current_time - last_line_send_time >= 10:
            line.sendimage(img[:, :, ::-1])  # ส่งภาพไปยัง LINE
            last_line_send_time = current_time

cv2.destroyAllWindows()  # ปิดหน้าต่างแสดงภาพ
cap.release()  # ปิดการทำงานของกล้อง
arduino_socket.close()  # ปิดการเชื่อมต่อกับ Arduino
