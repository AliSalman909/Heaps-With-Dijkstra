%%writefile dijkstra_tracked.h
#ifndef DIJKSTRA_TRACKED_H
#define DIJKSTRA_TRACKED_H

#include <vector>
#include <limits>
#include <chrono>
#include "graph.h"
#include "priority_queue.h"
#include "dijkstra.h"

template <typename PQType, typename Callback>
DijkstraResult runDijkstraTracked(
    const Graph& g,
    int source,
    DijkstraMetrics& metrics,
    Callback cb,
    long long sampleEvery = 1000
) {
    int n = g.numVertices();
    std::vector<double> dist(n, std::numeric_limits<double>::infinity());
    std::vector<int> parent(n, -1);
    std::vector<PQNodeBase*> handles(n, nullptr);

    PQType pq;
    auto start = std::chrono::high_resolution_clock::now();

    dist[source] = 0.0;
    handles[source] = pq.insert(0.0, source);
    metrics.inserts++;

    long long step = 0;

    while (!pq.empty()) {
        auto [d, u] = pq.extract_min();
        metrics.extractMins++;
        step++;

        if (d > dist[u]) continue;

        if (sampleEvery > 0 && (step % sampleEvery == 0)) {
            cb(pq, step, metrics);
        }

        for (const auto& e : g.neighbors(u)) {
            int v = e.to;
            double w = e.weight;
            double nd = d + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;
                if (handles[v] == nullptr) {
                    handles[v] = pq.insert(nd, v);
                    metrics.inserts++;
                } else {
                    pq.decrease_key(handles[v], nd);
                    metrics.decreaseKeys++;
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    metrics.runtimeMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    // final sample
    cb(pq, step, metrics);

    return DijkstraResult{dist, parent};
}

#endif // DIJKSTRA_TRACKED_H
