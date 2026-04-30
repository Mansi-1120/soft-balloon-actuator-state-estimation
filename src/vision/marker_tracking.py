import cv2
import numpy as np
import serial
import time
import csv
import os
import re

# --------------------------------------------------
# Serial ports
# --------------------------------------------------
pressure_ser = serial.Serial('COM11', 9600, timeout=0.05)
epm_ser = serial.Serial('COM10', 9600, timeout=0.05)

time.sleep(2)
pressure_ser.reset_input_buffer()

# --------------------------------------------------
# Camera setup
# --------------------------------------------------
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

cv2.namedWindow("Tracking", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Tracking", 640, 480)

cv2.namedWindow("Mask", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Mask", 640, 480)

# --------------------------------------------------
# Save folder
# --------------------------------------------------
save_folder = r"U:\Mansi Singh\State Estimation_camera_mounting\results\2 chamber\20 ml"

if not os.path.exists(save_folder):
    os.makedirs(save_folder)

# --------------------------------------------------
# Recording variables
# --------------------------------------------------
recording = False
csv_file = None
writer = None
video_writer = None

# --------------------------------------------------
# Tracking variables
# --------------------------------------------------
tip_ref = None

base_smooth = None
mid_smooth = None
tip_smooth = None

# --------------------------------------------------
# Pressure variables
# --------------------------------------------------
pressure_zero_offset = None
pressure_filtered = 0.0

PRESSURE_SIGN = 1.0            # change to -1.0 if sign is reversed
RAW_PRESSURE_LIMIT = 120.0     # raw Arduino value limit
DISPLAY_PRESSURE_LIMIT = 20.0  # pressure shown on screen max

# --------------------------------------------------
# Scale
# --------------------------------------------------
PX_PER_MM = 8.22

# --------------------------------------------------
# HSV range for red markers
# --------------------------------------------------
red_low1 = np.array([0, 80, 80])
red_high1 = np.array([15, 255, 255])

red_low2 = np.array([160, 80, 80])
red_high2 = np.array([179, 255, 255])

kernel = np.ones((5, 5), np.uint8)

# --------------------------------------------------
# Parse numeric value from serial line
# --------------------------------------------------
def parse_pressure_value(line):
    m = re.search(r'[-+]?\d*\.?\d+', line)

    if m:
        try:
            return float(m.group())
        except:
            return None

    return None

# --------------------------------------------------
# Marker detection
# --------------------------------------------------
def get_three_markers(mask):

    contours, _ = cv2.findContours(
        mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
    )

    points = []

    for c in contours:

        area = cv2.contourArea(c)

        if area < 80 or area > 3000:
            continue

        M = cv2.moments(c)

        if M["m00"] == 0:
            continue

        x = int(M["m10"] / M["m00"])
        y = int(M["m01"] / M["m00"])

        points.append(np.array([x, y], dtype=np.float32))

    if len(points) < 3:
        return None, None, None

    points = sorted(points, key=lambda p: p[1], reverse=True)

    return points[0], points[1], points[2]

# --------------------------------------------------
# Curvature calculation
# --------------------------------------------------
def get_curvature(base, mid, tip):

    a = np.linalg.norm(mid - base)
    b = np.linalg.norm(tip - mid)
    c = np.linalg.norm(tip - base)

    area = 0.5 * abs(
        base[0] * (mid[1] - tip[1]) +
        mid[0] * (tip[1] - base[1]) +
        tip[0] * (base[1] - mid[1])
    )

    if area < 1 or a == 0 or b == 0 or c == 0:
        return 0.0

    curvature_px = 4 * area / (a * b * c)

    return curvature_px * PX_PER_MM

# --------------------------------------------------
# Instructions
# --------------------------------------------------
print("1 -> EPM1 OPEN")
print("2 -> EPM1 CLOSE")
print("3 -> EPM2 OPEN")
print("4 -> EPM2 CLOSE")
print("5 -> EPM3 OPEN")
print("6 -> EPM3 CLOSE")
print("r -> reset tip reference only")
print("s -> start recording")
print("e -> stop recording")
print("q -> quit")

# --------------------------------------------------
# Main loop
# --------------------------------------------------
while True:

    ret, frame = cap.read()

    if not ret:
        break

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # --------------------------------------------------
    # Marker mask
    # --------------------------------------------------
    mask1 = cv2.inRange(hsv, red_low1, red_high1)
    mask2 = cv2.inRange(hsv, red_low2, red_high2)

    mask = cv2.bitwise_or(mask1, mask2)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)

    base, mid, tip = get_three_markers(mask)

    # --------------------------------------------------
    # Smooth marker positions
    # --------------------------------------------------
    if base is not None:

        if base_smooth is None:
            base_smooth = base
            mid_smooth = mid
            tip_smooth = tip
        else:
            base_smooth = 0.85 * base_smooth + 0.15 * base
            mid_smooth = 0.85 * mid_smooth + 0.15 * mid
            tip_smooth = 0.85 * tip_smooth + 0.15 * tip

        base = base_smooth.astype(int)
        mid = mid_smooth.astype(int)
        tip = tip_smooth.astype(int)

    # --------------------------------------------------
    # Pressure reading
    # --------------------------------------------------
    try:
        while pressure_ser.in_waiting > 1:
            pressure_ser.readline()

        line = pressure_ser.readline().decode(errors='ignore').strip()

        val = parse_pressure_value(line)

        if val is not None and abs(val) < RAW_PRESSURE_LIMIT:

            # First value becomes atmosphere reference
            if pressure_zero_offset is None:
                pressure_zero_offset = val
                pressure_filtered = 0.0

            corrected_pressure = (val - pressure_zero_offset) * PRESSURE_SIGN

            # If your sensor gives negative during inflation,
            # change PRESSURE_SIGN above to -1.0
            if corrected_pressure < 0:
                corrected_pressure = 0.0

            # remove tiny noise
            if corrected_pressure < 0.2:
                corrected_pressure = 0.0

            # limit to 20 kPa maximum
            if corrected_pressure > DISPLAY_PRESSURE_LIMIT:
                corrected_pressure = DISPLAY_PRESSURE_LIMIT

            # limit sudden jump
            delta = corrected_pressure - pressure_filtered

            if delta > 3:
                corrected_pressure = pressure_filtered + 3
            elif delta < -3:
                corrected_pressure = pressure_filtered - 3

            # smoothing
            pressure_filtered = (
                0.3 * pressure_filtered +
                0.7 * corrected_pressure
            )

            # final clamp
            pressure_filtered = max(
                0.0,
                min(DISPLAY_PRESSURE_LIMIT, pressure_filtered)
            )

            # snap tiny values to zero
            if pressure_filtered < 0.2:
                pressure_filtered = 0.0

    except:
        pass

    # --------------------------------------------------
    # Tip displacement
    # --------------------------------------------------
    dx = 0
    dy = 0
    tip_disp_px = 0
    tip_disp_mm = 0

    if tip is not None:

        if tip_ref is None:
            tip_ref = tip.copy()

        dx = int(tip[0] - tip_ref[0])
        dy = int(tip[1] - tip_ref[1])

        if abs(dx) < 2:
            dx = 0

        if abs(dy) < 2:
            dy = 0

        tip_disp_px = np.sqrt(dx**2 + dy**2)
        tip_disp_mm = tip_disp_px / PX_PER_MM

    # --------------------------------------------------
    # Curvature
    # --------------------------------------------------
    curv_mm = 0

    if base is not None and mid is not None and tip is not None:
        curv_mm = get_curvature(
            base.astype(float),
            mid.astype(float),
            tip.astype(float)
        )

    # --------------------------------------------------
    # Draw markers and lines
    # --------------------------------------------------
    if base is not None:
        cv2.circle(frame, tuple(base), 10, (255, 0, 0), -1)
        cv2.putText(frame, "Base",
                    (base[0] + 10, base[1] - 10),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (255, 0, 0), 2)

    if mid is not None:
        cv2.circle(frame, tuple(mid), 10, (0, 255, 255), -1)
        cv2.putText(frame, "Mid",
                    (mid[0] + 10, mid[1] - 10),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (0, 255, 255), 2)

    if tip is not None:
        cv2.circle(frame, tuple(tip), 10, (255, 0, 255), -1)
        cv2.putText(frame, "Tip",
                    (tip[0] + 10, tip[1] - 10),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (255, 0, 255), 2)

        if tip_ref is not None:
            cv2.circle(frame, tuple(tip_ref.astype(int)), 5, (255, 255, 255), -1)
            cv2.line(frame,
                     tuple(tip_ref.astype(int)),
                     tuple(tip),
                     (255, 255, 255), 2)

    if base is not None and mid is not None:
        cv2.line(frame, tuple(base), tuple(mid), (0, 255, 0), 2)

    if mid is not None and tip is not None:
        cv2.line(frame, tuple(mid), tuple(tip), (0, 255, 0), 2)

    # --------------------------------------------------
    # Display text
    # --------------------------------------------------
    cv2.putText(frame,
                f"P = {pressure_filtered:.2f} kPa",
                (20, 40),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.9,
                (255, 255, 255),
                2)

    cv2.putText(frame,
                f"dx = {dx}, dy = {dy}",
                (20, 80),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.8,
                (100, 255, 100),
                2)

    cv2.putText(frame,
                f"Tip = {tip_disp_px:.1f} px ({tip_disp_mm:.2f} mm)",
                (20, 120),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.9,
                (0, 255, 255),
                2)

    cv2.putText(frame,
                f"Curvature = {curv_mm:.4f} 1/mm",
                (20, 160),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.9,
                (0, 200, 255),
                2)

    # --------------------------------------------------
    # Recording indicator
    # --------------------------------------------------
    if recording:
        cv2.circle(frame, (610, 25), 8, (0, 0, 255), -1)
        cv2.putText(frame, "REC",
                    (575, 30),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.5,
                    (0, 0, 255),
                    1)

    # --------------------------------------------------
    # Save data
    # --------------------------------------------------
    if recording and writer is not None:
        writer.writerow([
            time.time(),
            pressure_filtered,
            dx,
            dy,
            tip_disp_px,
            tip_disp_mm,
            curv_mm,
            base[0] if base is not None else None,
            base[1] if base is not None else None,
            mid[0] if mid is not None else None,
            mid[1] if mid is not None else None,
            tip[0] if tip is not None else None,
            tip[1] if tip is not None else None
        ])

    if recording and video_writer is not None:
        video_writer.write(frame)

    # --------------------------------------------------
    # Show windows
    # --------------------------------------------------
    cv2.imshow("Tracking", frame)
    cv2.imshow("Mask", mask)

    key = cv2.waitKey(1) & 0xFF

    # --------------------------------------------------
    # Commands
    # --------------------------------------------------
    if key == ord('1'):
        epm_ser.write(b'21B500\n')
        print("EPM1 OPEN")

    elif key == ord('2'):
        epm_ser.write(b'21A500\n')
        print("EPM1 CLOSE")

    elif key == ord('3'):
        epm_ser.write(b'23A500\n')
        print("EPM2 OPEN")

    elif key == ord('4'):
        epm_ser.write(b'23B500\n')
        print("EPM2 CLOSE")

    elif key == ord('5'):
        epm_ser.write(b'24A500\n')
        print("EPM3 OPEN")

    elif key == ord('6'):
        epm_ser.write(b'24B500\n')
        print("EPM3 CLOSE")

    elif key == ord('r'):
        tip_ref = None
        print("Tip reference reset")

    elif key == ord('s') and not recording:

        trial_name = os.path.join(
            save_folder,
            f"trial_{time.strftime('%Y%m%d_%H%M%S')}"
        )

        csv_file = open(trial_name + ".csv", "w", newline="")
        writer = csv.writer(csv_file)

        writer.writerow([
            "time",
            "pressure_kPa",
            "dx_px",
            "dy_px",
            "tip_disp_px",
            "tip_disp_mm",
            "curvature_1_per_mm",
            "base_x", "base_y",
            "mid_x", "mid_y",
            "tip_x", "tip_y"
        ])

        fourcc = cv2.VideoWriter_fourcc(*'mp4v')

        video_writer = cv2.VideoWriter(
            trial_name + ".mp4",
            fourcc,
            20,
            (frame.shape[1], frame.shape[0])
        )

        recording = True
        print("Recording started")

    elif key == ord('e') and recording:

        recording = False

        if csv_file is not None:
            csv_file.close()
            csv_file = None

        if video_writer is not None:
            video_writer.release()
            video_writer = None

        writer = None
        print("Recording stopped")

    elif key == ord('q'):
        break

# --------------------------------------------------
# Cleanup
# --------------------------------------------------
cap.release()

if csv_file is not None:
    csv_file.close()

if video_writer is not None:
    video_writer.release()

pressure_ser.close()
epm_ser.close()

cv2.destroyAllWindows()