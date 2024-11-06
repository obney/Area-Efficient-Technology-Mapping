#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <time.h>

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

void check_redundance(const InputData& graph, set<int>& cut) {
    bool flag = true;
    while(flag) {
        flag = false;
        set<int> to_be_erase;
        set<int> to_be_insert;

        for(auto& c:cut) {
            if(graph.E[c].size() == 0) continue;
            int child1 = graph.E[c][0], child2 = graph.E[c][1];
            bool r1 = cut.count(child1), r2 = cut.count(child2);
            if(r1 || r2) {
                flag = true;
                to_be_erase.insert(c);
                if(!r1) to_be_insert.insert(child1);
                if(!r2) to_be_insert.insert(child2);
            }
        }
        for(auto& v:to_be_insert) cut.insert(v);
        for(auto& v:to_be_erase) cut.erase(v);
    }
}

// Function to label nodes
void label(const InputData& graph, int K, vector<set<int>>& opt_k_lut, vector<set<set<int>>>& feasible_cuts) {
    vector<int> deg(graph.N + 1);
    vector<double> label(graph.N + 1, 1e9);
    
    //calulate optimal cut of node in topological order
    queue<int> q;

    //base case
    for(auto& node: graph.PI) {
        label[node] = 0;
        for(auto& to: graph.reverse_E[node]) {
            deg[to]++;
            if(deg[to] == graph.E[to].size()) {
                set<int> cut;
                for(auto& child: graph.E[to]) cut.insert(child);
                label[to] = 1;
                feasible_cuts[to].insert(cut);
                opt_k_lut[to] = cut;
                for(auto& to2: graph.reverse_E[to]) {
                    deg[to2]++;
                    if(deg[to2] == graph.E[to2].size()) q.push(to2);
                }
            }
        }
    }

    while(!q.empty()) {
        int node = q.front();
        // cout<<node<<" "<<q.size()<<'\n';
        q.pop();

        for(auto& to: graph.reverse_E[node]) {
            deg[to]++;
            if(deg[to] == graph.E[to].size()) {
                // cout<<node<<"push"<<to<<'\n';
                q.push(to);
            }
        }

        feasible_cuts[node].insert({graph.E[node][0], graph.E[node][1]});
        
        for(auto& fc: feasible_cuts[graph.E[node][0]]) {
            set<int> cut({graph.E[node][1]});
            for(auto& v: fc) cut.insert(v);
            check_redundance(graph, cut);
            if(cut.size() <= K) feasible_cuts[node].insert(cut);
        }

        for(auto& fc: feasible_cuts[graph.E[node][1]]) {
            set<int> cut({graph.E[node][0]});
            for(auto& v: fc) cut.insert(v);
            check_redundance(graph, cut);
            if(cut.size() <= K) feasible_cuts[node].insert(cut);
        }

        for(auto& fc0: feasible_cuts[graph.E[node][0]]) {
            for(auto& fc1: feasible_cuts[graph.E[node][1]]) {
                set<int> cut(fc0);
                for(auto& v: fc1) cut.insert(v);
                check_redundance(graph, cut);
                if(cut.size() <= K) feasible_cuts[node].insert(cut);
            }
        }
        
        for(auto& cut: feasible_cuts[node]) {
            double cost = 1.0;
            for(auto& v: cut) {
                // cout<<v<<" ";
                cost += label[v]/(double)max(1, (int)graph.reverse_E[v].size());
            }
            // cout<<'\n';
            if(cost < label[node]) {
                label[node] = cost;
                opt_k_lut[node] = cut;
            }
        }
        // cout<<"Label:"<<node<<" "<<label[node]<<"\n\n";
    }
}

void mapping(const InputData& graph, int K, vector<set<int>>& opt_k_lut, vector<string>& output) {
    stack<int> stk;
    vector<bool> vis(graph.N + 1);
    for(auto& node: graph.PO) stk.push(node);

    while(!stk.empty()) {
        int cur = stk.top();
        stk.pop();

        ostringstream oss;
        oss << cur;
        // cout<<cur<<": ";
        // check_redundance(graph, opt_k_lut[cur]);
        for(auto& in: opt_k_lut[cur]) {
            if(!vis[in] && graph.E[in].size() > 0) {
                vis[in] = 1;
                stk.push(in);
            }
            oss << " " << in;
            // cout<<" "<<in;
        }
        // cout<<'\n';

        output.push_back(oss.str());
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
    // Start timing
    auto start = chrono::steady_clock::now();


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

    //phase 1: label the node
    vector<set<int>> opt_k_lut(graph.N + 1, set<int>());
    vector<set<set<int>>> feasible_cuts(graph.N + 1);
    label(graph, K, opt_k_lut, feasible_cuts);

    //phase 2: mapping
    mapping(graph, K, opt_k_lut, output);
    
    // for(auto& out: graph.PO) {
    //     cout<<out<<": "<<feasible_cuts[out].size()<<'\n';
    // }

    //Write output to file
    if (!write_output(outputFilePath, output)) {
        return 1;
    }

    cout << "Processing complete. Output written to " << outputFilePath << endl;

    // End timing
    auto end = chrono::steady_clock::now();

    // Calculate and display the elapsed time in milliseconds
    chrono::duration<double, std::milli> elapsed = end - start;
    cout << "Program runtime: " << elapsed.count() << " ms" << std::endl;
    return 0;
}
