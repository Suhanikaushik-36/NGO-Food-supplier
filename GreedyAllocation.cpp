#include "GreedyAllocation.h"
#include <sstream>

// Constructor
GreedyAllocation::GreedyAllocation(int foodAvailable) 
    : totalFoodAvailable(foodAvailable) {}

// ==================== CORE GREEDY ALLOCATION ALGORITHM ====================
// Time Complexity: O(n) where n is number of NGOs
// Space Complexity: O(1) excluding output storage
std::vector<AllocationResult> GreedyAllocation::allocate(std::vector<NGO>& sortedNGOs) {
    results.clear();
    int remainingFood = totalFoodAvailable;
    
    // Greedy approach: allocate to highest priority NGOs first
    for (const auto& ngo : sortedNGOs) {
        AllocationResult result;
        result.ngoId = ngo.getId();
        result.ngoName = ngo.getName();
        result.priorityScore = ngo.getPriorityScore();
        result.requestedQuantity = ngo.getRequestedQuantity();
        
        if (remainingFood >= ngo.getRequestedQuantity()) {
            // FULL ALLOCATION - NGO gets exactly what they requested
            result.allocatedQuantity = ngo.getRequestedQuantity();
            result.allocationStatus = AllocationResult::FULL_ALLOCATION;
            remainingFood -= ngo.getRequestedQuantity();
        }
        else if (remainingFood > 0) {
            // PARTIAL ALLOCATION - NGO gets whatever food is left
            result.allocatedQuantity = remainingFood;
            result.allocationStatus = AllocationResult::PARTIAL_ALLOCATION;
            remainingFood = 0;
        }
        else {
            // NO ALLOCATION - No food left
            result.allocatedQuantity = 0;
            result.allocationStatus = AllocationResult::NO_ALLOCATION;
        }
        
        results.push_back(result);
        
        // Stop if no food left
        if (remainingFood == 0) break;
    }
    
    return results;
}

// Get total allocated food across all NGOs
int GreedyAllocation::getTotalAllocated() const {
    int total = 0;
    for (const auto& r : results) {
        total += r.allocatedQuantity;
    }
    return total;
}

// Get remaining food after allocation
int GreedyAllocation::getRemainingFood() const {
    return totalFoodAvailable - getTotalAllocated();
}

// Calculate allocation efficiency percentage
double GreedyAllocation::getEfficiency() const {
    if (totalFoodAvailable == 0) return 0.0;
    return (static_cast<double>(getTotalAllocated()) / totalFoodAvailable) * 100.0;
}

// Convert allocation results to JSON string
std::string GreedyAllocation::resultsToJSON(const std::vector<AllocationResult>& results) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < results.size(); i++) {
        oss << results[i].toJSON();
        if (i < results.size() - 1) oss << ",";
    }
    oss << "]";
    return oss.str();
}

// Get detailed complexity analysis
std::string GreedyAllocation::getComplexityAnalysis(int algorithm) {
    std::ostringstream oss;
    
    oss << "\n";
    oss << "═══════════════════════════════════════════════════\n";
    oss << "         COMPLEXITY ANALYSIS REPORT\n";
    oss << "═══════════════════════════════════════════════════\n\n";
    
    oss << "📊 SORTING ALGORITHM (C++ Backend):\n";
    oss << "   Algorithm: " << SortAlgorithms::getAlgorithmName(algorithm) << "\n";
    oss << "   Time Complexity: " << SortAlgorithms::getTimeComplexity(algorithm) << "\n";
    oss << "   Space Complexity: " << SortAlgorithms::getSpaceComplexity(algorithm) << "\n\n";
    
    oss << "🎯 GREEDY ALLOCATION (C++ Backend):\n";
    oss << "   Strategy: Allocate to highest priority NGOs first\n";
    oss << "   Time Complexity: O(n) where n = number of NGOs\n";
    oss << "   Space Complexity: O(1) (excluding output storage)\n";
    oss << "   Description: Iterates through sorted NGOs once and allocates\n";
    oss << "                food greedily until stock is exhausted.\n\n";
    
    oss << "📈 PRIORITY CALCULATION (C++ Backend):\n";
    oss << "   Time Complexity: O(n)\n";
    oss << "   Space Complexity: O(1)\n";
    oss << "   Formula: Priority = (Urgency × 60) + (Beneficiaries × 40)\n\n";
    
    oss << "📦 OVERALL COMPLEXITY:\n";
    oss << "   Time Complexity: O(n log n)  (dominated by sorting)\n";
    oss << "   Space Complexity: O(n)       (for storing NGO list)\n\n";
    
    oss << "═══════════════════════════════════════════════════\n";
    
    return oss.str();
}

// Get allocation summary in text format
std::string GreedyAllocation::getSummary() const {
    std::ostringstream oss;
    
    oss << "═══════════════════════════════════════════════════\n";
    oss << "              GREEDY ALLOCATION REPORT\n";
    oss << "═══════════════════════════════════════════════════\n\n";
    
    oss << "📦 Total Food Available: " << totalFoodAvailable << " kg\n";
    oss << "✅ Total Allocated: " << getTotalAllocated() << " kg\n";
    oss << "📊 Remaining Food: " << getRemainingFood() << " kg\n";
    oss << "⚡ Efficiency: " << getEfficiency() << "%\n\n";
    
    oss << "───────────────────────────────────────────────────\n";
    oss << "NGO ALLOCATION DETAILS:\n";
    oss << "───────────────────────────────────────────────────\n\n";
    
    for (const auto& r : results) {
        oss << "🏢 " << r.ngoName << "\n";
        oss << "   Priority Score: " << r.priorityScore << "\n";
        oss << "   Requested: " << r.requestedQuantity << " kg\n";
        oss << "   Allocated: " << r.allocatedQuantity << " kg\n";
        oss << "   Status: " << r.getStatusString() << "\n\n";
    }
    
    oss << "═══════════════════════════════════════════════════\n";
    
    return oss.str();
}