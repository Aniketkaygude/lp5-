#include <omp.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using namespace std;

void merge(vector<int>& a, int i1, int j1, int i2, int j2) {
    vector<int> temp;
    int i = i1, j = i2;

    while (i <= j1 && j <= j2) {
        if (a[i] < a[j]) temp.push_back(a[i++]);
        else temp.push_back(a[j++]);
    }
    while (i <= j1) temp.push_back(a[i++]);
    while (j <= j2) temp.push_back(a[j++]);

    for (int k = 0; k < temp.size(); ++k)
        a[i1 + k] = temp[k];
}

void s_mergesort(vector<int>& a, int i, int j) {
    if (i < j) {
        int mid = (i + j) / 2;
        s_mergesort(a, i, mid);
        s_mergesort(a, mid + 1, j);
        merge(a, i, mid, mid + 1, j);
    }
}

void p_mergesort(vector<int>& a, int i, int j) {
    if (i < j) {
        if ((j - i) > 1000) {
            int mid = (i + j) / 2;
#pragma omp task shared(a)
            p_mergesort(a, i, mid);
#pragma omp task shared(a)
            p_mergesort(a, mid + 1, j);
#pragma omp taskwait
            merge(a, i, mid, mid + 1, j);
        } else {
            s_mergesort(a, i, j);
        }
    }
}

void parallel_mergesort(vector<int>& a, int i, int j) {
#pragma omp parallel num_threads(16)
    {
#pragma omp single
        p_mergesort(a, i, j);
    }
}

string bench(function<void()> fn) {
    auto start = high_resolution_clock::now();
    fn();
    auto stop = high_resolution_clock::now();
    return to_string(duration_cast<milliseconds>(stop - start).count());
}

int main() {
    int n, rand_max, choice;
    cout << "Enter array size: ";
    cin >> n;
    cout << "Enter maximum value for elements: ";
    cin >> rand_max;
    cout << "Enter 1 to generate random elements, 2 for manual input: ";
    cin >> choice;

    vector<int> a(n);
    if (choice == 1) {
        srand(time(NULL));
        for (int i = 0; i < n; ++i)
            a[i] = rand() % rand_max;
    } else {
        cout << "Enter " << n << " elements:\n";
        for (int i = 0; i < n; ++i)
            cin >> a[i];
    }

    vector<int> b = a;  // copy for parallel sort
    
    
     cout<<"\n\n Array --> "<<endl; 
     for (auto i:b ){
     	cout<<i<<"  ";
	 }
     
     cout<<endl<<endl; 
    

    cout << "\nRunning Sequential Merge Sort...\n";
    cout << "Time: " << bench([&] { s_mergesort(a, 0, n - 1); }) << "ms\n";

    cout << "\nRunning Parallel Merge Sort with 16 threads...\n";
    cout << "Time: " << bench([&] { parallel_mergesort(b, 0, n - 1); }) << "ms\n";

    if (n <= 100) {
        cout << "\nSorted array:\n";
        for (int val : b) cout << val << " ";
        cout << "\n";
    }

    return 0;
}
