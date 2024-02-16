#include<iostream>
#include<vector>
#include<fstream>
#include<stack>
#include<utility>
#include<unordered_set>
#include "merge_states.h"

using namespace std;

void print2d(vector<vector<string>> vec) {
    for (unsigned int i = 0; i < vec.size(); i++) {
        for (unsigned int j = 0; j < vec.at(i).size(); j++) {
            cout << vec.at(i).at(j) << " ";
        }
        cout << endl;
    }
}

int main(int argc, char *argv[]) {
    vector<vector<string>> NFA;
    if (argc > 1) {
        ifstream my_file(argv[1]);
        string accept;
        my_file >> accept;
        while (!my_file.eof()) {
            vector<string> row;
            row.push_back(accept);
            my_file >> accept;
            while (accept != "-" && accept != "+" && !my_file.eof()) {
                row.push_back(accept);
                my_file >> accept;
                
            }
            if (my_file.eof()) {
                row.push_back(accept);
            }
            
            NFA.push_back(row);
        }
    }

    vector<vector<string>> DFA = merge_states(NFA);
    unsigned int size1;
    do {
        size1 = DFA.size();
        DFA = merge_states(DFA);
    } while (size1 != DFA.size());
    print2d(DFA);

    
    return 0;
}