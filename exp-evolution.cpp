%%writefile experiment_evolution_fibonacci.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "graph.h"
#include "fibonacci_heap.h"
#include "dijkstra_tracked.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./expEvolution_fib <datasetIndex>\n";
        return 1;
    }

    int idx = std::stoi(argv[1]);
    std::vector<std::string> datasetFiles = {
        "/kaggle/input/road-d-datasets/Hongkong.road-d",
        "/kaggle/input/road-d-datasets/Chongqing.road-d",
        "/kaggle/input/road-d-datasets/Shanghai.road-d"
    };

    if (idx < 0 || idx >= (int)datasetFiles.size()) {
        std::cerr << "Invalid dataset index.\n";
        return 1;
    }

    Graph g;
    if (!g.loadRoadD(datasetFiles[idx])) {
        return 1;
    }

    std::cout << "Loaded graph: " << g.numVertices()
              << " vertices, " << g.numEdges() << " edges\n";

    std::ofstream out("heap_evolution_fibonacci.csv");
    if (!out.is_open()) {
        std::cerr << "Could not open heap_evolution_fibonacci.csv\n";
        return 1;
    }

    out << "step,heapSize,heapHeight,numTrees,cascadingCuts,inserts,extractMins,decreaseKeys\n";

    DijkstraMetrics metrics;

    auto callback = [&](const FibonacciHeap& pq, long long step, const DijkstraMetrics& m) {
        out << step << ","
            << pq.getSize() << ","
            << pq.getHeightEstimate() << ","
            << pq.getNumTrees() << ","
            << pq.getNumCascadingCuts() << ","
            << m.inserts << ","
            << m.extractMins << ","
            << m.decreaseKeys << "\n";
    };

    // sample every 1000 extract-min operations
    runDijkstraTracked<FibonacciHeap>(g, 0, metrics, callback, 1000);

    out.close();
    std::cout << "Heap evolution written to heap_evolution_fibonacci.csv\n";
    std::cout << "Total runtime (ms): " << metrics.runtimeMs << "\n";
    return 0;
}
