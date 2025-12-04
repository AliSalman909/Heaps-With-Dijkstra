// binary_heap.h
#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <cmath>
#include <cstddef>
#include "priority_queue.h"

class BinaryHeapNode : public PQNodeBase {
public:
    double key;
    int value;
    int index;      // position in heap array
    bool inHeap;

    BinaryHeapNode(double k, int v, int idx)
        : key(k), value(v), index(idx), inHeap(true) {}
};

class BinaryHeap : public PriorityQueue {
public:
    BinaryHeap() = default;
    ~BinaryHeap() override = default;

    PQNodeBase* insert(double key, int value) override {
        auto* node = new BinaryHeapNode(key, value, (int)heap.size());
        heap.push_back(node);
        siftUp((int)heap.size() - 1);
        return node;
    }

    bool empty() const override {
        return heap.empty();
    }

    std::pair<double,int> find_min() const override {
        if (heap.empty()) return {std::numeric_limits<double>::infinity(), -1};
        return {heap[0]->key, heap[0]->value};
    }

    std::pair<double,int> extract_min() override {
        if (heap.empty()) {
            return {std::numeric_limits<double>::infinity(), -1};
        }
        BinaryHeapNode* minNode = heap[0];
        std::pair<double,int> res = {minNode->key, minNode->value};

        BinaryHeapNode* last = heap.back();
        heap[0] = last;
        last->index = 0;
        heap.pop_back();

        if (!heap.empty())
            siftDown(0);

        minNode->inHeap = false;
        delete minNode;
        return res;
    }

    void decrease_key(PQNodeBase* n, double new_key) override {
        auto* node = dynamic_cast<BinaryHeapNode*>(n);
        if (!node || !node->inHeap) return;
        if (new_key >= node->key) return;
        node->key = new_key;
        siftUp(node->index);
    }

    void remove(PQNodeBase* n) override {
        auto* node = dynamic_cast<BinaryHeapNode*>(n);
        if (!node || !node->inHeap) return;
        int i = node->index;
        int lastIdx = (int)heap.size() - 1;
        if (i != lastIdx) {
            BinaryHeapNode* last = heap[lastIdx];
            heap[i] = last;
            last->index = i;
            heap.pop_back();
            if (i > 0 && heap[i]->key < heap[(i - 1) / 2]->key) {
                siftUp(i);
            } else {
                siftDown(i);
            }
        } else {
            heap.pop_back();
        }
        node->inHeap = false;
        delete node;
    }

    int getSize() const override {
        return (int)heap.size();
    }

    int getHeightEstimate() const override {
        if (heap.empty()) return 0;
        return (int)std::floor(std::log2((double)heap.size())) + 1;
    }

    int getNumTrees() const override {
        return heap.empty() ? 0 : 1;
    }

    long long getNumCascadingCuts() const override {
        return 0;
    }

    double getMemoryUsageMBEstimate() const override {
        double bytes = sizeof(BinaryHeap) + heap.capacity() * sizeof(BinaryHeapNode*);
        // plus actual nodes
        bytes += heap.size() * sizeof(BinaryHeapNode);
        return bytes / (1024.0 * 1024.0);
    }

private:
    std::vector<BinaryHeapNode*> heap;

    void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (heap[i]->key < heap[p]->key) {
                swapNodes(i, p);
                i = p;
            } else break;
        }
    }

    void siftDown(int i) {
        int n = (int)heap.size();
        while (true) {
            int l = 2 * i + 1;
            int r = 2 * i + 2;
            int smallest = i;
            if (l < n && heap[l]->key < heap[smallest]->key) smallest = l;
            if (r < n && heap[r]->key < heap[smallest]->key) smallest = r;
            if (smallest != i) {
                swapNodes(i, smallest);
                i = smallest;
            } else break;
        }
    }

    void swapNodes(int i, int j) {
        std::swap(heap[i], heap[j]);
        heap[i]->index = i;
        heap[j]->index = j;
    }
};

#endif // BINARY_HEAP_H
