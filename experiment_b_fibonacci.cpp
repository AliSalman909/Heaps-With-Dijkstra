// experiment_b_fibonacci.cpp
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "fibonacci_heap.h"

int main() {
    std::cout << "=== Experiment B — Fibonacci Heap (Operation Profiling) ===\n";

    const int NUM_OPS = 100000;

    // Seed the random number generator so profiling is not identical each run
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    FibonacciHeap heap;

    // We keep direct FibonacciNode* pointers so we can choose random nodes
    // for decrease_key operations. These pointers remain valid because
    // extract_min() no longer deletes nodes, only marks inHeap = false.
    std::vector<FibonacciNode*> nodes;
    nodes.reserve(NUM_OPS);

    long long insertCount  = 0;
    long long extractCount = 0;
    long long decCount     = 0;

    double insertTime  = 0.0; // microseconds
    double extractTime = 0.0; // microseconds
    double decTime     = 0.0; // microseconds

    for (int i = 0; i < NUM_OPS; ++i) {
        int op = std::rand() % 3; // 0 = insert, 1 = extract-min, 2 = decrease-key

        // Always insert if the heap is empty, otherwise choose randomly
        if (op == 0 || heap.getSize() == 0) {
            double key = static_cast<double>(std::rand() % 1000000);

            auto t1 = std::chrono::high_resolution_clock::now();
            PQNodeBase* base = heap.insert(key, i);
            auto t2 = std::chrono::high_resolution_clock::now();

            auto* node = dynamic_cast<FibonacciNode*>(base);
            if (node) {
                nodes.push_back(node);
            }

            insertTime += std::chrono::duration<double, std::micro>(t2 - t1).count();
            ++insertCount;
        } else if (op == 1) {
            // Extract-min (only if heap not empty based on earlier check)
            auto t1 = std::chrono::high_resolution_clock::now();
            heap.extract_min();
            auto t2 = std::chrono::high_resolution_clock::now();

            extractTime += std::chrono::duration<double, std::micro>(t2 - t1).count();
            ++extractCount;
        } else {
            // Decrease-key on a random in-heap node, if any exist
            if (nodes.empty()) {
                continue;
            }

            FibonacciNode* target = nullptr;

            // Try a few times to find a node that is still in the heap
            for (int attempt = 0; attempt < 10; ++attempt) {
                int idx = std::rand() % nodes.size();
                FibonacciNode* candidate = nodes[idx];
                if (candidate && candidate->inHeap) {
                    target = candidate;
                    break;
                }
            }

            if (!target || !target->inHeap) {
                // No valid node found this iteration
                continue;
            }

            // Choose a strictly smaller key for decrease-key
            double delta = static_cast<double>(1 + std::rand() % 100);
            double newKey = target->key - delta;

            auto t1 = std::chrono::high_resolution_clock::now();
            heap.decrease_key(target, newKey);
            auto t2 = std::chrono::high_resolution_clock::now();

            decTime += std::chrono::duration<double, std::micro>(t2 - t1).count();
            ++decCount;
        }
    }

    // Compute averages (in microseconds)
    double avgInsertUs  = insertCount  ? (insertTime  / static_cast<double>(insertCount))  : 0.0;
    double avgExtractUs = extractCount ? (extractTime / static_cast<double>(extractCount)) : 0.0;
    double avgDecUs     = decCount     ? (decTime     / static_cast<double>(decCount))     : 0.0;

    // Structural statistics of the Fibonacci heap after all operations
    int heapHeight      = heap.getHeightEstimate();
    int numTrees        = heap.getNumTrees();
    long long cascCuts  = heap.getNumCascadingCuts();
    double memUsageMB   = heap.getMemoryUsageMBEstimate();

    // Write results to file as required
    std::ofstream out("experiment_B_fibonacci.txt");
    if (!out.is_open()) {
        std::cerr << "Error: could not open experiment_B_fibonacci.txt for writing.\n";
        return 1;
    }

    out << "Experiment B - Fibonacci Heap (Operation Profiling)\n";
    out << "TotalOperations\tInsertCount\tExtractMinCount\tDecreaseKeyCount\n";
    out << NUM_OPS << "\t"
        << insertCount << "\t"
        << extractCount << "\t"
        << decCount << "\n\n";

    out << "AverageTimes_us\n";
    out << "Insert\tExtractMin\tDecreaseKey\n";
    out << avgInsertUs << "\t"
        << avgExtractUs << "\t"
        << avgDecUs << "\n\n";

    out << "HeapStructureAfterOperations\n";
    out << "HeapHeight\tNumTrees\tCascadingCuts\tMemoryUsage_MB\n";
    out << heapHeight << "\t"
        << numTrees << "\t"
        << cascCuts << "\t"
        << memUsageMB << "\n";

    out.close();

    // Console summary
    std::cout << "Results written to experiment_B_fibonacci.txt\n";
    std::cout << "Total operations: " << NUM_OPS << "\n";
    std::cout << "Insert count: " << insertCount
              << ", Extract-min count: " << extractCount
              << ", Decrease-key count: " << decCount << "\n";
    std::cout << "Average insert time (us): "   << avgInsertUs  << "\n";
    std::cout << "Average extract-min time (us): " << avgExtractUs << "\n";
    std::cout << "Average decrease-key time (us): " << avgDecUs   << "\n";
    std::cout << "Heap height estimate: " << heapHeight << "\n";
    std::cout << "Number of trees: " << numTrees << "\n";
    std::cout << "Cascading cuts: " << cascCuts << "\n";
    std::cout << "Memory usage estimate (MB): " << memUsageMB << "\n";

    return 0;
}
