import numpy as np
import matplotlib.pyplot as plt


# Your data (5 mL case)

flow_rate = np.array([5, 10, 15, 20, 25, 30, 35])   # mL/min
time_sec  = np.array([60, 30, 20, 15, 12, 8, 5])    # seconds


# Create figure

plt.figure(figsize=(8, 5))


# Plot actual measured data

plt.plot(
    flow_rate,
    time_sec,
    marker='o',
    linewidth=2.5,
    label="Measured Data (Experiment)"
)


# Polynomial fit (smooth trend line)

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


# Add value labels on points

for x, y in zip(flow_rate, time_sec):
    plt.text(x, y + 2, f"{y}", ha='center', fontsize=9)


# Labels + Title (VERY IMPORTANT)

plt.xlabel("Flow Rate (mL/min)", fontsize=12)
plt.ylabel("Inflation Time (seconds)", fontsize=12)

plt.title(
    "Flow Rate vs Inflation Time for 5 mL Input",
    fontsize=14,
    fontweight='bold'
)


# Legend (this fixes your issue)

plt.legend(loc="upper right", fontsize=10)


# Grid + styling

plt.grid(True, linestyle='--', alpha=0.6)

# Clean margins
plt.tight_layout()


# Show plot
plt.show()