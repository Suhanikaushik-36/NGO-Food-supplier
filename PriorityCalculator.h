#ifndef PRIORITYCALCULATOR_H
#define PRIORITYCALCULATOR_H

#include <vector>
#include "../models/NGO.h"

class PriorityCalculator {
public:
    static const int URGENCY_WEIGHT = 60;
    static const int BENEFICIARIES_WEIGHT = 40;
    
    // Priority Score Calculation: (Urgency × 60) + (Beneficiaries × 40)
    static int calculatePriority(int urgencyLevel, int peopleCount);
    
    // Batch Priority Calculation - O(n)
    static void calculateAllPriorities(std::vector<NGO>& ngos);
    
    // Weighted Priority with custom weights
    static int calculateCustomPriority(int urgency, int people, int w1, int w2);
    
    // Normalize scores to 0-100 range
    static std::vector<int> normalizeScores(const std::vector<int>& scores);
    
    // Get top N NGOs by priority
    static std::vector<NGO> getTopNGOs(std::vector<NGO>& ngos, int topN);
};

#endif