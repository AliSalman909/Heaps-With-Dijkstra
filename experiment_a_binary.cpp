// experiment_a_binary.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "graph.h"
#include "dijkstra.h"
#include "binary_heap.h"

int main() {
    std::vector<std::string> datasetFiles = {
        "/kaggle/input/road-d-datasets/Hongkong.road-d",
        "/kaggle/input/road-d-datasets/Chongqing.road-d",
        "/kaggle/input/road-d-datasets/ThirdDataset.road-d"
    };

    std::cout << "Available datasets:\n";
    for (size_t i = 0; i < datasetFiles.size(); ++i) {
        std::cout << "  [" << i << "] " << datasetFiles[i] << "\n";
    }
    std::cout << "Select dataset index: ";
    int idx;
    std::cin >> idx;
    if (idx < 0 || idx >= (int)datasetFiles.size()) {
        std::cerr << "Invalid index\n";
        return 1;
    }

    Graph g;
    if (!g.loadRoadD(datasetFiles[idx])) {
        return 1;
    }

    std::cout << "Loaded graph: " << g.numVertices()
              << " vertices, " << g.numEdges() << " edges\n";

    DijkstraMetrics metrics;
    DijkstraResult res = runDijkstra<BinaryHeap>(g, 0, metrics);

    std::ofstream out("experiment_A_binary.txt");
    out << "Experiment A - Binary Heap\n";
    out << "Dataset: " << datasetFiles[idx] << "\n";
    out << "Vertices: " << g.numVertices() << "\n";
    out << "Edges: " << g.numEdges() << "\n";
    out << "Total runtime (ms): " << metrics.runtimeMs << "\n";
    out << "Insert operations: " << metrics.inserts << "\n";
    out << "Extract-min operations: " << metrics.extractMins << "\n";
    out << "Decrease-key operations: " << metrics.decreaseKeys << "\n";

    BinaryHeap temp;
    out << "Heap height estimate: " << temp.getHeightEstimate() << "\n";
    out << "Number of trees: " << temp.getNumTrees() << "\n";
    out << "Cascading cuts: " << temp.getNumCascadingCuts() << "\n";
    out << "Memory usage estimate (MB): "
        << temp.getMemoryUsageMBEstimate() << "\n";

    out.close();

    std::cout << "Results written to experiment_A_binary.txt\n";
    return 0;
}
