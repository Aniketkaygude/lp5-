#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>  // For rand()
#include <omp.h>

using namespace std;
using namespace std::chrono;

// Swap function
void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

// Sequential Bubble Sort
void s_bubble(vector<int> &arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Parallel Bubble Sort
void p_bubble(vector<int> &arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        int first = i % 2;
#pragma omp parallel for shared(arr, first) num_threads(16)
        for (int j = first; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int main() {
    int n;
    char choice;

    cout << "Enter the number of elements in the array: ";
    cin >> n;

    vector<int> a(n);
    vector<int> b(n);

    cout << "Generate random array? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        int rand_max;
        cout << "Enter the maximum random value: ";
        cin >> rand_max;

        for (int i = 0; i < n; i++) {
            a[i] = rand() % rand_max;
        }
        cout << "Random array generated.\n";
    } else {
        cout << "Enter " << n << " integers for the array:\n";
        for (int i = 0; i < n; i++) {
            cin >> a[i];
        }
    }

    // Copy array for parallel sorting
    b = a;

    // Sequential sort
    auto start = high_resolution_clock::now();
    s_bubble(a);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    
	 cout<<"\n\n Array --> "<<endl; 
     for (auto i:b ){
     	cout<<i<<"  ";
	 }
     
     cout<<endl<<endl; 
    
    cout << "Time taken for Sequential Bubble Sort: " << duration.count() << "ms\n";

    // Parallel sort
    start = high_resolution_clock::now();
    p_bubble(b);
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken for Parallel Bubble Sort: " << duration.count() << "ms\n";

    return 0;
}
