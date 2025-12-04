%%writefile hollow_heap.h
#ifndef HOLLOW_HEAP_H
#define HOLLOW_HEAP_H

#include <vector>
#include <cmath>
#include <limits>
#include <unordered_map>
#include "priority_queue.h"

class HollowNode : public PQNodeBase {
public:
    double key;
    int value;
    bool isHollow;
    int rank;
    HollowNode* child;
    HollowNode* next;
    HollowNode* secondParent; // for hollow heap structure

    HollowNode(double k, int v)
        : key(k), value(v), isHollow(false), rank(0),
          child(nullptr), next(nullptr), secondParent(nullptr) {}
};

class HollowHeap : public PriorityQueue {
public:
    HollowHeap() : minNode(nullptr), nNodes(0), maxHeapHeight(0), currentRootCount(0) {}
    static constexpr int MAX_RANK = 64;

    ~HollowHeap() override {
        // Clean up all nodes
        std::vector<HollowNode*> toDelete;
        if (minNode) {
            collectAllNodes(minNode, toDelete);
        }
        for (auto* node : toDelete) {
            delete node;
        }
    }

    PQNodeBase* insert(double key, int value) override {
        auto* node = new HollowNode(key, value);
        minNode = meld(minNode, node);
        nNodes++;
        
        // Track the actual node pointer for this value
        valueToNode[value] = node;
        
        updateHeapHeight();
        return node;
    }

    bool empty() const override {
        return minNode == nullptr;
    }

    std::pair<double,int> find_min() const override {
        if (!minNode) return {std::numeric_limits<double>::infinity(), -1};
        return {minNode->key, minNode->value};
    }

    std::pair<double,int> extract_min() override {
        if (!minNode) {
            return {std::numeric_limits<double>::infinity(), -1};
        }
        
        HollowNode* oldMin = minNode;
        std::pair<double,int> res = {oldMin->key, oldMin->value};
        
        // Remove from tracking
        valueToNode.erase(oldMin->value);
        
        // Collect all roots except oldMin into a list
        std::vector<HollowNode*> rootList;
        HollowNode* curr = minNode->next;
        while (curr != nullptr && curr != minNode) {
            HollowNode* nextNode = curr->next;
            curr->next = nullptr;
            rootList.push_back(curr);
            curr = nextNode;
        }
        
        // Add children of oldMin to root list
        curr = oldMin->child;
        while (curr != nullptr) {
            HollowNode* nextNode = curr->next;
            curr->next = nullptr;
            rootList.push_back(curr);
            curr = nextNode;
        }
        
        delete oldMin;
        nNodes--;
        minNode = nullptr;
        
        // Consolidate: link trees of same rank
        std::vector<HollowNode*> rankBuckets(MAX_RANK + 1, nullptr);
        
        for (auto* node : rootList) {
            if (node->isHollow) {
                // Hollow node: promote its children
                HollowNode* c = node->child;
                while (c != nullptr) {
                    HollowNode* nextChild = c->next;
                    c->next = nullptr;
                    if (!c->isHollow) {
                        addToRankBuckets(rankBuckets, c);
                    }
                    c = nextChild;
                }
                delete node; // Remove hollow node
            } else {
                addToRankBuckets(rankBuckets, node);
            }
        }
        
        // Rebuild heap from rank buckets
        for (auto* node : rankBuckets) {
            if (node != nullptr) {
                minNode = meld(minNode, node);
            }
        }
        
        updateHeapHeight();
        updateRootCount();
        return res;
    }

