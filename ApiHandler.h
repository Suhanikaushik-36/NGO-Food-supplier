#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "models/NGO.h"
#include "models/AllocationResult.h"
#include "algorithm/PriorityCalculator.h"
#include "algorithm/SortAlgorithms.h"
#include "algorithm/GreedyAllocation.h"

struct Donation {
    std::string date;
    std::string supplier;
    std::string foodType;
    int quantity;
    std::string status;
    
    std::string toJSON() const {
        std::string json = "{";
        json += "\"date\":\"" + date + "\",";
        json += "\"supplier\":\"" + supplier + "\",";
        json += "\"foodType\":\"" + foodType + "\",";
        json += "\"quantity\":" + std::to_string(quantity) + ",";
        json += "\"status\":\"" + status + "\"";
        json += "}";
        return json;
    }
};

class ApiHandler {
private:
    std::vector<NGO> ngos;
    std::vector<Donation> donations;
    int nextId;
    int totalFoodStock;
    
    // JSON parsing helpers
    std::string getJsonValue(const std::string& json, const std::string& key);
    int getJsonInt(const std::string& json, const std::string& key);
    double getJsonDouble(const std::string& json, const std::string& key);
    std::string getJsonString(const std::string& json, const std::string& key);
    
    // Helper functions
    std::string getCurrentDateTime();
    std::string escapeJson(const std::string& str);
    void initializeSampleData();

public:
    ApiHandler();
    
    // API Handlers
    std::string handleGetNGOs();
    std::string handleAddNGO(const std::string& body);
    std::string handleCalculatePriority();
    std::string handleGetFoodStock();
    std::string handleAddFood(const std::string& body);
    std::string handleAllocate(const std::string& body);
    std::string handleDeleteNGO(int id);
    std::string handleUpdateNGO(int id, const std::string& body);
    std::string handleGetNGO(int id);
    std::string handleGetPriorityStats();
    std::string handleGetDonations(int limit = 20);
    std::string handleGetStats();
    std::string handleReset();
    std::string handleCompareAlgorithms();
    std::string handleComplexityAnalysis(int algorithm);
    
    // Getters - SIRF DECLARATIONS (No implementation here)
    int getTotalFoodStock() const;
    int getTotalNGOs() const;
};