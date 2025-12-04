%%writefile parallel_dijkstra.h
#ifndef PARALLEL_DIJKSTRA_H
#define PARALLEL_DIJKSTRA_H

#include <vector>
#include <thread>
#include <chrono>
#include "graph.h"
#include "dijkstra.h"

template <typename PQType>
void runDijkstraMultiSource(
    const Graph& g,
    const std::vector<int>& sources,
    int numThreads,
    std::vector<DijkstraMetrics>& metricsOut,
    double& totalRuntimeMs
) {
    if (numThreads <= 0) numThreads = 1;
    if (metricsOut.size() != sources.size()) {
        metricsOut.assign(sources.size(), DijkstraMetrics{});
    }

    auto worker = [&](int tid) {
        for (size_t i = tid; i < sources.size(); i += numThreads) {
            runDijkstra<PQType>(g, sources[i], metricsOut[i]);
        }
    };

    auto t1 = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) {
        th.join();
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    totalRuntimeMs = std::chrono::duration<double, std::milli>(t2 - t1).count();
}

#endif // PARALLEL_DIJKSTRA_H
