#include "ApiHandler.h"
#include <iostream>
#include <sstream>

// ==================== CONSTRUCTOR ====================
ApiHandler::ApiHandler() {
    nextId = 1;
    totalFoodStock = 500;
    initializeSampleData();
}

// ==================== INITIALIZE SAMPLE DATA WITH REALISTIC VALUES ====================
void ApiHandler::initializeSampleData() {
    // Formula: Requested Quantity = Beneficiaries × 0.5 kg per person (average meal)
    // Large NGOs (500-800 beneficiaries)
    ngos.push_back(NGO(nextId++, "Roti Bank", 10, 750, 375));      // 750 people → 375 kg
    ngos.push_back(NGO(nextId++, "Khalsa Aid", 9, 680, 340));      // 680 people → 340 kg
    ngos.push_back(NGO(nextId++, "Akshaya Patra", 8, 620, 310));   // 620 people → 310 kg
    
    // Medium NGOs (300-500 beneficiaries)
    ngos.push_back(NGO(nextId++, "Food For Life", 9, 480, 240));    // 480 people → 240 kg
    ngos.push_back(NGO(nextId++, "Seva Sadan", 8, 420, 210));       // 420 people → 210 kg
    ngos.push_back(NGO(nextId++, "Goonj", 7, 380, 190));            // 380 people → 190 kg
    ngos.push_back(NGO(nextId++, "Helping Hands", 7, 350, 175));    // 350 people → 175 kg
    
    // Small NGOs (150-250 beneficiaries)
    ngos.push_back(NGO(nextId++, "Annapurna Foundation", 5, 200, 100));  // 200 people → 100 kg
    ngos.push_back(NGO(nextId++, "Little Help", 6, 180, 90));            // 180 people → 90 kg
    ngos.push_back(NGO(nextId++, "Hope Initiative", 4, 150, 75));        // 150 people → 75 kg
    
    // Calculate initial priorities
    PriorityCalculator::calculateAllPriorities(ngos);
    
    // Add sample donations
    Donation d1;
    d1.date = "2024-03-20 10:30:00";
    d1.supplier = "Delhi University Hostel";
    d1.foodType = "Cooked Meal";
    d1.quantity = 150;
    donations.push_back(d1);
    
    Donation d2;
    d2.date = "2024-03-19 18:45:00";
    d2.supplier = "IIT Delhi Mess";
    d2.foodType = "Vegetables & Rice";
    d2.quantity = 200;
    donations.push_back(d2);
    
    Donation d3;
    d3.date = "2024-03-18 12:15:00";
    d3.supplier = "JNU Canteen";
    d3.foodType = "Bread & Curry";
    d3.quantity = 100;
    donations.push_back(d3);
}

// ==================== JSON PARSING HELPERS ====================
std::string ApiHandler::getJsonValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\":";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";
    
    pos += searchKey.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    
    if (json[pos] == '"') {
        pos++;
        size_t end = json.find('"', pos);
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos);
    } else {
        size_t end = pos;
        while (end < json.length() && (isdigit(json[end]) || json[end] == '-' || json[end] == '.')) end++;
        return json.substr(pos, end - pos);
    }
}

int ApiHandler::getJsonInt(const std::string& json, const std::string& key) {
    std::string val = getJsonValue(json, key);
    if (val.empty()) return 0;
    return std::stoi(val);
}

double ApiHandler::getJsonDouble(const std::string& json, const std::string& key) {
    std::string val = getJsonValue(json, key);
    if (val.empty()) return 0.0;
    return std::stod(val);
}

std::string ApiHandler::getJsonString(const std::string& json, const std::string& key) {
    return getJsonValue(json, key);
}

// ==================== API HANDLERS ====================

// API 1: Get all NGOs
std::string ApiHandler::handleGetNGOs() {
    std::string json = "[";
    for (size_t i = 0; i < ngos.size(); i++) {
        json += ngos[i].toJSON();
        if (i < ngos.size() - 1) json += ",";
    }
    json += "]";
    return json;
}

// API 2: Add new NGO
std::string ApiHandler::handleAddNGO(const std::string& body) {
    std::string name = getJsonString(body, "name");
    int urgency = getJsonInt(body, "urgency");
    int beneficiaries = getJsonInt(body, "beneficiaries");
    int quantity = getJsonInt(body, "quantity");
    
    if (name.empty()) {
        return "{\"status\":\"error\",\"message\":\"NGO name is required\"}";
    }
    
    if (urgency < 1 || urgency > 10) {
        return "{\"status\":\"error\",\"message\":\"Urgency must be between 1 and 10\"}";
    }
    
    if (beneficiaries <= 0) {
        return "{\"status\":\"error\",\"message\":\"Beneficiaries must be positive\"}";
    }
    
    // Auto-calculate quantity if not provided or unrealistic
    int suggestedQuantity = beneficiaries * 0.5;  // 0.5 kg per person
    if (quantity <= 0 || quantity > beneficiaries * 1.0) {
        quantity = suggestedQuantity;
    }
    
    NGO ngo(nextId++, name, urgency, beneficiaries, quantity);
    ngos.push_back(ngo);
    
    // Recalculate priorities after adding
    PriorityCalculator::calculateAllPriorities(ngos);
    
    std::string json = "{\"status\":\"success\",\"id\":" + std::to_string(nextId-1) + "}";
    return json;
}

