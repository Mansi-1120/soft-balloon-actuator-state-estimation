import pandas as pd
import os
import re

root_folder = r"U:\Mansi Singh\State Estimation_camera_mounting\results\1 chamber"

final_data = []

for volume_folder in os.listdir(root_folder):

    volume_path = os.path.join(root_folder, volume_folder)

    if not os.path.isdir(volume_path):
        continue

    # Extract volume (works for "5 ml" and "10ml")
    volume = int(re.search(r'\d+', volume_folder).group())

    csv_folder = os.path.join(volume_path, "CSV")

    if not os.path.exists(csv_folder):
        continue

    for file in os.listdir(csv_folder):

        if not file.endswith(".csv"):
            continue

        # Extract flow rate
        flow_rate = int(re.search(r'\d+', file).group())

        file_path = os.path.join(csv_folder, file)

        try:
            df = pd.read_csv(file_path)

            # KEEP ONLY TIP COLUMN
            df = df[["tip_disp_mm"]].dropna()

            if len(df) < 5:
                continue

            # TAKE MAX TIP (very important)
            tip = df["tip_disp_mm"].max()

            final_data.append([volume, flow_rate, tip])

            print(f"Done V={volume}, F={flow_rate}")

        except Exception as e:
            print(f"Error in {file}: {e}")

# -----------------------------
# CREATE DATASET
# -----------------------------
final_df = pd.DataFrame(final_data, columns=[
    "Volume_ml",
    "FlowRate_ml_min",
    "Tip_mm"
])

# -----------------------------
# ADD VIDEO DATA (MANUAL)
# -----------------------------
final_df.loc[len(final_df)] = [10, 5, 0.44]

# -----------------------------
# SORT
# -----------------------------
final_df = final_df.sort_values(by=["Volume_ml", "FlowRate_ml_min"])

# -----------------------------
# SAVE
# -----------------------------
save_path = os.path.join(root_folder, "final_dataset.csv")
final_df.to_csv(save_path, index=False)

print("\nFINAL DATASET READY:\n")
print(final_df)