// fibonacci_heap.h
#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <cmath>
#include <vector>
#include <limits>
#include <cstddef>
#include "priority_queue.h"

class FibonacciHeap;

class FibonacciNode : public PQNodeBase {
public:
    double key;
    int value;
    int degree;
    bool mark;
    bool inHeap;

    FibonacciNode* parent;
    FibonacciNode* child;
    FibonacciNode* left;
    FibonacciNode* right;

    explicit FibonacciNode(double k, int v)
        : key(k), value(v), degree(0), mark(false), inHeap(true),
          parent(nullptr), child(nullptr) {
        left = right = this;
    }
};

class FibonacciHeap : public PriorityQueue {
public:
    FibonacciHeap()
        : minNode(nullptr), nNodes(0), cascadingCuts(0) {}

    ~FibonacciHeap() override {
        if (minNode)
            freeList(minNode);
    }

    PQNodeBase* insert(double key, int value) override {
        auto* x = new FibonacciNode(key, value);
        if (minNode == nullptr) {
            minNode = x;
        } else {
            insertIntoRootList(x);
            if (x->key < minNode->key)
                minNode = x;
        }
        ++nNodes;
        return x;
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
        FibonacciNode* z = minNode;
        // Add children to root list
        if (z->child) {
            FibonacciNode* c = z->child;
            std::vector<FibonacciNode*> children;
            FibonacciNode* start = c;
            do {
                children.push_back(c);
                c = c->right;
            } while (c != start);

            for (auto* x : children) {
                x->parent = nullptr;
                removeFromList(x);
                insertIntoRootList(x);
            }
        }

        removeFromList(z);
        if (z == z->right) {
            minNode = nullptr;
        } else {
            minNode = z->right;
            consolidate();
        }
        std::pair<double,int> res = {z->key, z->value};
        z->inHeap = false;
       // delete z;  //commenting this to ensure 10000 runs for exp B
        --nNodes;
        return res;
    }

    void decrease_key(PQNodeBase* nodeBase, double new_key) override {
        auto* x = dynamic_cast<FibonacciNode*>(nodeBase);
        if (!x || !x->inHeap) return;
        if (new_key > x->key) return;
        x->key = new_key;
        FibonacciNode* y = x->parent;
        if (y && x->key < y->key) {
            cut(x, y);
            cascading_cut(y);
        }
        if (x->key < minNode->key) {
            minNode = x;
        }
    }

    void remove(PQNodeBase* nodeBase) override {
        decrease_key(nodeBase, -std::numeric_limits<double>::infinity());
        extract_min();
    }

    int getSize() const override {
        return (int)nNodes;
    }

    int getHeightEstimate() const override {
        // Rough upper bound: O(log n)
        if (nNodes == 0) return 0;
        return (int)std::ceil(std::log2((double)nNodes)) + 1;
    }

    int getNumTrees() const override {
        if (!minNode) return 0;
        int count = 0;
        FibonacciNode* curr = minNode;
        do {
            ++count;
            curr = curr->right;
        } while (curr != minNode);
        return count;
    }

    long long getNumCascadingCuts() const override {
        return cascadingCuts;
    }

    double getMemoryUsageMBEstimate() const override {
        double bytes = sizeof(FibonacciHeap);
        bytes += nNodes * sizeof(FibonacciNode);
        return bytes / (1024.0 * 1024.0);
    }

private:
    FibonacciNode* minNode;
    size_t nNodes;
    long long cascadingCuts;

    void insertIntoRootList(FibonacciNode* x) {
        if (!minNode) {
            minNode = x;
            x->left = x->right = x;
        } else {
            x->left = minNode;
            x->right = minNode->right;
            minNode->right->left = x;
            minNode->right = x;
        }
        x->parent = nullptr;
        x->mark = false;
    }

    static void removeFromList(FibonacciNode* x) {
        x->left->right = x->right;
        x->right->left = x->left;
    }

    void consolidate() {
        int D = (int)std::floor(std::log2((double)nNodes)) + 2;
        std::vector<FibonacciNode*> A(D, nullptr);

        // collect root list
        std::vector<FibonacciNode*> roots;
        FibonacciNode* w = minNode;
        if (w) {
            FibonacciNode* start = w;
            do {
                roots.push_back(w);
                w = w->right;
            } while (w != start);
        }

        for (FibonacciNode* w2 : roots) {
            FibonacciNode* x = w2;
            int d = x->degree;
            while (d < D && A[d] != nullptr) {
                FibonacciNode* y = A[d];
                if (x->key > y->key) std::swap(x, y);
                link(y, x);
                A[d] = nullptr;
                ++d;
            }
            if (d >= (int)A.size()) A.resize(d + 1, nullptr);
            A[d] = x;
        }

        minNode = nullptr;
        for (FibonacciNode* a : A) {
            if (a) {
                if (!minNode) {
                    a->left = a->right = a;
                    minNode = a;
                } else {
                    a->left = minNode;
                    a->right = minNode->right;
                    minNode->right->left = a;
                    minNode->right = a;
                    if (a->key < minNode->key) minNode = a;
                }
            }
        }
    }

    void link(FibonacciNode* y, FibonacciNode* x) {
        removeFromList(y);
        if (!x->child) {
            x->child = y;
            y->left = y->right = y;
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }
        y->parent = x;
        x->degree++;
        y->mark = false;
    }

    void cut(FibonacciNode* x, FibonacciNode* y) {
        // remove x from child list of y
        if (y->child == x) {
            if (x->right != x)
                y->child = x->right;
            else
                y->child = nullptr;
        }
        x->left->right = x->right;
        x->right->left = x->left;
        y->degree--;

        // add x to root list
        insertIntoRootList(x);
        x->parent = nullptr;
        x->mark = false;
        ++cascadingCuts;
    }

    void cascading_cut(FibonacciNode* y) {
        FibonacciNode* z = y->parent;
        if (z) {
            if (!y->mark) {
                y->mark = true;
            } else {
                cut(y, z);
                cascading_cut(z);
            }
        }
    }

    void freeList(FibonacciNode* start) {
        FibonacciNode* curr = start;
        do {
            FibonacciNode* next = curr->right;
            if (curr->child) {
                freeList(curr->child);
            }
            delete curr;
            curr = next;
        } while (curr != start);
    }
};

#endif // FIBONACCI_HEAP_H
