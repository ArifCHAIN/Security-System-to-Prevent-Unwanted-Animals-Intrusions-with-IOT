import cv2
from yolo_predictions import YOLO_Pred
import socket
import time

# Arduino Wi-Fi information
arduino_ip = "192.168.164.174"
arduino_port = 8888
arduino_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
arduino_socket.connect((arduino_ip, arduino_port))

yolo = YOLO_Pred('my_obj.onnx', 'my_obj.yaml')  # YOLO Model file

cap = cv2.VideoCapture(0)
#cap.set(3, 800)  # window width
#cap.set(4, 600)  # window height

while True:
    ret, img = cap.read()
    img = cv2.flip(img, -1)

    key = cv2.waitKey(1)
    if (key == ord('q')) or (ret == False):
        break

    pred_image, obj_box = yolo.predictions(img)


    cv2.line(pred_image, (img.shape[1] // 2, 0), (img.shape[1] // 2, img.shape[0]), (255, 255, 0), 1)  # Draw vertical line
    cv2.line(pred_image, (0, img.shape[0] // 2), (img.shape[1], img.shape[0] // 2), (255, 255, 0), 1)  # Draw horizontal line

    # Calculate center position (x, y) of the screen
    height, width, _ = pred_image.shape
    center_x, center_y = width // 2, height // 2

    # Draw circle at (center_x, center_y)
    cv2.circle(pred_image, (center_x, center_y), 5, (0, 255, 0), -2)

    cv2.imshow('pred_image', pred_image)

    if obj_box:
        
        for box in obj_box:  # Loop through all detected objects
            x, y, *_ = box  # Extract x coordinate
            cv2.putText(pred_image, f'x: {x}, y: {y}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            print(f'Send x: {x}, y: {y}')  # Print coordinates to Terminal
            data = f"{x},{y}"  # รวมข้อมูลเป็นรูปแบบ "x,y"
            arduino_socket.send(data.encode('utf-8'))# ส่งข้อมูลเป็น bytes โดยใช้ encode()
            #arduino_socket.send(f"{dataX},{dataY}".encode('utf-8'))
            #arduino_socket.send(f"x:{dataX},y:{dataY}".encode('utf-8'))


            time.sleep(0.1)

cv2.destroyAllWindows()
cap.release()
