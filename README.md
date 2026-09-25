# 🧠 Soft Balloon Actuator State Estimation

<div align="center">

  **Estimating soft actuator deformation from volume and flow rate, without pressure sensing**

  [![ME740](https://img.shields.io/badge/Course-ME740-blue)]()
  [![Python](https://img.shields.io/badge/Python-3.8+-green)]()
  [![License](https://img.shields.io/badge/License-MIT-yellow)](LICENSE)

</div>

This project estimates the deformation (tip displacement) of a fiber-reinforced soft balloon actuator (SBA) using only the two inputs the actuation system controls directly: **injected volume** and **flow rate**. Ground-truth deformation comes from a vision-based marker tracking setup, and a third-order polynomial regression maps (volume, flow rate) to tip displacement.

It is part of the thesis *"Integrated Design, Control and Perception Based Grasping of a Bio-inspired Underwater Soft Manipulator"* in the Morphable Biorobotics Lab at Boston University, under Prof. Tommaso Ranzani. The full manipulator combines a cage-based gripper, a suction-based attachment mechanism, and the soft balloon actuator; this repository covers the SBA subsystem only.

---

## 🧩 Problem statement

**The problem.** A rigid robot joint has an encoder, so its state is always known. A soft balloon actuator deforms continuously and has no discrete states, so there is no direct way to measure its configuration during operation.

**Why pressure is not enough.** Pressure looks like the natural signal, since internal pressure drives the expansion. In our experiments, however, pressure readings were noisy, drifted, and did not match the observed deformation from trial to trial. Work from the same lab (Lee et al., 2025) explains why: pressure is a coupled quantity,

```
P ≈ f(V) + g(Q)
```

so the same pressure can correspond to different shapes depending on how much fluid was injected (V) and how fast (Q). Material compliance and fabrication variability (Rogatinsky et al., 2022) weaken the pressure to shape link even further.

**Research question.** Can the actuator's deformation be estimated reliably from the controllable inputs, volume and flow rate, instead of from pressure?

**What we do.**

1. Fabricate a fiber-reinforced SBA and drive it with an EPM valve system and a syringe pump.
2. Measure ground-truth deformation with a fixed camera tracking red markers on the actuator.
3. Collect data across 4 input volumes (5, 10, 20, 30 ml) and flow rates from 5 to 35 ml/min.
4. Fit a third-order polynomial regression, `tip displacement = f(V, Q)`, and compare its predictions with the measured deformation.

---

## 📌 Headline result

<table>
  <tr>
    <th>Inflation time vs flow rate, all volumes</th>
    <th>Predicted vs measured tip displacement</th>
  </tr>
  <tr>
    <td><img src="results/Flow%20rate%20vs%20time/Combine%20volume.png" width="440"></td>
    <td><img src="results/ml%20model%20performance/Actual%20vs%20Predicted.png" width="360"></td>
  </tr>
  <tr>
    <td>Inflation time drops as flow rate rises and grows with injected volume. The same curve shape repeats for every volume. Report Figure 4(e).</td>
    <td>Most points lie close to the ideal line (red dashed); deviations grow at larger deformations. Report Figure 7(a).</td>
  </tr>
</table>

Volume and flow rate alone are enough to estimate actuator state, so the unreliable pressure sensor can be removed from the loop.

---

## 📊 Results

All plots below are taken directly from `results/` and are the same images used in the project report.

### Flow rate and volume behavior

<table>
  <tr>
    <th>5 ml</th>
    <th>10 ml</th>
  </tr>
  <tr>
    <td><img src="results/Flow%20rate%20vs%20time/5ml.png" width="400"></td>
    <td><img src="results/Flow%20rate%20vs%20time/10%20ml.png" width="400"></td>
  </tr>
  <tr>
    <th>20 ml</th>
    <th>30 ml</th>
  </tr>
  <tr>
    <td><img src="results/Flow%20rate%20vs%20time/20%20ml.png" width="400"></td>
    <td><img src="results/Flow%20rate%20vs%20time/30%20ml.png" width="400"></td>
  </tr>
</table>

* **Inverse relationship.** For every volume, a higher flow rate means a shorter inflation time.
* **Volume scaling.** At the same flow rate, a larger volume takes longer. At 5 ml/min, inflation takes about 60 s for 5 ml and about 353 s for 30 ml.
* **Nonlinear with saturation.** At low flow rates, a small increase in flow cuts inflation time sharply. At high flow rates the gains flatten out, which suggests the response is also limited by chamber expansion and material compliance, not only by input flow.
* **Repeatable.** The absolute times change with volume, but the shape of the curve stays the same across all four volumes.

Inflation times come from the Harvard syringe pump and are entered in the plotting scripts in `src/visualization/`.

### Model performance

<table>
  <tr>
    <th>Predicted vs actual</th>
    <th>Deformation vs flow rate</th>
    <th>Deformation vs volume</th>
  </tr>
  <tr>
    <td><img src="results/ml%20model%20performance/Actual%20vs%20Predicted.png" width="280"></td>
    <td><img src="results/ml%20model%20performance/Deformation%20vs%20Flow%20Rate.png" width="280"></td>
    <td><img src="results/ml%20model%20performance/Deformation%20vs%20Volume.png" width="280"></td>
  </tr>
</table>

* The third-order polynomial captures the nonlinear relationship between (volume, flow rate) and tip displacement without adding much model complexity.
* Predictions follow the measured values closely. Larger deviations appear at higher deformations, where material variability and large-expansion nonlinearities matter more.
* The first two plots are report Figure 7(a) and 7(b). The deformation vs volume plot is an extra view from the same script.
* Note: the model is fit and evaluated on the full 26-point dataset in `data collection/processed/final_dataset.csv` (no held-out split), so the plots show how well the model fits the collected data.

---

## ⚙️ System

### 🧱 Actuator fabrication

The actuator is a fiber-reinforced soft balloon actuator, fabricated following Vo et al. (2025).

| Layer | Role |
|-------|------|
| TPU | Primary elastic material and airtight sealing |
| TPU-coated nylon (nylon taffeta rings) | Directional stiffness and sealing |
| Hexagonal mesh embedded in TPU | Spreads stress and limits uncontrolled expansion, for more repeatable behavior |
| Teflon spacers | Define the internal cavity and prevent unwanted bonding |

* TPU and mesh are heat pressed into composite sheets, then laser cut into circular inserts.
* The mesh side of each composite faces the Teflon interface for good adhesion and no leaks. Several TPU layers are used because the material thins during pressing.
* Two-stage heat pressing: sub-stacks at 270 °F and 150 lbs for 6 minutes to set alignment, then the full stack for 30 minutes for uniform bonding.

### 🔌 Control and sensing

```
 Host PC (logging + keyboard commands)
   │                                   │
   ▼                                   ▼
 Arduino Nano Every                  Arduino Uno
 EPM valve control                   pressure sensing only
   │                                   │  I2C
   ▼                                   ▼
 Custom H-bridge MOSFET              MPRLS pressure sensor
 driver PCB (25 V)                   (in the fluid line)
   │
   ▼
 EPM valves ──► fluid into the SBA ◄── syringe pump
```

* **Electropermanent magnet (EPM) valves** (McDonald and Ranzani, 2021). A short current pulse switches the magnet, and it holds its state without continuous power, giving discrete, repeatable, energy-efficient valve control.
* **Two microcontrollers.** All I/O pins on the Nano Every are used by the EPM driver board, so pressure sensing runs on a separate Arduino Uno. This keeps valve timing reliable and sensor communication stable.
* Arduino code is in `src/arduino/`: `epm control/Multiple Epm Control.ino` and `pressure sensing/pressure sensing.ino`.

### 📷 Vision-based ground truth

* **Markers.** The actuator is black, so red markers give strong contrast for HSV segmentation in OpenCV. `src/vision/marker_tracking.py` tracks base, mid and tip markers and logs tip displacement (dx, dy) from the starting tip position.
* **Fixed viewpoint.** The camera and light are held by 3D-printed brackets on an aluminum bar, and the actuator sits in laser-cut acrylic and 3D-printed fixtures that keep it aligned with the camera plane. This limits perspective distortion for 2D tracking.
* **Calibration.** A known reference in the scene gives **255 px ≈ 31 mm** (about 8.22 px/mm), used to convert pixel displacement to millimetres.
* **Curvature.** Curvature is also estimated with a piecewise constant curvature (PCC) model, but it was too variable and is not used in the final analysis.

CAD for the fixtures is in `cad for laser cutting & 3d printing/`, and print files are in `stl files for 3d printing/`.

---

## 📋 Data collection

| Parameter | Values |
|-----------|--------|
| Input volume | 5, 10, 20, 30 ml |
| Flow rate | 5 to 35 ml/min |
| Configuration | 1 chamber (used for the model); 2-chamber runs at 20 ml are also recorded |

* Every trial is recorded as a CSV (time, pressure, marker positions, tip displacement, curvature) and an MP4 video, in `data collection/raw/` and `data collection/videos/`.
* `src/data processing/Clean_CSV.py` takes the maximum tip displacement of each 1-chamber trial and builds `data collection/processed/final_dataset.csv` (volume, flow rate, tip displacement). The 10 ml, 5 ml/min point (0.44 mm) was added manually from its video.
* Inflation time is read from the Harvard syringe pump.

### ⚠️ Why pressure was dropped

<p align="center">
  <img src="data%20collection/raw/pessure%20sensing%20noisy%20data.png" width="520">
  <br>
  <em>Logged pressure per trial. Readings jump between trials and do not follow tip displacement.</em>
</p>

Pressure was collected at the start of the project, but it was noisy, drifted, and was inconsistent across repeated trials. Because pressure depends on both volume and flow rate, it cannot identify the actuator state on its own, so it was removed from the model.

---

## 🤖 Modeling approach

A simple linear model could not capture the nonlinear response, so a third-order polynomial regression is used:

```
y = f(V, Q) = Σ a_ij · V^i · Q^j ,   i + j ≤ 3
```

where `y` is tip displacement, `V` is volume, `Q` is flow rate, and `a_ij` are learned coefficients. This keeps the model interpretable while allowing interaction between volume and flow rate. Implementation: `src/ml model/Polynomial_Regression.py` (scikit-learn `PolynomialFeatures(degree=3)` + `LinearRegression`).

---

## 📂 Project structure

```
soft-balloon-actuator-state-estimation/
│
├── cad for laser cutting & 3d printing/   # SolidWorks parts/drawings, DXF and AI files for fixtures and camera mounts
├── stl files for 3d printing/             # STL / 3MF print files (camera mount, GoPro mounts, stand, marker, board keeper)
│
├── data collection/
│   ├── raw/
│   │   ├── 1 chamber/<volume>/CSV/        # per-trial logs, 5 to 35 ml/min
│   │   ├── 2 chamber/20 ml/CSV/           # 2-chamber trials (not used in the model)
│   │   └── pessure sensing noisy data.png
│   ├── processed/final_dataset.csv        # volume, flow rate, max tip displacement (model input)
│   └── videos/                            # recorded trials (MP4)
│
├── results/
│   ├── Flow rate vs time/                 # inflation time vs flow rate, per volume and combined
│   └── ml model performance/              # predicted vs actual, deformation vs flow rate / volume
│
├── src/
│   ├── arduino/                           # EPM valve control and MPRLS pressure sensing
│   ├── vision/marker_tracking.py          # live HSV marker tracking + recording
│   ├── data processing/Clean_CSV.py       # builds final_dataset.csv
│   ├── ml model/Polynomial_Regression.py  # model fit + result plots
│   └── visualization/                     # inflation time plots
│
├── LICENSE
└── README.md
```

---

## 🚀 Running the code

```bash
pip install numpy pandas matplotlib seaborn opencv-python scikit-learn pyserial
```

The scripts were written for the lab setup, so file paths (Windows drive paths) and serial ports (`COM10`, `COM11`) are hard-coded near the top of each script. Update them before running.

1. **Record trials (needs the hardware).** `src/vision/marker_tracking.py` opens the camera and both serial ports. Keys `1` to `6` open and close the EPMs, `s` / `e` start and stop recording (CSV + MP4), `r` resets the tip reference, `q` quits.
2. **Build the dataset.**
   ```bash
   python "src/data processing/Clean_CSV.py"
   ```
3. **Fit the model and plot results.**
   ```bash
   python "src/ml model/Polynomial_Regression.py"
   ```
4. **Inflation time plots.**
   ```bash
   python "src/visualization/all volume combine.py"
   ```

---

## 🔬 Future work

* **Closed-loop control.** Combine vision feedback with the input-driven model to regulate the actuator online.
* **Cleaner sensing.** A custom PCB housing is being developed to reduce electrical noise and stabilize the pressure signal.
* **Better fluid control.** Replace the single-channel syringe pump with a multi-channel system for coordinated control.
* **3D perception.** Move from 2D tracking to a multi-camera 3D setup for full spatial reconstruction of the deformation.

---

## 📚 Key references

* Lee, Zamora Yáñez, Rogatinsky, et al., 2025. Simplifying data-driven modeling of the volume-flow-pressure relationship in hydraulic soft robotic actuators. arXiv:2506.23326.
* Vo, Zamora Yáñez, Muter, Moran, Saxena, Matthews, and Ranzani, 2025. Soft, fiber-reinforced bellow actuators. IEEE RA-L.
* McDonald and Ranzani, 2021. Hardware methods for onboard control of fluidically actuated soft robots. Frontiers in Robotics and AI.
* Rogatinsky et al., 2022. A collapsible soft actuator facilitates performance in constrained environments. Advanced Intelligent Systems.
* Werner, Hofer, Sferrazza, and D'Andrea, 2019. Vision-based proprioceptive sensing for soft inflatable actuators. arXiv:1909.09096.

---

## 👩‍💻 Author

**Mansi Singh** <br>
M.S. Robotics and Autonomous Systems, Boston University <br>
Morphable Biorobotics Lab (Prof. Tommaso Ranzani) <br>
🔗 GitHub: https://github.com/Mansi-1120

## 📜 License

MIT License. See [LICENSE](LICENSE).
