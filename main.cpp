#include <iostream>
#include "httplib.h"
#include "ApiHandler.h"

using namespace httplib;

int main() {
    Server svr;
    ApiHandler apiHandler;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  NGO Food Supplier System Server" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Initialized with " << apiHandler.getTotalNGOs() << " NGOs" << std::endl;
    std::cout << "Food Stock: " << apiHandler.getTotalFoodStock() << " kg" << std::endl;
    std::cout << "========================================" << std::endl;

    // ==================== CORS HEADERS ====================
    // ✅ FIX: Add CORS headers to every response so frontend can talk to backend
    svr.set_default_headers({
        { "Access-Control-Allow-Origin",  "*" },
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS" },
        { "Access-Control-Allow-Headers", "Content-Type" }
    });

    // Handle OPTIONS preflight requests
    svr.Options(".*", [](const Request&, Response& res) {
        res.status = 204;
    });

    // ==================== API ROUTES ====================
    
    // GET endpoints
    svr.Get("/api/ngos", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleGetNGOs(), "application/json");
    });
    
    svr.Get("/api/food-stock", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleGetFoodStock(), "application/json");
    });
    
    svr.Get("/api/priority-stats", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleGetPriorityStats(), "application/json");
    });

    // ✅ FIX: Pass limit param (default 50) instead of calling with no arguments
    svr.Get("/api/donations", [&](const Request& req, Response& res) {
        int limit = 50;
        if (req.has_param("limit")) {
            limit = std::stoi(req.get_param_value("limit"));
        }
        res.set_content(apiHandler.handleGetDonations(limit), "application/json");
    });
    
    svr.Get("/api/stats", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleGetStats(), "application/json");
    });
    
    svr.Get("/api/ngo/(\\d+)", [&](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        res.set_content(apiHandler.handleGetNGO(id), "application/json");
    });
    
    svr.Get("/api/compare-algorithms", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleCompareAlgorithms(), "application/json");
    });
    
    // POST endpoints
    svr.Post("/api/ngos", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleAddNGO(req.body), "application/json");
    });
    
    svr.Post("/api/calculate-priority", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleCalculatePriority(), "application/json");
    });
    
    svr.Post("/api/add-food", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleAddFood(req.body), "application/json");
    });
    
    svr.Post("/api/allocate", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleAllocate(req.body), "application/json");
    });
    
    svr.Post("/api/reset", [&](const Request& req, Response& res) {
        res.set_content(apiHandler.handleReset(), "application/json");
    });
    
    svr.Post("/api/complexity/(\\d+)", [&](const Request& req, Response& res) {
        int algorithm = std::stoi(req.matches[1]);
        res.set_content(apiHandler.handleComplexityAnalysis(algorithm), "application/json");
    });
    
    // PUT endpoint
    svr.Put("/api/ngos/(\\d+)", [&](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        res.set_content(apiHandler.handleUpdateNGO(id, req.body), "application/json");
    });
    
    // DELETE endpoint
    svr.Delete("/api/ngos/(\\d+)", [&](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        res.set_content(apiHandler.handleDeleteNGO(id), "application/json");
    });
    
    svr.set_mount_point("/", "../frontend");
    
    std::cout << "Serving frontend from: ../frontend" << std::endl;
    std::cout << "Server listening on http://localhost:8080" << std::endl;
    std::cout << "========================================" << std::endl;
    
    svr.listen("localhost", 8080);
    
    return 0;
}