# **Dijkstra’s Algorithm Benchmark Suite — Binary, Fibonacci & Hollow Heaps (Kaggle Version)**

This project implements and benchmarks **Binary Heap**, **Fibonacci Heap**, and **Hollow Heap** inside **Dijkstra’s shortest-path algorithm**, using large-scale road network datasets from **Kaggle**.  
All experiment files, dataset paths, and execution flow are designed **specifically to run inside a Kaggle Notebook environment** for convenient access to GPU/CPU resources, large datasets, and file output.

The repository also includes:
- Full Experiment A (static routing)
- Experiment B (priority queue operation profiling)
- Heap structural evolution logging
- Parallelized multi-source Dijkstra (bonus)
- Python visualization of heap evolution

---

##  **Important Note — Kaggle Environment Required**

This project is designed around the Kaggle notebook ecosystem.  
All dataset paths reference the Kaggle input directory:

```
/kaggle/input/road-d-datasets/
```

The C++ experiments are intended to be run using:

```bash
%%writefile filename.cpp
!g++ -O2 -std=c++17 filename.cpp -o runfile
!./runfile <datasetIndex>
```

If you wish to run locally (VS Code, Linux, Windows), simply update the dataset paths in the experiment files.

---

##  **Project Structure**

```
/ (root)
│
├── graph.h
├── dijkstra.h
├── dijkstra_tracked.h
├── ParallelDijkstra.h
│
├── priority_queue.h
├── binary_heap.h
├── fibonacci_heap.h
├── hollow_heap.h
│
├── experiment_a_binary.cpp
├── experiment_a_fibonacci.cpp
├── experiment_a_hollow.cpp
│
├── experiment_b_binary.cpp
├── experiment_b_fibonacci.cpp
├── experiment_b_hollow.cpp
│
├── exp-evolution.cpp           # Heap evolution experiment (Kaggle)
├── parallel.cpp                # Multi-threaded Dijkstra (Kaggle)
├── Visual.py                   # Visualization script (Kaggle)
│
└── Kaggle datasets (not included in this repo)
```

---

##  **Features**

### **1. Unified Priority Queue Interface**
All heaps implement a shared interface in `priority_queue.h`, enabling plug-and-play integration with Dijkstra.

### **2. Three Heaps Implemented**
- **Binary Heap** — fast in practice  
- **Fibonacci Heap** — asymptotically optimal decrease-key  
- **Hollow Heap** — experimental lazy structure  

### **3. Dijkstra’s Algorithm**
Generic templated implementation that can run with any heap:
```cpp
runDijkstra<BinaryHeap>();
runDijkstra<FibonacciHeap>();
runDijkstra<HollowHeap>();
```

### **4. Experiment A — Full Dijkstra Runtime**
Files:
```
experiment_a_binary.cpp
experiment_a_fibonacci.cpp
experiment_a_hollow.cpp
```
Outputs:
- Runtime (ms)
- Inserts, extract-mins, decrease-keys
- Heap height, number of trees, cascading cuts

### **5. Experiment B — Operation Profiling**
Measures average insert, extract-min, and decrease-key cost over thousands of operations.

### **6. Bonus: Parallelized Dijkstra (Multi-Threaded)**
```
ParallelDijkstra.h
parallel.cpp
```
Runs Dijkstra from multiple sources using multiple Kaggle CPU threads.

### **7. Heap Evolution Visualization**
```
dijkstra_tracked.h
exp-evolution.cpp
Visual.py
```
Tracks heap height, number of roots, cascading cuts, and produces time-series visualizations.

---

##  **How to Run on Kaggle**

### **1. Upload this repository to Kaggle**
- Create a new **Kaggle Notebook**
- Upload:
  - All `.cpp` files  
  - All `.h` files  
  - `Visual.py`

### **2. Add the road-d-datasets Kaggle dataset**
Use **Add Data → road-d-datasets**  
This provides paths:

```
/kaggle/input/road-d-datasets/Hongkong.road-d
/kaggle/input/road-d-datasets/Chongqing.road-d
/kaggle/input/road-d-datasets/Shanghai.road-d
```

### **3. Compile and Run**
Example:
```bash
%%writefile experiment_a_binary.cpp
# (paste file contents)

!g++ -O2 -std=c++17 experiment_a_binary.cpp -o expA_bin
!./expA_bin 0
```

---

##  **How to Run Locally (VS Code, Linux, Windows, macOS)**

If running outside Kaggle:

1. Update all dataset paths from:
   ```
   /kaggle/input/road-d-datasets/
   ```
   to:
   ```
   data/
   ```
   or any folder of your choice.

2. Compile using:
   ```bash
   g++ -std=c++17 experiment_a_binary.cpp -o expA_bin
   ```

3. Run:
   ```bash
   ./expA_bin
   ```

---

## **Visualization**

### Generate heap evolution data:
```bash
!g++ -std=c++17 exp-evolution.cpp -o evo
!./evo 0
```

### Visualize in Python:
```bash
python3 Visual.py
```

Produces:
- Heap height vs time  
- Number of trees vs time  
- Cascading cuts vs operations  

---

## **Parallel Dijkstra**

```bash
!g++ -std=c++17 parallel.cpp -pthread -o parallel_run
!./parallel_run 0 4
```

Runs 4-thread multi-source Dijkstra on Hongkong dataset.



##  **Author**
Ali Bin Salman
