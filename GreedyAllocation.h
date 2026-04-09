#ifndef GREEDYALLOCATION_H
#define GREEDYALLOCATION_H

#include <vector>
#include <string>
#include <sstream>
#include "../models/NGO.h"
#include "../models/AllocationResult.h"
#include "SortAlgorithms.h"

class GreedyAllocation {
private:
    int totalFoodAvailable;
    std::vector<AllocationResult> results;

public:
    // Constructor
    GreedyAllocation(int foodAvailable);
    
    // Core greedy allocation algorithm
    std::vector<AllocationResult> allocate(std::vector<NGO>& sortedNGOs);
    
    // Utility methods
    int getTotalAllocated() const;
    int getRemainingFood() const;
    double getEfficiency() const;
    std::string resultsToJSON(const std::vector<AllocationResult>& results);
    std::string getComplexityAnalysis(int algorithm);
    std::string getSummary() const;
};

#endif