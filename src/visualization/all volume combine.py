import matplotlib.pyplot as plt

# -----------------------------
# Data
# -----------------------------
flow_rate = [5, 10, 15, 20, 25, 30, 35]

time_5  = [60, 30, 20, 15, 12, 8, 5]
time_10 = [117, 59, 40, 30, 24, 20, 17]
time_20 = [238, 120, 77, 50, 47, 35, 33]
time_30 = [353, 174, 120, 102, 95, 58, 51]

# -----------------------------
# Plot
# -----------------------------
plt.figure(figsize=(10, 6))

plt.plot(flow_rate, time_5,  marker='o', markersize=7, linewidth=2.5, label='5 mL')
plt.plot(flow_rate, time_10, marker='s', markersize=7, linewidth=2.5, label='10 mL')
plt.plot(flow_rate, time_20, marker='^', markersize=7, linewidth=2.5, label='20 mL')
plt.plot(flow_rate, time_30, marker='d', markersize=7, linewidth=2.5, label='30 mL')

# -----------------------------
# Title & Labels
# -----------------------------
plt.title("Effect of Flow Rate on Inflation Time for Different Input Volumes",
          fontsize=14, weight='bold')

plt.xlabel("Flow Rate (mL/min)", fontsize=12)
plt.ylabel("Inflation Time (seconds)", fontsize=12)

# -----------------------------
# Grid & Limits
# -----------------------------
plt.grid(True, linestyle='--', alpha=0.4)
plt.ylim(0, 380)

# -----------------------------
# Legend
# -----------------------------
plt.legend(title="Input Volume", loc='upper right', fontsize=10)

# -----------------------------
# Data Labels (ONLY for 5 mL to keep it clean)
# -----------------------------
# -----------------------------
# Layout
# -----------------------------
plt.tight_layout()
plt.show()