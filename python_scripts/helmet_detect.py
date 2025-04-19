import sys
import json
import cv2
from ultralytics import YOLO

def detect_helmets(image_path, weights_path):
    model = YOLO(weights_path)
    img = cv2.imread(image_path)
    results = model(img)[0]

    detections = []
    for box, conf in zip(results.boxes.xyxy, results.boxes.conf):
        x1, y1, x2, y2 = map(int, box.tolist())
        detections.append({
            'bbox': [x1, y1, x2, y2],
            'confidence': float(conf)
        })

    return detections

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("[]")
        sys.exit(1)

    img_path = sys.argv[1]
    weights_path = sys.argv[2]
    dets = detect_helmets(img_path, weights_path)
    print(json.dumps(dets))