// experiment_b_binary.cpp
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <cstdlib>

#include "binary_heap.h"

int main() {
    const int NUM_OPS = 100000;
    BinaryHeap heap;

    std::vector<BinaryHeapNode*> nodes;

    long long insertCount = 0, extractCount = 0, decCount = 0;
    double insertTime = 0.0, extractTime = 0.0, decTime = 0.0;

    for (int i = 0; i < NUM_OPS; ++i) {
        int op = std::rand() % 3; // 0=insert,1=extract,2=decrease
        if (op == 0 || heap.getSize() == 0) {
            // insert
            double key = (double)(std::rand() % 1000000);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto* n = dynamic_cast<BinaryHeapNode*>(heap.insert(key, i));
            auto t2 = std::chrono::high_resolution_clock::now();
            insertTime += std::chrono::duration<double, std::micro>(t2 - t1).count();
            insertCount++;
            nodes.push_back(n);
        } else if (op == 1) {
            // extract-min
            auto t1 = std::chrono::high_resolution_clock::now();
            heap.extract_min();
            auto t2 = std::chrono::high_resolution_clock::now();
            extractTime += std::chrono::duration<double, std::micro>(t2 - t1).count();
            extractCount++;
        } else {
            // decrease-key randomly on one node
            if (!nodes.empty()) {
                int idx = std::rand() % nodes.size();
                BinaryHeapNode* n = nodes[idx];
                if (n && n->inHeap) {
                    double newK = n->key - (double)(1 + std::rand() % 100);
                    auto t1 = std::chrono::high_resolution_clock::now();
                    heap.decrease_key(n, newK);
                    auto t2 = std::chrono::high_resolution_clock::now();
                    decTime += std::chrono::duration<double, std::micro>(t2 - t1).count();
                    decCount++;
                }
            }
        }
    }

    std::ofstream out("experiment_B_binary.txt");
    out << "Experiment B - Binary Heap (Operation Profiling)\n";
    out << "Total operations: " << NUM_OPS << "\n";
    out << "Insert count: " << insertCount << "\n";
    out << "Extract-min count: " << extractCount << "\n";
    out << "Decrease-key count: " << decCount << "\n";
    out << "Average insert time (us): "
        << (insertCount ? insertTime / insertCount : 0.0) << "\n";
    out << "Average extract-min time (us): "
        << (extractCount ? extractTime / extractCount : 0.0) << "\n";
    out << "Average decrease-key time (us): "
        << (decCount ? decTime / decCount : 0.0) << "\n";
    out << "Heap height estimate: " << heap.getHeightEstimate() << "\n";
    out << "Number of trees: " << heap.getNumTrees() << "\n";
    out << "Cascading cuts: " << heap.getNumCascadingCuts() << "\n";
    out << "Memory usage estimate (MB): "
        << heap.getMemoryUsageMBEstimate() << "\n";

    out.close();
    std::cout << "Results written to experiment_B_binary.txt\n";
    return 0;
}
