#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <set>

using namespace std;

// Struct to hold all input-related graph
struct InputData {
    int N;                       // Number of nodes
    int number_of_pi;            // Number of primary inputs
    int number_of_po;            // Number of primary outputs
    vector<vector<int>> E;       // Adjacency list for graph representation
    vector<int> PI;              // Primary inputs
    vector<int> PO;              // Primary outputs
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
        }
    }
    inFile.close();
    return true;
}

// Function to process nodes based on the graph structure and create "cones"
void process_nodes(const InputData& graph, int K, vector<string>& output) {
    stack<int> stk;
    vector<bool> ot(graph.N + 1, false);
    
    // Initialize queue with primary output nodes
    for (int node : graph.PO) {
        stk.push(node);
    }

    while (!stk.empty()) {
        int node = stk.top();
        stk.pop();
        if (ot[node] || graph.E[node].empty()) continue;
        ot[node] = true;

        set<int> bdy, good;
        vector<bool> dul(graph.N + 1, false);
        bdy.insert(node);

        // Adjust boundary set to ensure the cut size does not exceed K
        while (!bdy.empty()) {
            auto it = bdy.begin();
            int m = *it;
            dul[m] = true;
            set<int> t1(bdy), t2(good);
            for (int nei : graph.E[m]) {
                if (dul[nei]) continue;
                if (ot[nei] || graph.E[nei].empty())
                    t2.insert(nei);
                else
                    t1.insert(nei);
            }
            t1.erase(m);
            
            if(t1.size() + t2.size() > K) break;
            bdy = t1;
            good = t2;
        }

        // Assemble the cone for the current node
        ostringstream oss;
        oss << node;
        for (int i : good) oss << " " << i;
        for (int i : bdy) {
            stk.push(i);
            oss << " " << i;
        }
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

    // Process nodes and generate output graph
    process_nodes(graph, K, output);

    // Write output to file
    if (!write_output(outputFilePath, output)) {
        return 1;
    }

    cout << "Processing complete. Output written to " << outputFilePath << endl;
    return 0;
}
