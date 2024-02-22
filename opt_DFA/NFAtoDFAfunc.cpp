#include "NFAtoDFAfunc.h"

string setToString(set<string> a) {
    string b = "";
    for (auto c:a) {
        b += c;
        b += " ";
    }
    return b;
}

void print1d(vector<string> vec) {
    for (unsigned int j = 0; j < vec.size(); j++) {
        cout << vec.at(j) << " ";
    }
    cout << endl;
}

set<string> follow_lambda(vector<vector<edge>> edges_sort, string lambda, string start, set<string> searched) {
    set<string> new_node;
    new_node.insert(start);
    searched.insert(start);
    
    for (unsigned int i = 0; i < edges_sort[stoi(start)].size(); i++) {
        for (unsigned int j = 0; j < edges_sort[stoi(start)][i].transition.size(); j++) {
            if (edges_sort[stoi(start)][i].transition[j] == lambda && searched.count(edges_sort[stoi(start)][i].to) == 0) {
                searched.insert(edges_sort[stoi(start)][i].to);
                set<string> a = follow_lambda(edges_sort, lambda, edges_sort[stoi(start)][i].to, searched);
                for (auto next:a) {
                    new_node.insert(next);
                }
                
            }
        }
        
    }
    
    return new_node;
}

set<string> follow_lambda(vector<vector<edge>> edges_sort, string lambda, set<string> node) {
    for (auto a:node) {
        for (size_t i = 0; i < edges_sort[stoi(a)].size(); i++) {
            for (unsigned int j = 0; j < edges_sort[stoi(a)][i].transition.size(); j++) {
                if (edges_sort[stoi(a)][i].transition[j] == lambda && node.count(edges_sort[stoi(a)][i].to) == 0) {
                    node.insert(edges_sort[stoi(a)][i].to);
                    set<string> next_node = follow_lambda(edges_sort, lambda, node);
                    for (auto next:next_node) {
                        node.insert(next);
                    }
                }
                 
            }
        }
    }
    return node;
}

set<string> follow_char(vector<vector<edge>> edges_sort, string trans, set<string> next) {
    set<string> following;
    for (auto a:next) {
        for (size_t i = 0; i < edges_sort[stoi(a)].size(); i++) {
            for (unsigned int j = 0; j < edges_sort[stoi(a)][i].transition.size(); j++) {
                if (edges_sort[stoi(a)][i].transition[j] == trans) {
                    following.insert(edges_sort[stoi(a)][i].to);
                }
            }
        }
    }
    return following;
}

vector<vector<string>> convertDFA(vector<edge> edges, map<string, unsigned int> alphabet, int num_state, string lambda) {
    int count = 1;
    set<string> changed;
    changed.insert("0");
    vector<edge> refined(edges);
    for (unsigned int i = 0; i < edges.size(); i++) {
        if (changed.count(edges[i].from) == 0) {
            changed.insert(edges[i].from);
            for (size_t j = 0; j < edges.size(); j++) {
                if (edges[j].from == edges[i].from) {
                    refined[j].from = to_string(count);
                }
                if (edges[j].to == edges[i].from) {
                    refined[j].to = to_string(count);
                }
            }
            count++;
        }
        
    }
    // for (unsigned int i = 0; i < edges.size(); i++) {
    //     cout << edges[i].accepting << " " << edges[i].from << " " << edges[i].to << " ";
    //     print1d(edges[i].transition);
    // }
    // cout << edges.size() << endl;
    vector<vector<edge>> edges_sort;
    
    edges_sort.resize(num_state);
    for (unsigned int i = 0; i < refined.size(); i++) {
        edges_sort[stoi(refined[i].from)].push_back(refined[i]);
    }

    set<string> node;
    node.insert("0");
    vector<vector<string>> T;
    node = follow_lambda(edges_sort, lambda, node);
    
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
                break;
            }
        }
        if (contains == false) {
            T.push_back({"-", b});
            T[T.size()-1].resize(alphabet.size()+2);
            for (unsigned int i = 2; i < T[T.size()-1].size(); i++){
                T[T.size()-1][i] = "E";
            }
        } else {
            continue;
        }
        
        map<string, unsigned int>::iterator it = alphabet.begin();
        while (it != alphabet.end()) {
            set<string> R = follow_lambda(edges_sort, lambda, follow_char(edges_sort, it->first, next));
            if (!R.empty()) {
                string d = setToString(R);
                
                T[T.size()-1][it->second + 2] = d;
                
                bool contains = false;
                for (size_t k = 0; k < T.size(); k++) {
                    if (T[k][1] == d) {
                        contains = true;
                        break;
                    }
                }
                if (contains == false) {
                    S.push(R);
                }
            }
            ++it;
        }
        
        for (auto a:next) {
            if (!edges_sort[stoi(a)].empty() && edges_sort[stoi(a)][0].accepting == "+") {
                T[T.size()-1][0] = "+";
                break;
            }
        }

    }
   
    return T;
}

