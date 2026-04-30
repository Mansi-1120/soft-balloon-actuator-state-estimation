🧠 Soft Balloon Actuator State Estimation

📌 Overview
This project focuses on estimating the deformation (tip displacement) of a soft balloon actuator (SBA) using a data-driven approach.
Instead of relying on unstable pressure sensing, the model predicts deformation using:
- Input Volume
- Flow Rate

This work is part of an ongoing research project on an octopus-inspired soft manipulator.

📄 Reference presentation: ME740_Presentation (1).pdf

🎯 Objective
- Estimate actuator deformation (tip displacement)
- Build a functional relationship: **Deformation = f(Volume, Flow Rate)**
- Replace unreliable pressure sensing with a stable predictive model

⚙️ System Overview

**Soft Balloon Actuator (SBA)**
Fabricated using a layered structure:
- TPU → sealing layer
- Teflon circles → air chambers
- Parchment paper → layer interface
- Nylon → reinforcement

Heat press bonding process used for assembly.

**Electronics & Control**
- Arduino Nano (multi-channel PCB)
- H-Bridge MOSFET for EPM valve control
- Syringe pump for fluid input
- Pressure sensor (tested but not used in final model)

📷 Vision-Based Measurement

**Camera Setup**
- Fixed front-view camera
- Custom 3D-printed mounting brackets
- Laser-cut platform for actuator stability
- 3D-printed red circular marker attached to actuator tip
- Controlled lighting to reduce noise

**Marker Tracking (OpenCV)**
- Marker detected using color segmentation
- Pixel-to-mm calibration: 1 px ≈ 0.1 mm
- Tip displacement extracted from video frames

📊 Data Collection
Experiments conducted across:
- **Volumes**: 5, 10, 20, 30 ml
- **Flow rates**: 5–35 ml/min

**Measured**:
- Tip displacement
- Inflation time
- Pressure (discarded due to noise)

⚠️ Why Pressure Was Not Used
- High noise in readings
- Poor correlation with deformation
- Inconsistent across trials

👉 Volume and flow rate showed better stability and predictability

🧹 Data Processing
Raw CSV files cleaned and merged
Final dataset created: `data collection/processed/final_dataset.csv`

🤖 Machine Learning Model

**Model Used**: Polynomial Regression

**Why Polynomial?**
- Captures nonlinear actuator behavior
- Better fit than linear models
- Matches physical deformation characteristics

📈 Results

**Prediction Performance**
- Strong agreement between predicted and actual values
- Most points lie close to ideal line
- Slight deviation at higher deformation

**Deformation Trends**
- Nonlinear relationship with flow rate
- Higher flow → faster response
- Different volumes → different deformation patterns

**Flow Rate vs Time**
- Inflation time decreases with flow rate
- Higher volume → longer inflation time
- Nonlinear trend
- Saturation at high flow rates

**Combined Volume Analysis**
- Consistent behavior across all volumes
- Repeatable system response
- Confirms stable input-output relationship

📂 Project Structure

```
soft balloon actuator state estimation/
├── LICENSE
├── README.md
├── .gitignore
│
├── cad for laser cutting & 3d printing/
│   ├── Bottom plate for AL.ai
│   ├── Bottom plate for AL.DXF
│   ├── Bottom plate for AL.SLDDRW
│   ├── Bottom plate for AL.SLDPRT
│   ├── camera_mount_camra.SLDPRT
│   ├── GoPro Camera Side Screw Adapter.SLDPRT
│   ├── GoPro Flat Mount Camera Side.SLDPRT
│   ├── GoPro Flat Mount Tripod Side.SLDPRT
│   ├── keeeper.SLDPRT
│   ├── Stand.SLDPRT
│   ├── Table.ai
│   ├── Table.DXF
│   ├── Table.SLDDRW
│   └── Table.SLDPRT
│
├── data collection/
│   ├── processed/
│   │   └── final_dataset.csv
│   │
│   ├── raw/
│   │   ├── 1 chamber/
│   │   │   ├── final_dataset.csv
│   │   │   ├── 5 ml/CSV/
│   │   │   ├── 10ml/CSV/
│   │   │   ├── 20 ml/CSV/
│   │   │   └── 30 ml/CSV/
│   │   │
│   │   ├── 2 chamber/
│   │   │   └── 20 ml/CSV/
│   │   │
│   │   └── pessure sensing noisy data.png
│   │
│   └── videos/
│       ├── 1 chamber/
│       │   ├── 5 ml/
│       │   ├── 10 ml/
│       │   ├── 20 ml/
│       │   └── 30 ml/
│       │
│       └── 2 chamber/
│           └── 20 ml/
│
├── results/
│   ├── Flow rate vs time/
│   │   ├── 5ml.png
│   │   ├── 10 ml.png
│   │   ├── 20 ml.png
│   │   ├── 30 ml.png
│   │   └── Combine volume.png
│   │
│   └── ml model performance/
│       ├── Actual vs Predicted.png
│       ├── Deformation vs Flow Rate.png
│       └── Deformation vs Volume.png
│
├── src/
│   ├── arduino/
│   │   ├── epm control/
│   │   │   └── Multiple Epm Control.ino
│   │   │
│   │   └── pressure sensing/
│   │       └── pressure sensing.ino
│   │
│   ├── data processing/
│   │   └── Clean_CSV.py
│   │
│   ├── ml model/
│   │   └── Polynomial_Regression.py
│   │
│   ├── vision/
│   │   └── marker_tracking.py
│   │
│   └── visualization/
│       ├── all volume combine.py
│       ├── flow rate vs time- 20 ml.py
│       ├── flow rate vs time- 30 ml.py
│       ├── Flow Rate vs Time- 5 ml.py
│       └── flow rate vs time-10 ml.py
│
└── stl files for 3d printing/
    ├── Board Keeper.STL
    ├── camera_mount_camra.3MF
    ├── GoPro Camera Side Screw Adapter.STL
    ├── GoPro Flat Mount Tripod Side.3MF
    ├── marker2.STL
    └── Stand.STL
```

🚀 How to Run

**Install Dependencies**
```bash
pip install numpy pandas matplotlib opencv-python scikit-learn
```

**Run Marker Tracking**
```bash
python src/vision/marker_tracking.py
```

**Clean Data**
```bash
python src/data processing/Clean_CSV.py
```

**Train Model**
```bash
python src/ml model/Polynomial_Regression.py
```

**Generate Plots**
```bash
python src/visualization/all volume combine.py
```

🧠 Key Insight
Soft actuators exhibit nonlinear dynamic behavior, and deformation can be effectively estimated using volume and flow rate, eliminating the need for complex sensing.

🔬 Future Work
- Real-time control integration
- Closed-loop feedback system
- Multi-chamber modeling
- Advanced ML models

👩‍💻 Author
Mansi Singh  
MS Robotics | AI | Manipulation Robotics

📜 License
This project is licensed under the MIT License.
