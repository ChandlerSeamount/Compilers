#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
#include <utility>
#include <unordered_set>
#include <map>
#include "merge_states.h"
#include "NFAtoDFAfunc.h"

using namespace std;

void print2d(vector<vector<string>> vec) {
    for (unsigned int i = 0; i < vec.size(); i++) {
        for (unsigned int j = 0; j < vec.at(i).size(); j++) {
            cout << vec.at(i).at(j) << " ";
        }
        cout << endl;
    }
}

void print1d(vector<string> vec) {
    for (unsigned int j = 0; j < vec.size(); j++) {
        cout << vec.at(j) << " ";
    }
    cout << endl;
}

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        return 1;
    }
    ifstream file_in;
    file_in.open(argv[1]);

    if (file_in.fail() || file_in.eof()) {
        return 1;
    }
    int num_state;
    string lambda;
    file_in >> num_state;
    file_in >> lambda;
    unsigned int count = 0;
    map<string, unsigned int> alphabet;
    while (file_in.peek() != '\n' && file_in.peek() != '\r') {
        string next_char;
        file_in >> next_char;
        alphabet[next_char] = count;
        count++;
    }

    vector<edge> edges;
    while (!file_in.eof()) {
        edge new_edge;
        file_in >> new_edge.accepting;
        file_in >> new_edge.from;
        file_in >> new_edge.to;
        
        while (file_in.peek() != '\n' && file_in.peek() != '\r' && !file_in.eof()) {
            string oper;
            file_in >> oper;
            new_edge.transition.push_back(oper);
        }
        edges.push_back(new_edge);
    }
    vector<vector<string>> DFA = convertDFA(edges, alphabet, num_state, lambda);
    // cout << num_state << " ";
    // print1d(alphabet);
    // for (unsigned int i = 0; i < edges.size(); i++) {
    //     cout << edges[i].accepting << " " << edges[i].from << " " << edges[i].to << " ";
    //     print1d(edges[i].transition);
    // }

    // vector<vector<string>> DFA = merge_states(NFA);
    DFA = make_sane(DFA);
    unsigned int size1;
    do {
        size1 = DFA.size();
        DFA = merge_states(DFA);
    } while (size1 != DFA.size());
    do {
        size1 = DFA.size();
        DFA = prune_unreach(DFA);
    } while (size1 != DFA.size());

    // DFA = prune_dead(DFA);

    ofstream file_out(argv[2]);
    for (unsigned int i = 0; i < DFA.size(); i++) {
        for (unsigned int j = 0; j < DFA.at(i).size(); j++) {
            file_out << DFA.at(i).at(j) << " ";
        }
        file_out << endl;
    }
    // print2d(DFA);
    // cout << argv[5] << endl;
    for (int i = 3; i < argc; i++) {
        int result = check_valid(DFA, argv[i], alphabet);
        cout << "OUTPUT ";
        if (result == -1) {
            cout << ":M:";
        } else {
            cout << result;
        }
        cout << endl;
    }
    
    return 0;
}