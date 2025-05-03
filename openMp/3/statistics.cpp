#include <limits.h>
#include <omp.h>
#include <stdlib.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using namespace std;

void s_avg(const vector<int>& arr) {
    long sum = 0L;
    for (int val : arr) sum += val;
    cout << sum / long(arr.size());
}

void p_avg(const vector<int>& arr) {
    long sum = 0L;
#pragma omp parallel for reduction(+ : sum) num_threads(16)
    for (int i = 0; i < arr.size(); i++) {
        sum += arr[i];
    }
    cout << sum / long(arr.size());
}

void s_sum(const vector<int>& arr) {
    long sum = 0L;
    for (int val : arr) sum += val;
    cout << sum;
}

void p_sum(const vector<int>& arr) {
    long sum = 0L;
#pragma omp parallel for reduction(+ : sum) num_threads(16)
    for (int i = 0; i < arr.size(); i++) {
        sum += arr[i];
    }
    cout << sum;
}

void s_max(const vector<int>& arr) {
    int max_val = INT_MIN;
    for (int val : arr) {
        if (val > max_val) max_val = val;
    }
    cout << max_val;
}

void p_max(const vector<int>& arr) {
    int max_val = INT_MIN;
#pragma omp parallel for reduction(max : max_val) num_threads(16)
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }
    cout << max_val;
}

void s_min(const vector<int>& arr) {
    int min_val = INT_MAX;
    for (int val : arr) {
        if (val < min_val) min_val = val;
    }
    cout << min_val;
}

void p_min(const vector<int>& arr) {
    int min_val = INT_MAX;
#pragma omp parallel for reduction(min : min_val) num_threads(16)
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] < min_val) min_val = arr[i];
    }
    cout << min_val;
}

string bench_traverse(function<void()> traverse_fn) {
    auto start = high_resolution_clock::now();
    traverse_fn();
    cout << " (";
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    return to_string(duration.count());
}

int main() {
    int n;
    cout << "Enter array size: ";
    cin >> n;

    vector<int> a(n);
    int choice;

    cout << "Choose input type:\n1. Random Input\n2. Manual Input\nEnter your choice (1 or 2): ";
    cin >> choice;

    if (choice == 1) {
        int rand_max;
        cout << "Enter maximum random value: ";
        cin >> rand_max;
        srand(time(NULL));
        for (int i = 0; i < n; i++) {
            a[i] = rand() % rand_max;
        }
    } else if (choice == 2) {
        cout << "Enter " << n << " elements:\n";
        for (int i = 0; i < n; i++) {
            cin >> a[i];
        }
    } else {
        cout << "Invalid choice. Exiting.\n";
        return 1;
    }

    cout << "\nArray Elements:\n";
    for (int val : a) cout << val << " ";
    cout << "\n\n";

    omp_set_num_threads(16);

    cout << "Sequential Min: " << bench_traverse([&] { s_min(a); }) << "ms)\n";
    cout << "Parallel (16) Min: " << bench_traverse([&] { p_min(a); }) << "ms)\n\n";

    cout << "Sequential Max: " << bench_traverse([&] { s_max(a); }) << "ms)\n";
    cout << "Parallel (16) Max: " << bench_traverse([&] { p_max(a); }) << "ms)\n\n";

    cout << "Sequential Sum: " << bench_traverse([&] { s_sum(a); }) << "ms)\n";
    cout << "Parallel (16) Sum: " << bench_traverse([&] { p_sum(a); }) << "ms)\n\n";

    cout << "Sequential Average: " << bench_traverse([&] { s_avg(a); }) << "ms)\n";
    cout << "Parallel (16) Average: " << bench_traverse([&] { p_avg(a); }) << "ms)\n";

    return 0;
}
