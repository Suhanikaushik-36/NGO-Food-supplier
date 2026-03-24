# 🍱 FoodBridge — NGO Food Supplier System

> An intelligent food distribution network that connects surplus food to those who need it most, using priority-based greedy algorithms built in C++.

---

## 📌 Overview

FoodBridge is a full-stack web application with a **C++ backend** and a **Vanilla JS frontend**. It manages food donations from hostels and mess suppliers, registers NGOs, calculates priority scores, and runs greedy allocation algorithms to distribute food efficiently.

---

## 🗂️ Project Structure

```
NGO food suplier/
├── frontend/
│   ├── index.html          # Main UI
│   ├── app.js              # Full frontend logic + API calls
│   └── style.css           # Styling
│
├── NGO_backend/
│   ├── main.cpp            # Server entry point (cpp-httplib)
│   ├── ApiHandler.h        # API handler declarations
│   ├── ApiHandlers.cpp     # All API route implementations
│   │
│   ├── algorithm/
│   │   ├── PriorityCalculator.h / .cpp   # Priority score formula
│   │   ├── SortAlgorithms.h / .cpp       # 6 sorting algorithms
│   │   └── GreedyAllocation.h / .cpp     # Greedy food distribution
│   │
│   ├── models/
│   │   ├── NGO.h / .cpp                  # NGO model
│   │   ├── AllocationResult.h            # Allocation result model
│   │   └── Allocation.cpp
│   │
│   └── ngo_server.exe      # Compiled server (Windows)
│
└── cpp-httplib/            # HTTP library dependency
    └── httplib.h
```

---

## ⚙️ Tech Stack

| Layer | Technology |
|-------|-----------|
| Backend | C++17 |
| HTTP Server | [cpp-httplib](https://github.com/yhirose/cpp-httplib) |
| Frontend | Vanilla JavaScript, HTML, CSS |
| Algorithms | QuickSort, MergeSort, HeapSort, BubbleSort, SelectionSort, InsertionSort |
| Allocation | Greedy Algorithm |

---

## 🧠 How It Works

### Priority Formula
```
Priority Score = (Urgency × 60) + (Beneficiaries × 40)
```

### Allocation Flow
1. **Calculate** priority scores for all NGOs using the C++ `PriorityCalculator`
2. **Sort** NGOs by priority using the selected sorting algorithm
3. **Allocate** food greedily — highest priority NGO gets food first until stock runs out
4. **Return** results with allocation status: `FULL`, `PARTIAL`, or `NONE`

### Algorithm Complexity

| Step | Time | Space |
|------|------|-------|
| Priority Calculation | O(n) | O(1) |
| Sorting (QuickSort/MergeSort/HeapSort) | O(n log n) | O(log n) / O(n) / O(1) |
| Greedy Allocation | O(n) | O(1) |
| **Overall** | **O(n log n)** | **O(n)** |

---

## 🚀 Getting Started

### Prerequisites
- Windows 10 or later
- GCC (MinGW64 recommended) with C++17 support
- `cpp-httplib` (already included in repo)

### Build & Run

```bash
# Navigate to backend folder
cd NGO_backend

# Compile
g++ -std=c++17 -D_WIN32_WINNT=0x0A00 -o ngo_server.exe main.cpp ApiHandlers.cpp algorithm/SortAlgorithms.cpp algorithm/PriorityCalculator.cpp algorithm/GreedyAllocation.cpp models/NGO.cpp models/Allocation.cpp -I../cpp-httplib -lws2_32

# Run
.\ngo_server.exe
```

### Open in Browser

```
http://localhost:8080
```

> ⚠️ Do NOT open via VS Code Live Server (port 5500). Always use `localhost:8080` — the C++ server serves the frontend directly.

---

## 🔌 API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/ngos` | Get all registered NGOs |
| POST | `/api/ngos` | Add a new NGO |
| DELETE | `/api/ngos/:id` | Delete an NGO |
| PUT | `/api/ngos/:id` | Update an NGO |
| GET | `/api/food-stock` | Get current food stock |
| POST | `/api/add-food` | Add a food donation |
| GET | `/api/donations` | Get donation history |
| POST | `/api/calculate-priority` | Calculate priority scores |
| POST | `/api/allocate` | Run greedy allocation |
| GET | `/api/stats` | Get system statistics |
| POST | `/api/reset` | Reset system to defaults |

---

## 👥 Demo Accounts

| Role | Email | Password |
|------|-------|----------|
| Admin | admin@foodbridge.org | admin123 |
| Hostel | hostel@mess.edu | hostel123 |
| NGO | ngo@help.org | ngo123 |

---

## ✨ Features

- 🔐 Role-based login (Admin, Hostel, NGO)
- 📊 Dashboard with live stats
- 🏥 NGO registration and management
- 🍛 Food donation tracking
- ⚡ 6 sorting algorithm options with complexity analysis
- 🟢 Real-time backend connection status
- 📋 Exportable system report
- 🔄 Auto-refreshing food stock every 10 seconds

---

## 📸 Screenshots

> Add screenshots of your dashboard, NGO management, and allocation results here.

---

## 🙋 Author

Made with ❤️ as a DSA + Full Stack project.
