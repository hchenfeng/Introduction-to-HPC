// CIS 677 High Performance Computing
// Chenfeng Hao
// Project 1
//

#include<iostream>
#include<vector>
#include<list>
#include<queue>
#include <fstream>

using namespace std;
#define INF 0x3f3f3f3f

// iPair ==> Integer Pair
typedef pair<int, int> iPair;

// To add an edge
// Vector index u is the source number
// Vector pair {(v, wt)} represents target node v and weight wt
void addEdge(vector<pair<int, int> > adj[], int u,
             int v, int wt) {
    // directed graph
    adj[u].emplace_back(v, wt);
}

// Get the shortest path from the parent array
list<int> getShortestPath(int V, const int *parent) {
    // V is the final target node number
    // -1 because t is used as index
    int t = V - 1;
    list<int> l;

    while (parent[t] != -1) {
        int temp;
        // Get the correct node number
        temp = t + 1;
        // Insert node number to list
        l.push_front(temp);
        t = parent[t];
    }
    // Add source number to list
    l.push_front(1);
    return l;
}

// A rather ad hoc way to print the path
void printPath(list<int> const &input) {
    // Use count for not adding , at the end
    int count = 0;
    cout << "Path: " << "[";
    for (int i: input) {
        count++;
        if (count == input.size()) cout << i;
        else cout << i << ", ";
    }
    cout << "]\n";
}


// Find the shortest paths from src to all other vertices
void shortestPath(vector<pair<int, int> > adj[], int V, int src) {
    // Create a priority queue to store vertices that
    // are being preprocessed.
    priority_queue<iPair, vector<iPair>, greater<> > pq;

    // Create a vector for distances and initialize all
    // distances as infinite (INF)
    vector<int> dist(V, INF);

    // Create an array for keeping note of node parent
    int parent[V];
    // Set the value of node 1 to -1 for stop condition
    parent[0] = -1;

    // Insert source to priority queue and initialize
    // its distance as 0.
    pq.push(make_pair(0, src));
    dist[src] = 0;

    // Looping till priority queue becomes empty
    while (!pq.empty()) {
        // The first vertex in pair is the minimum distance
        // vertex, extract it from priority queue.
        // vertex label is stored in second of pair
        int u = pq.top().second;
        pq.pop();

        // Get all adjacent of u.
        for (auto x: adj[u]) {
            // Get vertex label and weight of current adjacent
            // of u.
            int v = x.first;
            int weight = x.second;

            // If there is shorter path to v through u
            if (dist[v] > dist[u] + weight) {
                // Update the distance of v
                dist[v] = dist[u] + weight;
                // Insert v to pq
                pq.push(make_pair(dist[v], v));
                // update the parent of v
                parent[v] = u;
            }
        }
    }

    list<int> l = getShortestPath(V, parent);
    printPath(l);

    printf("Cost: %d\n", dist[V - 1]);

}

int main() {

    string file_name;
    int destination_number;

    cout << "Enter graph file name: ";
    cin >> file_name;

    cout << "Enter destination number: ";
    cin >> destination_number;

    int V = destination_number;
    vector<iPair> adj[V];
    int a, b, c;

    // Get nodes, vertices, weights from file
    // In the file, on each line the three space separated numbers
    // represent source node, destination node, and weight
    ifstream graph1_file(file_name);

    while (graph1_file >> a >> b >> c) {
        addEdge(adj, a - 1, b - 1, c);
    }

    graph1_file.close();

    printf("Filename: %s\n", file_name.c_str());
    printf("Source: %d\n", 1);
    printf("Sink: %d\n", V);

    shortestPath(adj, V, 0);

    return 0;
}