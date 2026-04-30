# 🧠 Soft Balloon Actuator State Estimation

<div align="center">
  
  **Estimating Soft Actuator Deformation Using Data-Driven Approaches**
  
  [![ME740](https://img.shields.io/badge/Course-ME740-blue)]() 
  [![Python](https://img.shields.io/badge/Python-3.8+-green)]()
  [![License](https://img.shields.io/badge/License-MIT-yellow)]()

</div>

---

## 📌 Overview

This project focuses on **estimating the deformation (tip displacement)** of a soft balloon actuator (SBA) using a data-driven approach.

> **Key Innovation:** Instead of relying on unstable pressure sensing, the model predicts deformation using only:
> - 🔹 **Input Volume**
> - 🔹 **Flow Rate**

This work is part of an ongoing research project on an **octopus-inspired soft manipulator**, combining soft robotics with machine learning for practical state estimation.

📄 **Reference:** ME740_Presentation (1).pdf

---

## 🎯 Objective

| Goal | Description |
|------|-------------|
| 🎪 **Primary** | Estimate actuator deformation (tip displacement) |
| 📐 **Mathematical** | Build functional relationship: **Deformation = f(Volume, Flow Rate)** |
| 🔧 **Practical** | Replace unreliable pressure sensing with stable predictive model |

---

## ⚙️ System Overview

### 🧱 Soft Balloon Actuator (SBA)
Fabricated using a **layered composite structure**:

| Layer | Material | Purpose |
|-------|----------|---------|
| 🛡️ Sealing | TPU | Airtight enclosure |
| 💨 Chambers | Teflon circles | Air channel design |
| 🔗 Interface | Parchment paper | Layer separation |
| 💪 Reinforcement | Nylon | Structural support |

> Assembly: **Heat press bonding process**

### 🔌 Electronics & Control

```
┌─────────────────────────────────────┐
│   Arduino Nano (Multi-Channel PCB)  │
│   • EPM Valve Control (H-Bridge)    │
│   • Pressure Sensing (Optional)     │
│   • Real-time monitoring            │
└──────────┬──────────────────────────┘
           │
    ┌──────┴──────┬──────────┐
    │             │          │
  💧 Syringe   🔌 Valve   📊 Sensor
   Pump      (MOSFET)  (Unused)
```

---

## 📷 Vision-Based Measurement

### 📹 Camera Setup
- 📌 Fixed front-view camera positioning
- 🖨️ Custom 3D-printed mounting brackets
- 🎯 Laser-cut platform for stability
- 🔴 Red circular marker on actuator tip
- 💡 Controlled lighting to minimize noise

### 🎨 Marker Tracking (OpenCV)
```
Color Segmentation → Centroid Detection → Pixel-to-mm Conversion → Displacement
     [Red Detection]      [Image Processing]    [Calibration: 1px ≈ 0.1mm]    [Output]
```

---

## 📊 Data Collection

### 📋 Experimental Parameters

| Parameter | Range | Units |
|-----------|-------|-------|
| **Volume** | 5, 10, 20, 30 | ml |
| **Flow Rate** | 5 – 35 | ml/min |
| **Chambers** | 1 – 2 | configuration |

### 📈 Measured Outputs
- ✅ Tip displacement (primary)
- ✅ Inflation time
- ❌ Pressure (discarded - noisy)

### ⚠️ Why Pressure Sensing Failed

| Issue | Impact | Resolution |
|-------|--------|-----------|
| 🔊 High noise | Unreliable readings | Removed from model |
| 📉 Poor correlation | Weak predictive power | Used volume/flow instead |
| 🔄 Inconsistent trials | Non-repeatable results | Switched to vision-based |

**Lesson Learned:** 👉 *Volume and flow rate provided superior stability and predictability*

---

## 🧹 Data Processing

**Pipeline:**
```
Raw CSV Files → Cleaning → Merging → Validation → Final Dataset
                    ↓
            data collection/processed/final_dataset.csv
```

---

## 🤖 Machine Learning Model

### 🔍 Model Selection: **Polynomial Regression**

#### Why Polynomial Over Alternatives?
| Aspect | Linear | Polynomial ✓ | Neural Net |
|--------|--------|--------------|-----------|
| Nonlinearity Capture | ❌ | ✅ | ⚠️ |
| Interpretability | ✅ | ✅ | ❌ |
| Data Efficiency | ⚠️ | ✅ | ❌ |
| Physical Match | ❌ | ✅ | ⚠️ |

> **The polynomial model captures the intrinsic nonlinear actuator behavior with minimal data requirements**

---

## 📈 Results

### 🎯 Prediction Performance
- ✨ **Strong agreement** between predicted and actual values
- 📍 Most points lie **close to ideal line**
- 🎪 Slight deviation at higher deformation (expected)

### 🌊 Deformation Trends
| Observation | Behavior |
|-------------|----------|
| Flow Rate Effect | Nonlinear relationship |
| Higher Flow | Faster response |
| Volume Impact | Different deformation patterns |

### ⏱️ Flow Rate vs Time
- ⬇️ Inflation time **decreases** with flow rate
- ⬆️ Higher volume → **longer inflation time**
- 📉 Nonlinear saturation at high flow rates

### 🔄 Combined Volume Analysis
- 🔁 **Consistent behavior** across all volumes
- 🎯 Repeatable system response
- ✓ Confirms stable input-output relationship
---

## 📂 Project Structure

```
soft balloon actuator state estimation/
│
├── 📄 LICENSE
├── 📖 README.md
├── 🔐 .gitignore
│
├── 🎨 cad for laser cutting & 3d printing/
│   ├── Bottom plate for AL (DXF/SLDDRW/SLDPRT/AI)
│   ├── Camera mount (SLDPRT)
│   ├── GoPro mounts (3 variants - SLDPRT)
│   ├── Stand & Table (DXF/SLDDRW/SLDPRT)
│   └── Keeper board (SLDPRT)
│
├── 📊 data collection/
│   ├── 📁 processed/
│   │   └── final_dataset.csv ⭐
│   │
│   ├── 📁 raw/
│   │   ├── 1 chamber/
│   │   │   ├── 5ml, 10ml, 20ml, 30ml (volume variants)
│   │   │   ├── Each: 5-35 ml/min flow rates (CSV)
│   │   │   └── final_dataset.csv (aggregated)
│   │   │
│   │   ├── 2 chamber/
│   │   │   └── 20ml with 4 flow rates
│   │   │
│   │   └── 📸 pessure sensing noisy data.png
│   │
│   └── 📁 videos/
│       ├── 1 chamber (5,10,20,30ml - MP4)
│       └── 2 chamber (20ml - MP4)
│
├── 📈 results/
│   ├── 📊 Flow rate vs time/
│   │   ├── 5ml.png
│   │   ├── 10ml.png
│   │   ├── 20ml.png
│   │   ├── 30ml.png
│   │   └── Combine volume.png
│   │
│   └── 🤖 ml model performance/
│       ├── Actual vs Predicted.png
│       ├── Deformation vs Flow Rate.png
│       └── Deformation vs Volume.png
│
├── 💻 src/
│   ├── 🔌 arduino/
│   │   ├── epm control/
│   │   │   └── Multiple Epm Control.ino
│   │   └── pressure sensing/
│   │       └── pressure sensing.ino
│   │
│   ├── 🧹 data processing/
│   │   └── Clean_CSV.py
│   │
│   ├── 🤖 ml model/
│   │   └── Polynomial_Regression.py
│   │
│   ├── 👁️ vision/
│   │   └── marker_tracking.py
│   │
│   └── 📊 visualization/
│       ├── all volume combine.py
│       ├── flow rate vs time- 20 ml.py
│       ├── flow rate vs time- 30 ml.py
│       ├── Flow Rate vs Time- 5 ml.py
│       └── flow rate vs time-10 ml.py
│
└── 🖨️ stl files for 3d printing/
    ├── Board Keeper.STL
    ├── camera_mount_camra.3MF
    ├── GoPro Camera Side Screw Adapter.STL
    ├── GoPro Flat Mount Tripod Side.3MF
    ├── marker2.STL
    └── Stand.STL
```

---

## 🚀 Quick Start Guide

### 1️⃣ Install Dependencies
```bash
pip install numpy pandas matplotlib opencv-python scikit-learn
```

### 2️⃣ Process Data
```bash
python src/data\ processing/Clean_CSV.py
```

### 3️⃣ Run Vision Tracking
```bash
python src/vision/marker_tracking.py
```

### 4️⃣ Train ML Model
```bash
python src/ml\ model/Polynomial_Regression.py
```

### 5️⃣ Generate Visualizations
```bash
python src/visualization/all\ volume\ combine.py
```

> ⏱️ **Total Runtime:** ~2-3 minutes (depending on video length)

---

## 💡 Key Insights

### The Core Discovery

> **Soft actuators exhibit nonlinear dynamic behavior, and deformation can be effectively estimated using volume and flow rate, eliminating the need for complex pressure sensing.**

### Why This Matters

| Traditional Approach | Our Approach |
|---------------------|--------------|
| 🔴 Pressure sensors | ✅ Vision + Volume/Flow |
| ❌ High noise | ✅ Low noise |
| ❌ Complex calibration | ✅ Simple calibration |
| ❌ Cost & reliability | ✅ Robust & scalable |

---

## 🔬 Future Work

| Priority | Initiative | Impact |
|----------|-----------|--------|
| 🟢 High | Real-time control integration | Practical deployment |
| 🟢 High | Closed-loop feedback system | Improved accuracy |
| 🟡 Medium | Multi-chamber modeling | Extended capabilities |
| 🟡 Medium | Advanced ML models (LSTM, GNN) | State-of-the-art performance |

---

## 🏗️ Technology Stack

| Category | Technologies |
|----------|--------------|
| **Hardware** | Arduino Nano, MOSFET, Syringe Pump, Pressure Sensor |
| **Vision** | OpenCV, Python, Marker Detection |
| **ML/Analysis** | Scikit-learn, Polynomial Regression, NumPy, Pandas |
| **Visualization** | Matplotlib, Python |
| **CAD** | SolidWorks, Fusion 360 |

---

## 👩‍💻 Author

**Mansi Singh**  
🎓 *MS in Robotics | AI | Manipulation Robotics*

---

## 📜 License

This project is licensed under the **MIT License** - see [LICENSE](LICENSE) file for details.

---

<div align="center">

### ⭐ If you find this project useful, please star it! ⭐

</div>
