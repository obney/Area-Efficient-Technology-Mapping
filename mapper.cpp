#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <set>
#include "flow.h"

using namespace std;


int main(int argc, char* argv[]) {
    // Check if arguments are correctly provided
    if (argc != 4) {
        cerr << "Usage: ./mapper <input file path> <output file path> <K>" << endl;
        return 1;
    }

    // Parse command line arguments
    string inputFilePath = argv[1];
    string outputFilePath = argv[2];
    int K = stoi(argv[3]), effect = 0;

    // Open input file
    ifstream inFile(inputFilePath);
    if (!inFile) {
        cerr << "Error: Could not open input file." << endl;
        return 1;
    }

    // Read data from input file
    vector<string> data;
    string line;
    getline(inFile, line);
    istringstream iss(line);
    string name;
    int N, number_of_pi, number_of_po;
    iss >> name;
    iss >> N >> number_of_pi >> number_of_po;

    vector<vector<int>> e(N + 1);
    vector<int> PI(number_of_pi), PO(number_of_po);

    for(int i = 0; i < number_of_pi; i++) {
        getline(inFile, line);
        istringstream iss(line);
        iss >> PI[i];
    }

    for(int i = 0; i < number_of_po; i++) {
        getline(inFile, line);
        istringstream iss(line);
        iss.str(line);
        iss >> PO[i];
    }

    while(getline(inFile, line)) {
        istringstream iss(line);
        int node;
        iss >> node;
        int id;
        while(iss >> id) {
            e[node].push_back(id);
        }
    }
    inFile.close();

    queue<int> q;
    vector<bool> ot(N + 1);
    for(int i = 0; i < PO.size(); i++) {
        q.push(PO[i]);
    }
 

    while(!q.empty()) {
        int node = q.front();
        q.pop();
        if(ot[node]) continue;
        ot[node] = 1;
        // first find the cut of size K

        //bdy contains non-leaf nodes
        set<int> bdy;
        //good contains leaf nodes
        set<int> good;

        for(int nei:e[node]) {
            if(ot[nei] || e[nei].size() == 0)
                good.insert(nei);
            else
                bdy.insert(nei);
        }


        // if the number of input node is not constant 2, then here need to be revised.
        vector<bool> dul(N + 1);
        while(bdy.size() < K - good.size() && bdy.size() > 0) {
            auto it = bdy.begin();
            int m = *it;
            dul[m] = 1;
            for(int nei: e[m]) {
                if(dul[nei]) continue;
                if(ot[nei] || e[nei].size() == 0)
                    good.insert(nei);
                else
                    bdy.insert(nei);
            }
            bdy.erase(m);
        }


        //find the subgraph of node from bdy.

        if(bdy.size() > 0) {
            if(node == 739) {
                for(auto v: bdy) cout<<v<<" ";
                cout<<'\n';
                for(auto v: good) cout<<v<<" ";
                cout<<'\n';
                cout<<"hahaha\n";
            }
            vector<int> vis(N + 1);
            vector<int> rec;
            queue<int> bfs;
            for(int v:bdy) {
                bfs.push(v);
                vis[v] = 1;
            }
            while(!bfs.empty()) {
                int t = bfs.front();
                rec.push_back(t);
                // cout<<t<<'\n';
                bfs.pop();
                if(ot[t] || e[t].size() == 0) {
                    continue;
                }
                for(int i = 0; i < e[t].size(); i++) {
                    if(vis[e[t][i]] == 1) continue;
                    vis[e[t][i]] = 1;
                    bfs.push(e[t][i]);
                }
            }
            // ot[node] = 1;
            sort(rec.begin(), rec.end());
            unordered_map<int, int> map;
            // cout<<rec.size()<<'\n';
            Graph_FlowNetWorks g(2 * rec.size() + 2);

            for(int i = 0; i < rec.size(); i++)
                map[rec[i]] = i + 1;
            
            for(int i = 0; i < rec.size(); i++) {
                if(ot[rec[i]] || e[rec[i]].size() == 0) {
                    if(node == 739) {
                        cout<<"pri"<<rec[i]<<" "<<ot[rec[i]]<<'\n';
                    }
                    g.AddEdge(0, i + 1, 1e6);
                    g.AddEdge(i + 1, i + 1 + rec.size(), 1);
                }
                else if(bdy.find(rec[i]) != bdy.end()) {
                    for(auto nei:e[rec[i]]) {
                        g.AddEdge(map[nei] + rec.size(), i + 1, 1e6);
                        g.AddEdge(i + 1, i + 1 + rec.size(), 1);
                        g.AddEdge(i + 1 + rec.size(), 2*rec.size() + 1, 1e6);
                    }
                }
                else {
                    for(auto nei:e[rec[i]]) {
                        g.AddEdge(map[nei] + rec.size(), i + 1, 1e6);
                        g.AddEdge(i + 1, i + 1 + rec.size(), 1);
                    }
                }
            }

            vector<int> cut;
            g.FordFulkerson(0, 2*rec.size() + 1, cut);

            set<int> bdy2;
        
            for(auto num: cut) {
                bdy2.insert(rec[num - 1]);
                if(node == 739) {
                    cout<<rec[num - 1]<<"\n";
                }
            }

            if(bdy2 != bdy) {
                effect += 1;
                bdy.clear();
                for(auto v: bdy2) {
                    if(ot[v] || e[v].size() == 0) {
                        good.insert(v);
                    }
                    else {
                        bdy.insert(v);
                    }
                }
            }

        }

        
        vector<bool> dul2(N + 1);

        while(bdy.size() < K - good.size() && bdy.size() > 0) {
            auto it = bdy.begin();
            int m = *it;
            dul2[m] = 1;
            for(int nei: e[m]) {
                if(dul2[nei]) continue;
                if(ot[nei] || e[nei].size() == 0)
                    good.insert(nei);
                else
                    bdy.insert(nei);
            }
            bdy.erase(m);
        }
        string cone;
        std::ostringstream oss;
        oss<<node;

        for(auto i:good) {
            cout<<i<<'\n';
            oss<<" "<<i;
        }
        for(auto i:bdy) {
            cout<<i<<'\n';
            q.push(i);
            oss<<" "<<i;
        }
        cone = oss.str();
        data.push_back(cone);
        cout<<"-------\n";
    }
    

    // Open output file
    ofstream outFile(outputFilePath);
    if (!outFile) {
        cerr << "Error: Could not open output file." << endl;
        return 1;
    }


    for (const auto& line : data) {
        outFile << line << endl;
    }

    outFile.close();
    cout<<effect<<'\n';
    cout << "Processing complete. Output written to " << outputFilePath << endl;

    return 0;
}
