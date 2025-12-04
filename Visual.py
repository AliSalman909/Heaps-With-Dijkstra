import pandas as pd
import matplotlib.pyplot as plt

csv_file = "heap_evolution_fibonacci.csv"
df = pd.read_csv(csv_file)

# 1) Heap height over time
plt.figure()
plt.plot(df["step"], df["heapHeight"])
plt.xlabel("Extract-min step")
plt.ylabel("Heap height estimate")
plt.title("Heap Height Evolution (Fibonacci Heap)")
plt.tight_layout()
plt.show()

# 2) Number of trees over time
plt.figure()
plt.plot(df["step"], df["numTrees"])
plt.xlabel("Extract-min step")
plt.ylabel("Number of trees")
plt.title("Root List Size Evolution (Fibonacci Heap)")
plt.tight_layout()
plt.show()

# 3) Cascading cuts over time
plt.figure()
plt.plot(df["step"], df["cascadingCuts"])
plt.xlabel("Extract-min step")
plt.ylabel("Total cascading cuts")
plt.title("Cascading Cuts Over Time (Fibonacci Heap)")
plt.tight_layout()
plt.show()
