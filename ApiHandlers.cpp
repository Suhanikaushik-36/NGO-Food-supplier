#include "ApiHandler.h"
#include <iostream>
#include <sstream>

// ==================== CONSTRUCTOR ====================
ApiHandler::ApiHandler() {
    nextId = 1;
    totalFoodStock = 500;
    initializeSampleData();
}

// ==================== DEHRADUN NGO LOCATIONS ====================
void ApiHandler::initializeSampleData() {
    // Large NGOs (500+ beneficiaries) - Dehradun Locations
    NGO ngo1(nextId++, "Roti Bank Dehradun", 10, 750, 375);
    ngo1.setLocation(30.3165, 78.0322);  // Race Course
    ngos.push_back(ngo1);
    
    NGO ngo2(nextId++, "Khalsa Aid UK", 9, 680, 340);
    ngo2.setLocation(30.3256, 78.0419);  // Khalsa Colony
    ngos.push_back(ngo2);
    
    NGO ngo3(nextId++, "Akshaya Patra Dehradun", 8, 620, 310);
    ngo3.setLocation(30.3385, 78.0255);  // Vasant Vihar
    ngos.push_back(ngo3);
    
    // Medium NGOs (300-500 beneficiaries)
    NGO ngo4(nextId++, "Food For Life Doon", 9, 480, 240);
    ngo4.setLocation(30.3085, 78.0485);  // Dilaram Bazaar
    ngos.push_back(ngo4);
    
    NGO ngo5(nextId++, "Seva Sadan Dehradun", 8, 420, 210);
    ngo5.setLocation(30.2955, 78.0355);  // Ballupur
    ngos.push_back(ngo5);
    
    NGO ngo6(nextId++, "Goonj Uttarakhand", 7, 380, 190);
    ngo6.setLocation(30.3525, 78.0555);  // Patel Nagar
    ngos.push_back(ngo6);
    
    NGO ngo7(nextId++, "Helping Hands Doon", 7, 350, 175);
    ngo7.setLocation(30.2825, 78.0285);  // Shimla Bypass
    ngos.push_back(ngo7);
    
    // Small NGOs (150-250 beneficiaries)
    NGO ngo8(nextId++, "Annapurna Dehradun", 5, 200, 100);
    ngo8.setLocation(30.3425, 78.0685);  // Jakhan
    ngos.push_back(ngo8);
    
    NGO ngo9(nextId++, "Little Help Uttarakhand", 6, 180, 90);
    ngo9.setLocation(30.2725, 78.0455);  // Mothrowala
    ngos.push_back(ngo9);
    
    NGO ngo10(nextId++, "Hope Initiative Doon", 4, 150, 75);
    ngo10.setLocation(30.3585, 78.0185);  // Prem Nagar
    ngos.push_back(ngo10);
    
    // Calculate initial priorities
    PriorityCalculator::calculateAllPriorities(ngos);
    
    // Add sample donations
    Donation d1;
    d1.date = "2024-03-20 10:30:00";
    d1.supplier = "Graphic Era Hostel";
    d1.foodType = "Cooked Meal";
    d1.quantity = 150;
    d1.status = "Received";
    donations.push_back(d1);
    
    Donation d2;
    d2.date = "2024-03-19 18:45:00";
    d2.supplier = "DIT University Mess";
    d2.foodType = "Vegetables & Rice";
    d2.quantity = 200;
    d2.status = "Distributed";
    donations.push_back(d2);
    
    Donation d3;
    d3.date = "2024-03-18 12:15:00";
    d3.supplier = "Uttaranchal University Canteen";
    d3.foodType = "Bread & Curry";
    d3.quantity = 100;
    d3.status = "Received";
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

std::string ApiHandler::handleGetNGOs() {
    std::string json = "[";
    for (size_t i = 0; i < ngos.size(); i++) {
        json += ngos[i].toJSON();
        if (i < ngos.size() - 1) json += ",";
    }
    json += "]";
    return json;
}

std::string ApiHandler::handleAddNGO(const std::string& body) {
    std::string name = getJsonString(body, "name");
    int urgency = getJsonInt(body, "urgency");
    int beneficiaries = getJsonInt(body, "beneficiaries");
    int quantity = getJsonInt(body, "quantity");
    double latitude = getJsonDouble(body, "latitude");
    double longitude = getJsonDouble(body, "longitude");
    
    if (name.empty()) {
        return "{\"status\":\"error\",\"message\":\"NGO name is required\"}";
    }
    
    if (urgency < 1 || urgency > 10) {
        return "{\"status\":\"error\",\"message\":\"Urgency must be between 1 and 10\"}";
    }
    
    if (beneficiaries <= 0) {
        return "{\"status\":\"error\",\"message\":\"Beneficiaries must be positive\"}";
    }
    
    int suggestedQuantity = beneficiaries * 0.5;
    if (quantity <= 0 || quantity > beneficiaries * 1.0) {
        quantity = suggestedQuantity;
    }
    
    NGO ngo(nextId++, name, urgency, beneficiaries, quantity);
    
    if (latitude != 0 && longitude != 0) {
        ngo.setLocation(latitude, longitude);
    } else {
        // Default Dehradun area location
        ngo.setLocation(30.3165 + (nextId * 0.002), 78.0322 + (nextId * 0.0015));
    }
    
    ngos.push_back(ngo);
    PriorityCalculator::calculateAllPriorities(ngos);
    
    return "{\"status\":\"success\",\"id\":" + std::to_string(nextId-1) + "}";
}

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

std::string ApiHandler::handleGetFoodStock() {
    return "{\"foodStock\":" + std::to_string(totalFoodStock) + "}";
}

std::string ApiHandler::handleAddFood(const std::string& body) {
    int quantity = getJsonInt(body, "quantity");
    std::string supplier = getJsonString(body, "supplier");
    std::string foodType = getJsonString(body, "foodType");
    
    if (quantity <= 0) {
        return "{\"status\":\"error\",\"message\":\"Quantity must be positive\"}";
    }
    
    totalFoodStock += quantity;
    
    Donation donation;
    donation.date = getCurrentDateTime();
    donation.supplier = supplier.empty() ? "Anonymous" : supplier;
    donation.foodType = foodType.empty() ? "Mixed Food" : foodType;
    donation.quantity = quantity;
    donation.status = "Received";
    donations.push_back(donation);
    
    return "{\"status\":\"success\",\"foodStock\":" + std::to_string(totalFoodStock) + "}";
}

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
        return "{\"status\":\"error\",\"message\":\"No food available\"}";
    }
    
    PriorityCalculator::calculateAllPriorities(ngos);
    
    std::vector<NGO> sortedNgos = ngos;
    SortAlgorithms::sortByAlgorithm(sortedNgos, algorithm);
    
    GreedyAllocation allocator(totalFoodStock);
    std::vector<AllocationResult> results = allocator.allocate(sortedNgos);
    totalFoodStock = allocator.getRemainingFood();
    
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