vector<vector<string>> prune_unreach(vector<vector<string>> DFA) {
    stack<string> S;
    set<string> L;
    S.push("0");
    L.insert("0");
    while (!S.empty()) {
        string cur = S.top();
        S.pop();
        for (size_t i = 2; i < DFA[stoi(cur)].size(); i++) {
            if (DFA[stoi(cur)][i] != "E" && L.count(DFA[stoi(cur)][i]) == 0) {
                S.push(DFA[stoi(cur)][i]);
                L.insert(DFA[stoi(cur)][i]);
            }
        }
    }
    vector<vector<string>> reformed(DFA);
    for (size_t i = 0; i < DFA.size(); i++) {
        if (L.count(DFA[i][1]) == 0) {
            reformed.erase(reformed.begin() + i);
        }
    }
    reformed = make_sane(reformed);
    return reformed;
}

vector<vector<string>> prune_dead(vector<vector<string>> DFA) {
    stack<string> S;
    set<string> L;
    vector<vector<string>> backwards;
    backwards.resize(DFA.size());
    for (size_t i = 0; i < DFA.size(); i++) {
        for (size_t j = 2; j < DFA[i].size(); j++) {
            if (DFA[i][j] != "E") {
                backwards[stoi(DFA[i][j])].push_back(DFA[i][1]);
            }
            
        }
        if (DFA[i][0] == "+") {
            S.push(DFA[i][1]);
            L.insert(DFA[i][1]);
        }
    }
    
    while (!S.empty()) {
        string cur = S.top();
        S.pop();
        for (size_t i = 0; i < backwards[stoi(cur)].size(); i++) {
            if (L.count(backwards[stoi(cur)][i]) == 0) {
                S.push(backwards[stoi(cur)][i]);
                L.insert(backwards[stoi(cur)][i]);
            }
        }
    }

    

    vector<vector<string>> reformed;
    
    for (auto a:L) {
        reformed.push_back(DFA[stoi(a)]);
    }
    // print2d(reformed);
    for (size_t i = 0; i < reformed.size(); i++) {
        for (size_t j = 2; j < reformed[i].size(); j++) {
            if (L.count(reformed[i][j]) == 0) {
                reformed[i][j] = "E";
            }
        }
    }
    
    reformed = make_sane(reformed);
    return reformed;
}

int check_valid(vector<vector<string>> DFA, string token, map<string, unsigned int> alphabet) {
    string state = "0";
    for (size_t i = 0; i < token.length(); i++) {
        if (DFA[stoi(state)][alphabet[token.substr(i,1)] + 2] != "E") {
            state = DFA[stoi(state)][alphabet[token.substr(i,1)] + 2];
        } else {
            return i + 1;
        }
    }
    
    if (DFA[stoi(state)][0] == "+") {
        return -1;
    } else if (token == "") {
        return 0;
    } else {
        return token.length() + 1;
    }
}

void print2d(vector<vector<string>> vec) {
    for (unsigned int i = 0; i < vec.size(); i++) {
        for (unsigned int j = 0; j < vec.at(i).size(); j++) {
            cout << vec.at(i).at(j) << ", ";
        }
        cout << endl;
    }
}

