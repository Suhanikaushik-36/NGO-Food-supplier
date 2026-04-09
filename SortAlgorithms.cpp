#include "SortAlgorithms.h"
#include <algorithm>

// ==================== QUICKSORT ====================
void SortAlgorithms::quickSort(std::vector<NGO>& ngos, int low, int high) {
    if (low < high) {
        int pi = partition(ngos, low, high);
        quickSort(ngos, low, pi - 1);
        quickSort(ngos, pi + 1, high);
    }
}

int SortAlgorithms::partition(std::vector<NGO>& ngos, int low, int high) {
    int pivot = ngos[high].getPriorityScore();
    int i = low - 1;
    
    for (int j = low; j <= high - 1; j++) {
        if (ngos[j].getPriorityScore() >= pivot) {  // Descending order
            i++;
            std::swap(ngos[i], ngos[j]);
        }
    }
    std::swap(ngos[i + 1], ngos[high]);
    return i + 1;
}

// ==================== MERGESORT ====================
void SortAlgorithms::mergeSort(std::vector<NGO>& ngos, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(ngos, left, mid);
        mergeSort(ngos, mid + 1, right);
        merge(ngos, left, mid, right);
    }
}

void SortAlgorithms::merge(std::vector<NGO>& ngos, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    std::vector<NGO> L(n1), R(n2);
    
    for (int i = 0; i < n1; i++) L[i] = ngos[left + i];
    for (int j = 0; j < n2; j++) R[j] = ngos[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (L[i].getPriorityScore() >= R[j].getPriorityScore()) {
            ngos[k] = L[i];
            i++;
        } else {
            ngos[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) { ngos[k] = L[i]; i++; k++; }
    while (j < n2) { ngos[k] = R[j]; j++; k++; }
}

// ==================== HEAPSORT ====================
void SortAlgorithms::heapSort(std::vector<NGO>& ngos) {
    int n = ngos.size();
    
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(ngos, n, i);
    
    for (int i = n - 1; i > 0; i--) {
        std::swap(ngos[0], ngos[i]);
        heapify(ngos, i, 0);
    }
}

void SortAlgorithms::heapify(std::vector<NGO>& ngos, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && ngos[left].getPriorityScore() > ngos[largest].getPriorityScore())
        largest = left;
    
    if (right < n && ngos[right].getPriorityScore() > ngos[largest].getPriorityScore())
        largest = right;
    
    if (largest != i) {
        std::swap(ngos[i], ngos[largest]);
        heapify(ngos, n, largest);
    }
}

// ==================== BUBBLESORT ====================
void SortAlgorithms::bubbleSort(std::vector<NGO>& ngos) {
    int n = ngos.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (ngos[j].getPriorityScore() < ngos[j + 1].getPriorityScore()) {
                std::swap(ngos[j], ngos[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;  // Optimization: array is sorted
    }
}

// ==================== SELECTIONSORT ====================
void SortAlgorithms::selectionSort(std::vector<NGO>& ngos) {
    int n = ngos.size();
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (ngos[j].getPriorityScore() > ngos[maxIdx].getPriorityScore()) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            std::swap(ngos[i], ngos[maxIdx]);
        }
    }
}

// ==================== INSERTIONSORT ====================
void SortAlgorithms::insertionSort(std::vector<NGO>& ngos) {
    int n = ngos.size();
    for (int i = 1; i < n; i++) {
        NGO key = ngos[i];
        int j = i - 1;
        while (j >= 0 && ngos[j].getPriorityScore() < key.getPriorityScore()) {
            ngos[j + 1] = ngos[j];
            j--;
        }
        ngos[j + 1] = key;
    }
}

// ==================== WRAPPER FUNCTION ====================
void SortAlgorithms::sortByAlgorithm(std::vector<NGO>& ngos, int algorithm) {
    if (ngos.empty()) return;
    
    switch(algorithm) {
        case 0:
            quickSort(ngos, 0, ngos.size() - 1);
            break;
        case 1:
            mergeSort(ngos, 0, ngos.size() - 1);
            break;
        case 2:
            heapSort(ngos);
            break;
        case 3:
            bubbleSort(ngos);
            break;
        case 4:
            selectionSort(ngos);
            break;
        case 5:
            insertionSort(ngos);
            break;
        default:
            // STL sort as fallback
            std::sort(ngos.begin(), ngos.end(),
                [](const NGO& a, const NGO& b) {
                    return a.getPriorityScore() > b.getPriorityScore();
                });
    }
}

// ==================== UTILITY FUNCTIONS ====================
std::string SortAlgorithms::getAlgorithmName(int algorithm) {
    switch(algorithm) {
        case 0: return "QuickSort";
        case 1: return "MergeSort";
        case 2: return "HeapSort";
        case 3: return "BubbleSort";
        case 4: return "SelectionSort";
        case 5: return "InsertionSort";
        default: return "STL Sort";
    }
}

std::string SortAlgorithms::getTimeComplexity(int algorithm) {
    switch(algorithm) {
        case 0: return "O(n log n) average, O(n²) worst";
        case 1: return "O(n log n) always";
        case 2: return "O(n log n) always";
        case 3: return "O(n²)";
        case 4: return "O(n²)";
        case 5: return "O(n²)";
        default: return "O(n log n)";
    }
}

std::string SortAlgorithms::getSpaceComplexity(int algorithm) {
    switch(algorithm) {
        case 0: return "O(log n)";
        case 1: return "O(n)";
        case 2: return "O(1)";
        case 3: return "O(1)";
        case 4: return "O(1)";
        case 5: return "O(1)";
        default: return "O(n)";
    }
}