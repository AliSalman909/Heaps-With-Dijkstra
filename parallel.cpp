%%writefile experiment_parallel_binary.cpp
// experiment_parallel_binary.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "graph.h"
#include "dijkstra.h"
#include "binary_heap.h"
#include "parallel_dijkstra.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: ./expParallel_bin <datasetIndex> <numThreads>\n";
        std::cerr << "Example: ./expParallel_bin 1 4\n";
        return 1;
    }

    int datasetIdx = std::stoi(argv[1]);
    int numThreads = std::stoi(argv[2]);

    std::vector<std::string> datasetFiles = {
        "/kaggle/input/road-d-datasets/Hongkong.road-d",
        "/kaggle/input/road-d-datasets/Chongqing.road-d",
        "/kaggle/input/road-d-datasets/Shanghai.road-d"
    };

    if (datasetIdx < 0 || datasetIdx >= (int)datasetFiles.size()) {
        std::cerr << "Invalid dataset index.\n";
        return 1;
    }

    Graph g;
    if (!g.loadRoadD(datasetFiles[datasetIdx])) {
        return 1;
    }

    std::cout << "Loaded graph: " << g.numVertices()
              << " vertices, " << g.numEdges() << " edges\n";

    // choose up to 8 sources spread across the graph
    std::vector<int> sources;
    int step = std::max(1, g.numVertices() / 8);
    for (int s = 0; s < g.numVertices() && (int)sources.size() < 8; s += step) {
        sources.push_back(s);
    }

    std::vector<DijkstraMetrics> metricsParallel;
    double parallelTimeMs = 0.0;
    runDijkstraMultiSource<BinaryHeap>(g, sources, numThreads, metricsParallel, parallelTimeMs);

    // sequential for comparison
    std::vector<DijkstraMetrics> metricsSequential;
    double sequentialTimeMs = 0.0;
    runDijkstraMultiSource<BinaryHeap>(g, sources, 1, metricsSequential, sequentialTimeMs);

    std::ofstream out("experiment_parallel_binary.txt");
    if (!out.is_open()) {
        std::cerr << "Could not open experiment_parallel_binary.txt\n";
        return 1;
    }

    out << "Experiment: Parallel Dijkstra - Binary Heap\n";
    out << "Dataset: " << datasetFiles[datasetIdx] << "\n";
    out << "Vertices: " << g.numVertices() << "\n";
    out << "Edges: " << g.numEdges() << "\n";
    out << "NumSources: " << sources.size() << "\n\n";

    out << "Threads\tTotalRuntime_ms\n";
    out << "1\t" << sequentialTimeMs << "\n";
    out << numThreads << "\t" << parallelTimeMs << "\n\n";

    out << "Per-source metrics (parallel run):\n";
    out << "Source\tRuntime_ms\tInserts\tExtractMins\tDecreaseKeys\n";
    for (size_t i = 0; i < sources.size(); ++i) {
        out << sources[i] << "\t"
            << metricsParallel[i].runtimeMs << "\t"
            << metricsParallel[i].inserts << "\t"
            << metricsParallel[i].extractMins << "\t"
            << metricsParallel[i].decreaseKeys << "\n";
    }

    out.close();
    std::cout << "Results written to experiment_parallel_binary.txt\n";
    return 0;
}
