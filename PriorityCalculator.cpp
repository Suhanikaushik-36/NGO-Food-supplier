#include "PriorityCalculator.h"
#include <algorithm>

int PriorityCalculator::calculatePriority(int urgencyLevel, int peopleCount) {
    return (urgencyLevel * URGENCY_WEIGHT) + (peopleCount * BENEFICIARIES_WEIGHT);
}

void PriorityCalculator::calculateAllPriorities(std::vector<NGO>& ngos) {
    for (auto& ngo : ngos) {
        int priority = calculatePriority(ngo.getUrgencyLevel(), ngo.getPeopleCount());
        ngo.setPriorityScore(priority);
    }
}

int PriorityCalculator::calculateCustomPriority(int urgency, int people, int w1, int w2) {
    return (urgency * w1) + (people * w2);
}

std::vector<int> PriorityCalculator::normalizeScores(const std::vector<int>& scores) {
    if (scores.empty()) return {};
    
    int maxScore = *std::max_element(scores.begin(), scores.end());
    if (maxScore == 0) return scores;
    
    std::vector<int> normalized;
    for (int score : scores) {
        normalized.push_back((score * 100) / maxScore);
    }
    return normalized;
}

std::vector<NGO> PriorityCalculator::getTopNGOs(std::vector<NGO>& ngos, int topN) {
    std::vector<NGO> sorted = ngos;
    
    // Sort by priority (descending)
    std::sort(sorted.begin(), sorted.end(),
              [](const NGO& a, const NGO& b) {
                  return a.getPriorityScore() > b.getPriorityScore();
              });
    
    if (topN > (int)sorted.size()) topN = sorted.size();
    
    std::vector<NGO> topNGOs;
    for (int i = 0; i < topN; i++) {
        topNGOs.push_back(sorted[i]);
    }
    return topNGOs;
}