// API 3: Calculate priority scores
std::string ApiHandler::handleCalculatePriority() {
    PriorityCalculator::calculateAllPriorities(ngos);
    
    std::string json = "[";
    for (size_t i = 0; i < ngos.size(); i++) {
        json += ngos[i].toJSON();
        if (i < ngos.size() - 1) json += ",";
    }
    json += "]";
    return json;
}

// API 4: Get total food stock
std::string ApiHandler::handleGetFoodStock() {
    return "{\"foodStock\":" + std::to_string(totalFoodStock) + "}";
}

// API 5: Add food donation
std::string ApiHandler::handleAddFood(const std::string& body) {
    int quantity = getJsonInt(body, "quantity");
    std::string supplier = getJsonString(body, "supplier");
    std::string foodType = getJsonString(body, "foodType");
    
    if (quantity <= 0) {
        return "{\"status\":\"error\",\"message\":\"Quantity must be positive\"}";
    }
    
    totalFoodStock += quantity;
    
    // Record donation
    Donation donation;
    donation.date = getCurrentDateTime();
    donation.supplier = supplier.empty() ? "Anonymous" : supplier;
    donation.foodType = foodType.empty() ? "Mixed Food" : foodType;
    donation.quantity = quantity;
    donation.status = "Received";
    donations.push_back(donation);
    
    std::string json = "{\"status\":\"success\",\"foodStock\":" + std::to_string(totalFoodStock) + "}";
    return json;
}

// API 6: Run allocation
std::string ApiHandler::handleAllocate(const std::string& body) {
    int algorithm = getJsonInt(body, "algorithm");
    int foodStock = getJsonInt(body, "food_stock");
    
    if (foodStock > 0) {
        totalFoodStock = foodStock;
    }
    
    if (ngos.empty()) {
        return "{\"status\":\"error\",\"message\":\"No NGOs registered\"}";
    }
    
    if (totalFoodStock <= 0) {
        return "{\"status\":\"error\",\"message\":\"No food available for allocation\"}";
    }
    
    // Step 1: Calculate priorities (C++ Algorithm)
    PriorityCalculator::calculateAllPriorities(ngos);
    
    // Step 2: Sort NGOs using selected algorithm (C++ Algorithm)
    std::vector<NGO> sortedNgos = ngos;
    SortAlgorithms::sortByAlgorithm(sortedNgos, algorithm);
    
    // Step 3: Run greedy allocation (C++ Algorithm)
    GreedyAllocation allocator(totalFoodStock);
    std::vector<AllocationResult> results = allocator.allocate(sortedNgos);
    
    // Step 4: Update food stock
    totalFoodStock = allocator.getRemainingFood();
    
    // Step 5: Return results with complexity analysis
    std::string json = "{";
    json += "\"results\":" + allocator.resultsToJSON(results) + ",";
    json += "\"totalAllocated\":" + std::to_string(allocator.getTotalAllocated()) + ",";
    json += "\"remainingFood\":" + std::to_string(allocator.getRemainingFood()) + ",";
    json += "\"efficiency\":" + std::to_string(allocator.getEfficiency()) + ",";
    json += "\"algorithm\":\"" + SortAlgorithms::getAlgorithmName(algorithm) + "\",";
    json += "\"timeComplexity\":\"" + SortAlgorithms::getTimeComplexity(algorithm) + "\",";
    json += "\"spaceComplexity\":\"" + SortAlgorithms::getSpaceComplexity(algorithm) + "\"";
    json += "}";
    
    return json;
}

// API 7: Delete NGO
std::string ApiHandler::handleDeleteNGO(int id) {
    auto it = std::remove_if(ngos.begin(), ngos.end(),
        [id](const NGO& ngo) { return ngo.getId() == id; });
    
    if (it != ngos.end()) {
        ngos.erase(it, ngos.end());
        return "{\"status\":\"success\"}";
    } else {
        return "{\"status\":\"error\",\"message\":\"NGO not found\"}";
    }
}

// API 8: Update NGO
std::string ApiHandler::handleUpdateNGO(int id, const std::string& body) {
    for (auto& ngo : ngos) {
        if (ngo.getId() == id) {
            std::string name = getJsonString(body, "name");
            int urgency = getJsonInt(body, "urgency");
            int beneficiaries = getJsonInt(body, "beneficiaries");
            int quantity = getJsonInt(body, "quantity");
            
            if (!name.empty()) ngo.setName(name);
            if (urgency >= 1 && urgency <= 10) ngo.setUrgencyLevel(urgency);
            if (beneficiaries > 0) {
                ngo.setPeopleCount(beneficiaries);
                // Auto-update quantity if realistic
                if (quantity <= 0 || quantity > beneficiaries * 1.0) {
                    quantity = beneficiaries * 0.5;
                }
            }
            if (quantity > 0) ngo.setRequestedQuantity(quantity);
            
            // Recalculate priority
            int priority = PriorityCalculator::calculatePriority(ngo.getUrgencyLevel(), ngo.getPeopleCount());
            ngo.setPriorityScore(priority);
            
            return "{\"status\":\"success\"}";
        }
    }
    
    return "{\"status\":\"error\",\"message\":\"NGO not found\"}";
}

