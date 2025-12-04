// graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

struct Edge {
    int to;
    double weight;
};

class Graph {
public:
    Graph() : nVertices(0), nEdges(0) {}

    bool loadRoadD(const std::string& path, bool undirected = true) {
        std::ifstream in(path);
        if (!in.is_open()) {
            std::cerr << "Error: cannot open file " << path << std::endl;
            return false;
        }
        std::string line;
        std::vector<std::tuple<int,int,double>> edges;
        int maxVertex = -1;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            if (line[0] == '#') {
                // comment line, skip
                continue;
            }
            std::istringstream iss(line);
            int u, v;
            double w;
            if (!(iss >> u >> v >> w)) continue;
            edges.emplace_back(u, v, w);
            maxVertex = std::max(maxVertex, std::max(u, v));
        }
        in.close();

        if (maxVertex < 0) {
            std::cerr << "Error: no edges found in " << path << std::endl;
            return false;
        }

        nVertices = maxVertex + 1;
        adj.assign(nVertices, {});
        nEdges = 0;

        for (auto& e : edges) {
            int u, v;
            double w;
            std::tie(u, v, w) = e;
            adj[u].push_back({v, w});
            if (undirected) {
                adj[v].push_back({u, w});
            }
            nEdges++;
        }

        return true;
    }

    int numVertices() const { return nVertices; }
    long long numEdges() const { return nEdges; }

    const std::vector<Edge>& neighbors(int u) const {
        return adj[u];
    }

private:
    int nVertices;
    long long nEdges;
    std::vector<std::vector<Edge>> adj;
};

#endif // GRAPH_H