    void decrease_key(PQNodeBase* n, double new_key) override {
        auto* node = dynamic_cast<HollowNode*>(n);
        if (!node || node->isHollow) return;
        if (new_key >= node->key) return;

        // The key insight: we need to find the ACTUAL active node for this value
        // because the pointer 'n' might be stale
        int targetValue = node->value;
        
        // Find the current active node for this value
        auto it = valueToNode.find(targetValue);
        if (it != valueToNode.end()) {
            node = it->second;
        }
        
        if (node->isHollow) return; // Already hollow
        
        // Create new node with decreased key
        auto* newNode = new HollowNode(new_key, node->value);
        newNode->rank = node->rank;
        newNode->child = node->child;
        newNode->secondParent = node;
        
        // Mark old node as hollow
        node->isHollow = true;
        node->child = nullptr;
        
        // Update tracking to point to new node
        valueToNode[node->value] = newNode;
        
        // Add new node to root list
        minNode = meld(minNode, newNode);
        
        updateHeapHeight();
    }

    void remove(PQNodeBase* n) override {
        decrease_key(n, -std::numeric_limits<double>::infinity());
        extract_min();
    }

    int getSize() const override {
        return (int)nNodes;
    }

    int getHeightEstimate() const override {
        return maxHeapHeight;
    }

    int getNumTrees() const override {
        return currentRootCount;
    }

    long long getNumCascadingCuts() const override {
        return 0;
    }

    double getMemoryUsageMBEstimate() const override {
        // Count actual nodes (including hollow ones still in structure)
        size_t totalNodes = nNodes + valueToNode.size(); 
        double bytes = sizeof(HollowHeap) + totalNodes * sizeof(HollowNode);
        return bytes / (1024.0 * 1024.0);
    }

private:
    HollowNode* minNode;
    size_t nNodes;
    int maxHeapHeight;
    int currentRootCount;
    std::unordered_map<int, HollowNode*> valueToNode; // Track active node for each vertex

    void collectAllNodes(HollowNode* root, std::vector<HollowNode*>& nodes) {
        if (!root) return;
        
        std::vector<HollowNode*> stack;
        stack.push_back(root);
        
        std::unordered_map<HollowNode*, bool> visited;
        
        while (!stack.empty()) {
            HollowNode* curr = stack.back();
            stack.pop_back();
            
            if (visited[curr]) continue;
            visited[curr] = true;
            nodes.push_back(curr);
            
            // Add siblings
            HollowNode* sibling = curr->next;
            while (sibling && sibling != root && !visited[sibling]) {
                stack.push_back(sibling);
                sibling = sibling->next;
            }
            
            // Add children
            if (curr->child) {
                stack.push_back(curr->child);
            }
        }
    }

    static HollowNode* meld(HollowNode* a, HollowNode* b) {
        if (!a) return b;
        if (!b) return a;
        
        // Ensure 'a' has smaller key (will be the min)
        if (b->key < a->key) std::swap(a, b);
        
        // Insert b into root list after a
        b->next = a->next;
        a->next = b;
        
        return a;
    }

    void addToRankBuckets(std::vector<HollowNode*>& buckets, HollowNode* x) {
        x->next = nullptr; // Detach from any list
        
        while (true) {
            int r = std::min(x->rank, MAX_RANK);
            
            if (buckets[r] == nullptr) {
                buckets[r] = x;
                break;
            } else {
                HollowNode* y = buckets[r];
                buckets[r] = nullptr;
                x = link(x, y);
            }
        }
    }

    HollowNode* link(HollowNode* a, HollowNode* b) {
        if (b->key < a->key) std::swap(a, b);
        
        // Make 'b' a child of 'a'
        b->next = a->child;
        a->child = b;
        a->rank++;
        
        return a;
    }
    
    void updateHeapHeight() {
        if (!minNode) {
            maxHeapHeight = 0;
            return;
        }
        
        int maxRank = 0;
        HollowNode* curr = minNode;
        do {
            maxRank = std::max(maxRank, curr->rank);
            curr = curr->next;
            if (!curr) break;
        } while (curr != minNode);
        
        maxHeapHeight = maxRank + 1;
    }
    
    void updateRootCount() {
        currentRootCount = 0;
        if (!minNode) return;
        
        HollowNode* curr = minNode;
        do {
            currentRootCount++;
            curr = curr->next;
            if (!curr) break;
        } while (curr != minNode);
    }
};

#endif // HOLLOW_HEAP_H