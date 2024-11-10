import cv2
from yolo_predictions import YOLO_Pred
from parinya import LINE
import socket
import time

# เกี่ยวกับ wifi
arduino_ip = "192.168.164.174"
arduino_port = 8888
arduino_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
arduino_socket.connect((arduino_ip, arduino_port))

# ไฟล์อ้างอิงในการ detect
yolo = YOLO_Pred('my_obj.onnx', 'my_obj.yaml')

# เซต token line
line = LINE('My line Token')

cap = cv2.VideoCapture(0)  # ภาพจากกล้อง
line.sendtext('เปิดใช้งานเครื่อง')

# ตัวแปรสำหรับจัดการเวลา
last_arduino_send_time = time.perf_counter()
last_line_send_time = time.perf_counter()

while True:
    ret, img = cap.read()
    img = cv2.flip(img, -1)

    key = cv2.waitKey(1)
    if (key == ord('q')) or (ret == False):
        break

    pred_image, obj_box = yolo.predictions(img)

    cv2.line(pred_image, (img.shape[1] // 2, 0), (img.shape[1] // 2, img.shape[0]), (255, 255, 0), 1)  # เส้นแบ่งแนวตั้ง
    cv2.line(pred_image, (0, img.shape[0] // 2), (img.shape[1], img.shape[0] // 2), (255, 255, 0), 1)  # เส้นแบ่งแนวนอน

    # คำนวนพิกัด x y
    height, width, _ = pred_image.shape
    center_x, center_y = width // 2, height // 2

    # จุดเล็งเป้า
    cv2.circle(pred_image, (center_x, center_y), 5, (0, 255, 0), -2)

    cv2.imshow('pred_image', pred_image)

    if obj_box:
        current_time = time.perf_counter()

        # ส่งข้อมูลไปยัง Arduino ทุก 0.1 วินาที
        if current_time - last_arduino_send_time >= 0.1:
            for box in obj_box:
                x, y, *_ = box  # Extract x coordinate
                data = f"{x},{y}"  # รวมข้อมูลเป็นรูปแบบ "x,y"
                print(f'Send x: {x}, y: {y}')  # Print พิกัด ใน py
                arduino_socket.send(data.encode('utf-8'))  # ส่งข้อมูลเป็น bytes โดยใช้ encode()
                last_arduino_send_time = current_time

        # ส่งภาพไปยัง LINE ทุก 30 วินาที
        if current_time - last_line_send_time >= 10:
            line.sendimage(img[:, :, ::-1])
            last_line_send_time = current_time

cv2.destroyAllWindows()
cap.release()
arduino_socket.close()
