import numpy as np
import matplotlib.pyplot as plt

# -----------------------------------------
# Data for 20 mL
# -----------------------------------------
flow_rate = np.array([5, 10, 15, 20, 25, 30, 35])   # mL/min
time_sec  = np.array([238, 120, 77, 50, 47, 35, 33]) # seconds

# -----------------------------------------
# Create figure
# -----------------------------------------
plt.figure(figsize=(8, 5))

# -----------------------------------------
# Measured data
# -----------------------------------------
plt.plot(
    flow_rate,
    time_sec,
    marker='o',
    linewidth=2.5,
    label="Measured Data (Experiment)"
)

# -----------------------------------------
# Trend line
# -----------------------------------------
z = np.polyfit(flow_rate, time_sec, 2)
p = np.poly1d(z)

x_smooth = np.linspace(min(flow_rate), max(flow_rate), 100)

plt.plot(
    x_smooth,
    p(x_smooth),
    linestyle='--',
    linewidth=2,
    label="Trend Line (Polynomial Fit)"
)

# -----------------------------------------
# Value labels
# -----------------------------------------
for x, y in zip(flow_rate, time_sec):
    plt.text(x, y + 5, f"{y}", ha='center', fontsize=9)

# -----------------------------------------
# Labels and title
# -----------------------------------------
plt.xlabel("Flow Rate (mL/min)", fontsize=12)
plt.ylabel("Inflation Time (seconds)", fontsize=12)

plt.title(
    "Flow Rate vs Inflation Time for 20 mL Input",
    fontsize=14,
    fontweight='bold'
)

# -----------------------------------------
# Legend
# -----------------------------------------
plt.legend(loc="upper right", fontsize=10)

# -----------------------------------------
# Grid
# -----------------------------------------
plt.grid(True, linestyle='--', alpha=0.6)

plt.tight_layout()
plt.show()