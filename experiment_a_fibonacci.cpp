// experiment_a_fibonacci.cpp  (Kaggle non-interactive version)
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "graph.h"
#include "dijkstra.h"
#include "fibonacci_heap.h"

int main(int argc, char** argv) {

    // Expect 1 argument: dataset index
    if (argc < 2) {
        std::cerr << "Error: No dataset index provided.\n";
        std::cerr << "Usage: ./expA_fib <dataset_index>\n";
        return 1;
    }

    int idx = std::stoi(argv[1]);  // convert argument to integer

    std::vector<std::string> datasetFiles = {
        "/kaggle/input/road-d-datasets/Hongkong.road-d",
        "/kaggle/input/road-d-datasets/Chongqing.road-d",
        "/kaggle/input/road-d-datasets/Shanghai.road-d"
    };

    std::cout << "=== Experiment A — Fibonacci Heap (Static Routing) ===\n";
    std::cout << "Dataset index provided: " << idx << "\n";

    if (idx < 0 || idx >= static_cast<int>(datasetFiles.size())) {
        std::cerr << "Error: Invalid dataset index.\n";
        return 1;
    }

    // Load graph
    Graph g;
    if (!g.loadRoadD(datasetFiles[idx])) {
        return 1; // loadRoadD prints its own error
    }

    std::cout << "Loaded graph: " << g.numVertices()
              << " vertices, " << g.numEdges() << " edges\n";

    // Run Dijkstra with Fibonacci heap
    DijkstraMetrics metrics;
    DijkstraResult result = runDijkstra<FibonacciHeap>(g, 0, metrics);

    // Build heap to measure structural properties (one insert per reachable vertex)
    FibonacciHeap structuralHeap;
    for (int v = 0; v < g.numVertices(); ++v) {
        if (result.dist[v] < std::numeric_limits<double>::infinity()) {
            structuralHeap.insert(result.dist[v], v);
        }
    }

    // Gather structural statistics
    int heapHeight      = structuralHeap.getHeightEstimate();
    int numTrees        = structuralHeap.getNumTrees();
    long long cascCuts  = structuralHeap.getNumCascadingCuts();
    double memUsageMB   = structuralHeap.getMemoryUsageMBEstimate();

    // Write output table
    std::ofstream out("experiment_A_fibonacci.txt");
    if (!out.is_open()) {
        std::cerr << "Error: Cannot open experiment_A_fibonacci.txt\n";
        return 1;
    }

    out << "HeapType\tDataset\tVertices\tEdges\t"
           "TotalRuntime_ms\tInserts\tExtractMins\tDecreaseKeys\t"
           "HeapHeight\tNumTrees\tCascadingCuts\tMemoryUsage_MB\n";

    out << "Fibonacci\t"
        << datasetFiles[idx] << "\t"
        << g.numVertices() << "\t"
        << g.numEdges() << "\t"
        << metrics.runtimeMs << "\t"
        << metrics.inserts << "\t"
        << metrics.extractMins << "\t"
        << metrics.decreaseKeys << "\t"
        << heapHeight << "\t"
        << numTrees << "\t"
        << cascCuts << "\t"
        << memUsageMB << "\n";

    out.close();

    std::cout << "Results written to experiment_A_fibonacci.txt\n";
    return 0;
}
