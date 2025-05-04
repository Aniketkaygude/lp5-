#include <iostream>
#include <vector>
#include <stack>
#include <omp.h>
#include <unordered_map>

using namespace std;

class Graph {
    int V;
    unordered_map<int, vector<int>> adj;

public:
    Graph(int V) {
        this->V = V;
    }

    void addEdge(int v, int w) {
        adj[v].push_back(w);
        adj[w].push_back(v); // Undirected graph
    }

    void display() {
        for (auto i : adj) {
            cout << i.first << "  -->  ";
            for (auto j : i.second) {
                cout << j << " ";
            }
            cout << endl;
        }
    }

    // Iterative DFS using stack
    void iterativeDFS(int start) {
        vector<bool> visited(V, false);
        stack<int> s;
        s.push(start);

        while (!s.empty()) {
            int u = s.top();
            s.pop();

            if (!visited[u]) {
                visited[u] = true;
                cout << u << " ";
            }

            // Push unvisited neighbors to the stack
            for (int v : adj[u]) {
                if (!visited[v]) {
                    s.push(v);
                }
            }
        }

        cout << endl;
    }

    // Recursive DFS
    void recursiveDFS(int start) {
        vector<bool> visited(V, false);
        recursiveDFSHelper(start, visited);
        cout << endl;
    }

    void recursiveDFSHelper(int u, vector<bool>& visited) {
        visited[u] = true;
        cout << u << " ";

        for (int v : adj[u]) {
            if (!visited[v]) {
                recursiveDFSHelper(v, visited);
            }
        }
    }

    // Parallel DFS using OpenMP
    void parallelDFS(int start) {
        vector<bool> visited(V, false);
        stack<int> s;
        s.push(start);

        while (!s.empty()) {
            int level_size = s.size();
            vector<int> current_level;

            // Extract current level nodes
            for (int i = 0; i < level_size; ++i) {
                current_level.push_back(s.top());
                s.pop();
            }

            // Parallel processing of the current level nodes
            #pragma omp parallel for
            for (int i = 0; i < level_size; ++i) {
                int u = current_level[i];

                // Critical section to print in correct order
                #pragma omp critical
                cout << u << " ";

                // Push unvisited neighbors into the stack
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        #pragma omp critical
                        {
                            if (!visited[v]) {
                                visited[v] = true;
                                s.push(v);
                            }
                        }
                    }
                }
            }
        }

        cout << endl;
    }
};

int main() {
    int V = 8;
    Graph g(V);

    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(2, 6);
    g.addEdge(6, 7);

    cout << "ADJACENCY list --> " << endl;
    g.display();

    cout << endl << endl;

    // Iterative DFS
    cout << "Iterative DFS starting from node 0:\n";
    double start_time = omp_get_wtime();
    g.iterativeDFS(0);
    double end_time = omp_get_wtime();
    cout << "Time taken (Iterative DFS): " << (end_time - start_time) << " seconds\n\n";

    // Recursive DFS
    cout << "Recursive DFS starting from node 0:\n";
    start_time = omp_get_wtime();
    g.recursiveDFS(0);
    end_time = omp_get_wtime();
    cout << "Time taken (Recursive DFS): " << (end_time - start_time) << " seconds\n\n";

    // Parallel DFS
    cout << "Parallel DFS starting from node 0:\n";
    start_time = omp_get_wtime();
    g.parallelDFS(0);
    end_time = omp_get_wtime();
    cout << "Time taken (Parallel DFS): " << (end_time - start_time) << " seconds\n";

    return 0;
}
