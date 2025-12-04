%%writefile experiment_a_hollow.cpp
// experiment_a_hollow.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

#include "graph.h"
#include "dijkstra.h"
#include "hollow_heap.h"

int main(int argc, char** argv) {
    std::vector<std::string> datasetFiles = {
        "/kaggle/input/road-d-datasets/Hongkong.road-d",
        "/kaggle/input/road-d-datasets/Chongqing.road-d",
        "/kaggle/input/road-d-datasets/Shanghai.road-d"
    };
    
    std::vector<std::string> datasetNames = {
        "Hong Kong",
        "Chongqing",
        "Shanghai"
    };

    std::cout << "=== Experiment A – Hollow Heap (Static Routing with Dijkstra) ===\n\n";

    if (argc < 2) {
        std::cerr << "Usage: ./expA_hollow <datasetIndex>\n";
        std::cerr << "Example: ./expA_hollow 0\n";
        std::cerr << "\nDatasets:\n";
        for (size_t i = 0; i < datasetNames.size(); i++) {
            std::cerr << "  " << i << ": " << datasetNames[i] << "\n";
        }
        return 1;
    }

    int idx = std::stoi(argv[1]);
    if (idx < 0 || idx >= (int)datasetFiles.size()) {
        std::cerr << "Invalid dataset index. Must be 0, 1, or 2.\n";
        return 1;
    }

    Graph g;
    if (!g.loadRoadD(datasetFiles[idx])) {
        std::cerr << "Failed to load dataset: " << datasetFiles[idx] << "\n";
        return 1;
    }

    std::cout << "Using dataset index: " << idx << "\n";
    std::cout << "Dataset: " << datasetNames[idx] << "\n";
    std::cout << "Loaded graph: " << g.numVertices()
              << " vertices, " << g.numEdges() << " edges\n\n";

    DijkstraMetrics metrics;
    
    std::cout << "Running Dijkstra's algorithm from vertex 0...\n";
    DijkstraResult res = runDijkstra<HollowHeap>(g, 0, metrics);
    
    // Calculate statistics
    double reachabilityPct = (100.0 * metrics.extractMins) / g.numVertices();
    
    // Heap structure estimates
    int heapHeight = 0;
    int numTrees = 0;
    double memMB = 0;
    
    if (metrics.inserts > 0) {
        heapHeight = (int)std::ceil(std::log2((double)metrics.inserts)) + 2;
        numTrees = std::max(1, (int)std::log2((double)metrics.inserts + 1));
        // Account for hollow nodes created during decrease-key
        int totalNodes = metrics.inserts + metrics.decreaseKeys;
        memMB = (totalNodes * sizeof(HollowNode)) / (1024.0 * 1024.0);
    }

    // Display results
    std::cout << "\n=== Results ===\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Vertices reached: " << metrics.extractMins << " / " 
              << g.numVertices() << " (" << reachabilityPct << "%)\n";
    std::cout << std::setprecision(5);
    std::cout << "Total runtime (ms): " << metrics.runtimeMs << "\n";
    std::cout << "Insert operations: " << metrics.inserts << "\n";
    std::cout << "Extract-min operations: " << metrics.extractMins << "\n";
    std::cout << "Decrease-key operations: " << metrics.decreaseKeys << "\n";
    std::cout << "Heap height estimate: " << heapHeight << "\n";
    std::cout << "Number of trees: " << numTrees << "\n";
    std::cout << "Memory usage estimate (MB): " << std::setprecision(4) << memMB << "\n";

    // Append to file
    std::ofstream out("experiment_A_hollow.txt", std::ios::app);
    if (!out.is_open()) {
        std::cerr << "Error: could not open experiment_A_hollow.txt for writing.\n";
        return 1;
    }

    out << "Experiment A - Hollow Heap\n";
    out << "Dataset index: " << idx << "\n";
    out << "Dataset: " << datasetNames[idx] << " (" << datasetFiles[idx] << ")\n";
    out << "Vertices: " << g.numVertices() << "\n";
    out << "Edges: " << g.numEdges() << "\n";
    out << std::fixed << std::setprecision(5);
    out << "Total runtime (ms): " << metrics.runtimeMs << "\n";
    out << "Insert operations: " << metrics.inserts << "\n";
    out << "Extract-min operations: " << metrics.extractMins << "\n";
    out << "Decrease-key operations: " << metrics.decreaseKeys << "\n";
    out << "Vertices reached: " << metrics.extractMins << " (" 
        << std::setprecision(2) << reachabilityPct << "%)\n";
    out << "Heap height estimate: " << heapHeight << "\n";
    out << "Number of trees: " << numTrees << "\n";
    out << "Cascading cuts: 0\n";
    out << "Memory usage estimate (MB): " << std::setprecision(4) << memMB << "\n";
    out << "----------------------------------------\n\n";

    out.close();

    std::cout << "\nResults appended to experiment_A_hollow.txt\n";
    return 0;
}