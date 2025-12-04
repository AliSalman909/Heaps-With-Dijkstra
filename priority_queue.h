// priority_queue.h
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <limits>

// We assume: key = double (distance), value = int (vertex id)

class PQNodeBase {
public:
    virtual ~PQNodeBase() = default;
};

class PriorityQueue {
public:
    virtual ~PriorityQueue() = default;

    virtual PQNodeBase* insert(double key, int value) = 0;
    virtual bool empty() const = 0;
    virtual std::pair<double,int> find_min() const = 0;
    virtual std::pair<double,int> extract_min() = 0;
    virtual void decrease_key(PQNodeBase* node, double new_key) = 0;
    virtual void remove(PQNodeBase* node) = 0;

    // For metrics
    virtual int getSize() const = 0;
    virtual int getHeightEstimate() const = 0;
    virtual int getNumTrees() const = 0;
    virtual long long getNumCascadingCuts() const = 0;
    virtual double getMemoryUsageMBEstimate() const = 0;
};

#endif // PRIORITY_QUEUE_H
