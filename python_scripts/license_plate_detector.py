# python_scripts/license_plate_detector.py

import sys
import easyocr

def detect_plate(image_path):
    reader = easyocr.Reader(['en'])
    results = reader.readtext(image_path)

    # Try to find the most plate-like text (longest alphanumeric string)
    best_plate = ""
    for (bbox, text, confidence) in results:
        plate_candidate = ''.join(char for char in text if char.isalnum())
        if len(plate_candidate) > len(best_plate):
            best_plate = plate_candidate

    return best_plate

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python license_plate_detector.py image.jpg", file=sys.stderr)
        sys.exit(1)

    image_path = sys.argv[1]
    plate = detect_plate(image_path)
    print(plate)