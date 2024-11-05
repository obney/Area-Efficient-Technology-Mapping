#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <iterator>
#include <chrono>
#include <time.h>
#include "flow.h"

using namespace std;

// Struct to hold all input-related graph
struct InputData {
    int N;                         // Number of nodes
    int number_of_pi;              // Number of primary inputs
    int number_of_po;              // Number of primary outputs
    vector<vector<int>> E;         // Adjacency list for graph representation
    vector<vector<int>> reverse_E; // Reverse direction of edge in E
    vector<int> PI;                // Primary inputs
    vector<int> PO;                // Primary outputs
};

// Function to read input from file into InputData struct
bool read_input(const string& inputFilePath, InputData& graph) {
    ifstream inFile(inputFilePath);
    if (!inFile) {
        cerr << "Error: Could not open input file." << endl;
        return false;
    }

    string line;
    getline(inFile, line);
    istringstream iss(line);
    string name;
    iss >> name >> graph.N >> graph.number_of_pi >> graph.number_of_po;

    graph.E.resize(graph.N + 1);
    graph.reverse_E.resize(graph.N + 1);
    graph.PI.resize(graph.number_of_pi);
    graph.PO.resize(graph.number_of_po);

    for (int i = 0; i < graph.number_of_pi; i++) {
        getline(inFile, line);
        istringstream iss(line);
        iss >> graph.PI[i];
    }

    for (int i = 0; i < graph.number_of_po; i++) {
        getline(inFile, line);
        istringstream iss(line);
        iss >> graph.PO[i];
    }

    while (getline(inFile, line)) {
        istringstream iss(line);
        int node;
        iss >> node;
        int id;
        while (iss >> id) {
            graph.E[node].push_back(id);
            graph.reverse_E[id].push_back(node);
        }
    }
    inFile.close();
    return true;
}

// Function to process nodes based on the graph structure and create "cones"
void process_nodes(const InputData& graph, int K, vector<string>& output) {
    vector<int> label(graph.N + 1), deg(graph.N + 1), subgraph_size(graph.N + 1);
    vector<vector<int>> k_lut(graph.N + 1, vector<int>());



    //PHASE1 LABELING
    //calulate labels in topological order
    queue<int> q;

    //base case
    for(auto& node: graph.PI) {
        subgraph_size[node] = 1;
        for(auto& to: graph.reverse_E[node]) {
            deg[to]++;
            if(deg[to] == graph.E[to].size()) {
                label[to] = 1;
                subgraph_size[to] = graph.E[to].size() + 1;
                for(auto& to2: graph.reverse_E[to]) {
                    deg[to2]++;
                    if(deg[to2] == graph.E[to2].size()) q.push(to2);
                }
            }
        }
    }
    int cnt = 0;

    while(!q.empty()) {
        int node = q.front();
        // cout<<node<<" "<<q.size()<<'\n';
        q.pop();
        cnt++;
        if(cnt == 13) break;

        for(auto& to: graph.reverse_E[node]) {
            deg[to]++;
            if(deg[to] == graph.E[to].size()) {
                // cout<<node<<"push"<<to<<'\n';
                q.push(to);
            }
        }

        int p = 0, cnt = 2;
        queue<int> bfs;
        vector<bool> vis(graph.N + 1);
        vector<int> discrete(graph.N + 1), map(2*graph.N + 10);
        vis[node] = discrete[node] = 1;

        subgraph_size[node] = 1;
        for(auto& child: graph.E[node]) {
            p = max(p, label[child]);
            subgraph_size[node] += subgraph_size[child];
        }

        

        Graph_FlowNetWorks net(2*subgraph_size[node] + 10);

        bfs.push(node);

        while(!bfs.empty()) {
            int cur = bfs.front();
            bfs.pop();

            for(auto& child: graph.E[cur]) {
                if(!vis[child]) {
                    vis[child] = 1;
                    bfs.push(child);
                    if(label[child] == p) {
                        discrete[child] = 1;
                        continue;
                    }
                    discrete[child] = cnt;
                    map[cnt] = child;
                    cnt += 2;
                }
                if(label[child] == p) continue;
                net.AddEdge(discrete[child], discrete[child] + 1, 1);
                net.AddEdge(discrete[child] + 1, discrete[cur], 1e9);
            }

            if(graph.E[cur].size() == 0) {
                net.AddEdge(0, discrete[cur], 1e9);
            }
        }

        vector<int> cut;
        
        net.FordFulkerson(0, 1, cut);
        // for(int i = 0; i < map.size(); i++) {
        //     if(map[i] != 0) {
        //         cout<<map[i]<<" "<<i<<'\n';
        //     }
        // }
        // cout<<"cut";
        // for(auto& c: cut) {
        //     cout<<map[c]<<" ";
        //     // k_lut[node].push_back(map[c]);
        // }
        // cout<<"\n\n";

        if(cut.size() > K) {
            label[node] = p + 1;
            for(auto& child: graph.E[node])
                k_lut[node].push_back(child);
        }
        else {
            label[node] = p;
            for(auto& c: cut) {
                k_lut[node].push_back(map[c]);
            }
        }
        // cout<<node<<" "<<label[node]<<'\n';


    }
}

// Function to write output to the specified file
bool write_output(const string& outputFilePath, const vector<string>& output) {
    ofstream outFile(outputFilePath);
    if (!outFile) {
        cerr << "Error: Could not open output file." << endl;
        return false;
    }

    for (const auto& line : output) {
        outFile << line << endl;
    }
    outFile.close();
    return true;
}

int main(int argc, char* argv[]) {
    // Check for correct number of command-line arguments
    if (argc != 4) {
        cerr << "Usage: ./mapper <input file path> <output file path> <K>" << endl;
        return 1;
    }

    // Parse command-line arguments
    string inputFilePath = argv[1];
    string outputFilePath = argv[2];
    int K = stoi(argv[3]);

    // Prepare input graph structure and output storage
    InputData graph;
    vector<string> output;


    // Read input graph
    if (!read_input(inputFilePath, graph)) {
        return 1;
    }

    
    process_nodes(graph, K, output);
     
    // Write output to file
    // if (!write_output(outputFilePath, output)) {
    //     return 1;
    // }

    cout << "Processing complete. Output written to " << outputFilePath << endl;
    return 0;
}