std::string ApiHandler::handleDeleteNGO(int id) {
    auto it = std::remove_if(ngos.begin(), ngos.end(),
        [id](const NGO& ngo) { return ngo.getId() == id; });
    
    if (it != ngos.end()) {
        ngos.erase(it, ngos.end());
        return "{\"status\":\"success\"}";
    }
    return "{\"status\":\"error\",\"message\":\"NGO not found\"}";
}

std::string ApiHandler::handleUpdateNGO(int id, const std::string& body) {
    for (auto& ngo : ngos) {
        if (ngo.getId() == id) {
            std::string name = getJsonString(body, "name");
            int urgency = getJsonInt(body, "urgency");
            int beneficiaries = getJsonInt(body, "beneficiaries");
            int quantity = getJsonInt(body, "quantity");
            double latitude = getJsonDouble(body, "latitude");
            double longitude = getJsonDouble(body, "longitude");
            
            if (!name.empty()) ngo.setName(name);
            if (urgency >= 1 && urgency <= 10) ngo.setUrgencyLevel(urgency);
            if (beneficiaries > 0) ngo.setPeopleCount(beneficiaries);
            if (quantity > 0) ngo.setRequestedQuantity(quantity);
            if (latitude != 0 && longitude != 0) ngo.setLocation(latitude, longitude);
            
            int priority = PriorityCalculator::calculatePriority(ngo.getUrgencyLevel(), ngo.getPeopleCount());
            ngo.setPriorityScore(priority);
            
            return "{\"status\":\"success\"}";
        }
    }
    return "{\"status\":\"error\",\"message\":\"NGO not found\"}";
}

std::string ApiHandler::handleGetNGO(int id) {
    for (const auto& ngo : ngos) {
        if (ngo.getId() == id) return ngo.toJSON();
    }
    return "{\"status\":\"error\",\"message\":\"NGO not found\"}";
}

std::string ApiHandler::handleGetPriorityStats() {
    if (ngos.empty()) return "{\"status\":\"error\",\"message\":\"No NGOs\"}";
    
    int minPriority = ngos[0].getPriorityScore();
    int maxPriority = ngos[0].getPriorityScore();
    int total = 0;
    int high = 0, medium = 0, low = 0;
    
    for (const auto& ngo : ngos) {
        int score = ngo.getPriorityScore();
        total += score;
        if (score < minPriority) minPriority = score;
        if (score > maxPriority) maxPriority = score;
        if (score >= 800) high++;
        else if (score >= 500) medium++;
        else low++;
    }
    
    std::string json = "{";
    json += "\"minPriority\":" + std::to_string(minPriority) + ",";
    json += "\"maxPriority\":" + std::to_string(maxPriority) + ",";
    json += "\"avgPriority\":" + std::to_string(total / ngos.size()) + ",";
    json += "\"highPriorityCount\":" + std::to_string(high) + ",";
    json += "\"mediumPriorityCount\":" + std::to_string(medium) + ",";
    json += "\"lowPriorityCount\":" + std::to_string(low);
    json += "}";
    return json;
}

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
    json += "\"totalDonations\":" + std::to_string(donations.size());
    json += "}";
    return json;
}

std::string ApiHandler::handleReset() {
    ngos.clear();
    donations.clear();
    nextId = 1;
    totalFoodStock = 500;
    initializeSampleData();
    return "{\"status\":\"success\"}";
}

std::string ApiHandler::handleCompareAlgorithms() {
    return "{\"comparison\":\"QuickSort: O(n log n), MergeSort: O(n log n), HeapSort: O(n log n)\"}";
}

std::string ApiHandler::handleComplexityAnalysis(int algorithm) {
    GreedyAllocation allocator(totalFoodStock);
    return "{\"analysis\":\"" + escapeJson(allocator.getComplexityAnalysis(algorithm)) + "\"}";
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