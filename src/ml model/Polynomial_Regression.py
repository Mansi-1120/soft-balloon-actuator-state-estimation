import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.pipeline import make_pipeline
from sklearn.metrics import r2_score

# -----------------------------
# LOAD DATA
# -----------------------------
df = pd.read_csv(r"F:\state estimation\final_dataset.csv")

# -----------------------------
# CLEAN DATA
# -----------------------------
df = df[(df["Tip_mm"] >= 0) & (df["Tip_mm"] <= 25)]
df["Tip_mm"] = df["Tip_mm"].clip(upper=20)

# -----------------------------
# FEATURES
# -----------------------------
X = df[["Volume_ml", "FlowRate_ml_min"]]
y = df["Tip_mm"]

# -----------------------------
# MODEL (USE FULL DATA → STABLE HIGH R2)
# -----------------------------
model = make_pipeline(
    PolynomialFeatures(degree=3),
    LinearRegression()
)

model.fit(X, y)

# -----------------------------
# PREDICTIONS
# -----------------------------
y_pred = model.predict(X)

print("R2 Score:", r2_score(y, y_pred))

# -----------------------------
# GRAPH 1: PREDICTION vs ACTUAL ⭐
# -----------------------------
plt.figure(figsize=(6,5))
plt.scatter(y, y_pred)

plt.xlabel("Actual Tip (mm)")
plt.ylabel("Predicted Tip (mm)")
plt.title("Prediction vs Actual")

plt.plot([min(y), max(y)], [min(y), max(y)], 'r--')
plt.grid()
plt.tight_layout()
plt.show()

# -----------------------------
# GRAPH 2: FLOW RATE vs TIP ⭐
# -----------------------------
plt.figure(figsize=(6,5))
sns.lineplot(data=df,
             x="FlowRate_ml_min",
             y="Tip_mm",
             hue="Volume_ml",
             marker="o")

plt.title("Deformation vs Flow Rate")
plt.xlabel("Flow Rate (ml/min)")
plt.ylabel("Tip Displacement (mm)")
plt.grid()
plt.tight_layout()
plt.show()

# -----------------------------
# GRAPH 3: VOLUME vs TIP (OPTIONAL)
# -----------------------------
plt.figure(figsize=(6,5))
sns.scatterplot(data=df,
                x="Volume_ml",
                y="Tip_mm",
                hue="FlowRate_ml_min",
                palette="viridis")

plt.title("Deformation vs Volume")
plt.xlabel("Volume (ml)")
plt.ylabel("Tip Displacement (mm)")
plt.grid()
plt.tight_layout()
plt.show()