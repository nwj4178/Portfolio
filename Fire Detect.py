from ultralytics import YOLO
import cv2
import time
import winsound
import os

model = YOLO(r'Fire_Detect.pt')
cap   = cv2.VideoCapture('fire_AI_5.mp4')

fire_start           = None
fire_alarm_triggered = False
FIRE_DELAY           = 3.0
ALARM_SOUND          = os.path.join(os.path.dirname(__file__), 'alert.wav')

while True:
    ret, frame = cap.read()
    if not ret:
        break

    results      = model(frame, conf=0.1, imgsz=640)[0]
    fire_detected = any(int(cls) == 0 for cls in results.boxes.cls)

    now = time.time()
    if fire_detected:
        if fire_start is None:
            fire_start = now
            fire_alarm_triggered = False
        elif not fire_alarm_triggered and (now - fire_start) >= FIRE_DELAY:
            winsound.PlaySound(
                ALARM_SOUND,
                winsound.SND_FILENAME | winsound.SND_ASYNC
            )
            fire_alarm_triggered = True
    else:
        fire_start = None
        fire_alarm_triggered = False

    annotated = results.plot()
    cv2.imshow("Fire Detection", annotated)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()