// API 9: Get single NGO by ID
std::string ApiHandler::handleGetNGO(int id) {
    for (const auto& ngo : ngos) {
        if (ngo.getId() == id) {
            return ngo.toJSON();
        }
    }
    return "{\"status\":\"error\",\"message\":\"NGO not found\"}";
}

// API 10: Get priority statistics
std::string ApiHandler::handleGetPriorityStats() {
    if (ngos.empty()) {
        return "{\"status\":\"error\",\"message\":\"No NGOs available\"}";
    }
    
    int minPriority = ngos[0].getPriorityScore();
    int maxPriority = ngos[0].getPriorityScore();
    int total = 0;
    int highPriority = 0, mediumPriority = 0, lowPriority = 0;
    
    for (const auto& ngo : ngos) {
        int score = ngo.getPriorityScore();
        total += score;
        if (score < minPriority) minPriority = score;
        if (score > maxPriority) maxPriority = score;
        
        if (score >= 800) highPriority++;
        else if (score >= 500) mediumPriority++;
        else lowPriority++;
    }
    
    std::string json = "{";
    json += "\"minPriority\":" + std::to_string(minPriority) + ",";
    json += "\"maxPriority\":" + std::to_string(maxPriority) + ",";
    json += "\"avgPriority\":" + std::to_string(total / ngos.size()) + ",";
    json += "\"highPriorityCount\":" + std::to_string(highPriority) + ",";
    json += "\"mediumPriorityCount\":" + std::to_string(mediumPriority) + ",";
    json += "\"lowPriorityCount\":" + std::to_string(lowPriority);
    json += "}";
    
    return json;
}

// API 11: Get donation history
std::string ApiHandler::handleGetDonations(int limit) {
    std::string json = "[";
    int start = std::max(0, (int)donations.size() - limit);
    for (int i = donations.size() - 1; i >= start; i--) {
        json += donations[i].toJSON();
        if (i > start) json += ",";
    }
    json += "]";
    return json;
}

// API 12: Get system statistics
std::string ApiHandler::handleGetStats() {
    int totalBeneficiaries = 0;
    int totalRequested = 0;
    
    for (const auto& ngo : ngos) {
        totalBeneficiaries += ngo.getPeopleCount();
        totalRequested += ngo.getRequestedQuantity();
    }
    
    std::string json = "{";
    json += "\"totalNGOs\":" + std::to_string(ngos.size()) + ",";
    json += "\"totalBeneficiaries\":" + std::to_string(totalBeneficiaries) + ",";
    json += "\"totalFoodRequested\":" + std::to_string(totalRequested) + ",";
    json += "\"currentFoodStock\":" + std::to_string(totalFoodStock) + ",";
    json += "\"totalDonations\":" + std::to_string(donations.size()) + ",";
    json += "\"avgFoodPerNGO\":" + std::to_string(ngos.empty() ? 0 : totalRequested / ngos.size());
    json += "}";
    
    return json;
}

// API 13: Reset system
std::string ApiHandler::handleReset() {
    ngos.clear();
    donations.clear();
    nextId = 1;
    totalFoodStock = 500;
    initializeSampleData();
    return "{\"status\":\"success\",\"message\":\"System reset successfully\"}";
}

// API 14: Get algorithm comparison
std::string ApiHandler::handleCompareAlgorithms() {
    std::string json = "{";
    json += "\"comparison\":\"";
    json += "QuickSort: O(n log n) avg, O(log n) space | ";
    json += "MergeSort: O(n log n), O(n) space | ";
    json += "HeapSort: O(n log n), O(1) space | ";
    json += "BubbleSort: O(n^2), O(1) space | ";
    json += "SelectionSort: O(n^2), O(1) space | ";
    json += "InsertionSort: O(n^2), O(1) space\"";
    json += "}";
    
    return json;
}

// API 15: Get complexity analysis
std::string ApiHandler::handleComplexityAnalysis(int algorithm) {
    GreedyAllocation allocator(totalFoodStock);
    std::string analysis = allocator.getComplexityAnalysis(algorithm);
    
    std::string json = "{";
    json += "\"analysis\":\"" + escapeJson(analysis) + "\"";
    json += "}";
    
    return json;
}

// ==================== HELPER FUNCTIONS ====================
std::string ApiHandler::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ApiHandler::escapeJson(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// ==================== GETTERS ====================
int ApiHandler::getTotalNGOs() const {
    return ngos.size();
}

int ApiHandler::getTotalFoodStock() const {
    return totalFoodStock;
}
