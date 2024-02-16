#include "NFAtoDFAfunc.h"

string setToString(set<string> a) {
    string b = "";
    for (auto c:a) {
        b += c;
    }
    return b;
}

set<string> follow_lambda(vector<vector<edge>> edges_sort, string lambda, string start, set<string> searched) {
    set<string> new_node;
    new_node.insert(start);
    searched.insert(start);
    
    for (unsigned int i = 0; i < edges_sort[stoi(start)].size(); i++) {
        for (unsigned int j = 0; j < edges_sort[stoi(start)][i].transition.size(); j++) {
            if (edges_sort[stoi(start)][i].transition[j] == lambda && searched.count(edges_sort[stoi(start)][i].to) == 0) {
                set<string> a = follow_lambda(edges_sort, lambda, edges_sort[stoi(start)][i].to, searched);
                for (auto next:a) {
                    new_node.insert(next);
                }
                
            }
        }
        
    }
    
    return new_node;
}

vector<vector<string>> convertDFA(vector<edge> edges, map<string, unsigned int> alphabet, int num_state, string lambda) {
    vector<vector<edge>> edges_sort;
    edges_sort.resize(num_state);
    for (unsigned int i =0; i < edges.size(); i++) {
        edges_sort[stoi(edges[i].from)].push_back(edges[i]);
    }

    set<string> node;
    vector<vector<string>> T;
    node = follow_lambda(edges_sort, lambda, "0", node);
    
    stack<set<string>> S;
    S.push(node);
    while(!S.empty()) {
        set<string> next = S.top();
        S.pop();
        string b = setToString(next);
        bool contains = false;
        for (size_t k = 0; k < T.size(); k++) {
            if (T[k][1] == b) {
                contains = true;
                continue;
            }
        }
        // cout << b << endl;
        T.push_back({"-", b});
        T[T.size()-1].resize(alphabet.size()+2);
        for (unsigned int i = 2; i < T[T.size()-1].size(); i++){
            T[T.size()-1][i] = "E";
        }
        for (auto a:next) {
            if (edges_sort[stoi(a)][0].accepting == "+") {
                T[T.size()-1][0] = "+";
            }
            for (unsigned int i = 0; i < edges_sort[stoi(a)].size(); i++) {
                for (unsigned int j = 0; j < edges_sort[stoi(a)][i].transition.size(); j++) {
                    if (edges_sort[stoi(a)][i].transition[j] != lambda) {
                        set<string> e = follow_lambda(edges_sort, lambda, edges_sort[stoi(a)][i].to, set<string>{});
                        string d = setToString(e);
                        T[T.size()-1][alphabet[edges_sort[stoi(a)][i].transition[j]] + 2] = d;
                        bool contains = false;
                        for (size_t k = 0; k < T.size(); k++) {
                            if (T[k][1] == d) {
                                contains = true;
                                break;
                            }
                        }
                        if (contains == false) {
                            S.push(e);
                        }
                    }
                }
            }
        }
    }
   
    return T;
}