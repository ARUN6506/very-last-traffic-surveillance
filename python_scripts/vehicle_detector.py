# vehicle_detector.py
import torch
import sys
import cv2

# Load model (make sure the correct path is provided)
model = torch.hub.load('ultralytics/yolov5', 'custom', path='models/Vehicle-Detection-main/runs/train/exp12/weights/best.pt', force_reload=False)

image_path = sys.argv[1]
img = cv2.imread(image_path)
results = model(img)

for *xyxy, conf, cls in results.xyxy[0]:
    x1, y1, x2, y2 = map(int, xyxy)
    w, h = x2 - x1, y2 - y1
    print(f"{x1} {y1} {w} {h} {int(cls)} {float(conf):.2f}")