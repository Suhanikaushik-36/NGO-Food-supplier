#ifndef SORTALGORITHMS_H
#define SORTALGORITHMS_H

#include <vector>
#include <string>
#include "../models/NGO.h"

class SortAlgorithms {
public:
    // QuickSort - O(n log n) average, O(n²) worst
    static void quickSort(std::vector<NGO>& ngos, int low, int high);
    static int partition(std::vector<NGO>& ngos, int low, int high);
    
    // MergeSort - O(n log n) always
    static void mergeSort(std::vector<NGO>& ngos, int left, int right);
    static void merge(std::vector<NGO>& ngos, int left, int mid, int right);
    
    // HeapSort - O(n log n) always
    static void heapSort(std::vector<NGO>& ngos);
    static void heapify(std::vector<NGO>& ngos, int n, int i);
    
    // BubbleSort - O(n²) - Educational
    static void bubbleSort(std::vector<NGO>& ngos);
    
    // SelectionSort - O(n²) - Educational
    static void selectionSort(std::vector<NGO>& ngos);
    
    // InsertionSort - O(n²) - Educational
    static void insertionSort(std::vector<NGO>& ngos);
    
    // Wrapper function - sorts by selected algorithm
    static void sortByAlgorithm(std::vector<NGO>& ngos, int algorithm);
    
    // Utility functions for complexity display
    static std::string getAlgorithmName(int algorithm);
    static std::string getTimeComplexity(int algorithm);
    static std::string getSpaceComplexity(int algorithm);
};

#endif