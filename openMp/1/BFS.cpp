#include <iostream>
#include <vector>
#include <queue>
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

    void iterativeBFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            cout << u << " ";

            for (int v : adj[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }

        cout << endl;
    }

    void parallelBFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int level_size = q.size();
            vector<int> current_level;

            // Extract current level nodes (sequential to preserve BFS order)
            for (int i = 0; i < level_size; ++i) {
                current_level.push_back(q.front());
                q.pop();
            }

            // Parallel processing of the current level nodes
            #pragma omp parallel for
            for (int i = 0; i < level_size; ++i) {
                int u = current_level[i];

                // Critical section to print in correct order
                #pragma omp critical
                cout << u << " ";

                // Critical section for pushing unvisited nodes into the queue
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        #pragma omp critical
                        {
                            if (!visited[v]) {
                                visited[v] = true;
                                q.push(v);
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
    
    cout<<endl<<endl;

    // Iterative BFS
    cout << "Iterative BFS starting from node 0:\n";
    double start_time = omp_get_wtime();
    g.iterativeBFS(0);
    double end_time = omp_get_wtime();
    cout << "Time taken (Iterative BFS): " << (end_time - start_time) << " seconds\n\n";

    // Parallel BFS
    cout << "Parallel BFS starting from node 0:\n";
    start_time = omp_get_wtime();
    g.parallelBFS(0);
    end_time = omp_get_wtime();
    cout << "Time taken (Parallel BFS): " << (end_time - start_time) << " seconds\n";

    return 0;
}
