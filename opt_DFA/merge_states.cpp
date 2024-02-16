#include "merge_states.h"



// Simplifies the DFA
vector<vector<string>> merge_states(vector<vector<string>> T) {
    vector<vector<string>> M;
    stack<pair<vector<string>, unsigned int>> L;
    vector<string> accepting;
    vector<string> non_accepting;
    // Identify accpeting and non accepting states
    for (unsigned int i = 0; i < T.size(); i++) {
        if (T.at(i).at(0) == "-") {
            non_accepting.push_back(T.at(i).at(1));
        } else {
            accepting.push_back(T.at(i).at(1));
        }
    }
    // Push those states onto the stack with a start of one
    L.push(make_pair(accepting, 0));
    L.push(make_pair(non_accepting, 0));
    while (!L.empty()) {
        vector<string> S = L.top().first;
        unsigned int C = L.top().second;
        L.pop();
        vector<vector<string>> x;
        x.resize(T.size() + 1);
        // Partitions the elements into ones that have the same paths
        for (unsigned int i = 0; i < S.size(); i++) {
            if (T.at(stoi(S.at(i))).at(C+2) == "E") {
                x.at(x.size()-1).push_back(S.at(i));
            } else {
                x.at(stoi(T.at(stoi(S.at(i))).at(C+2))).push_back(S.at(i));
            }
        }
        // Puts those partitions either back on the stack or push them onto the map
        M.resize(T.size()+1);
        for (unsigned int i = 0; i < x.size(); i++) {
            if ((C + 3 >= T[0].size() || x.at(i).size() == 1) && x.at(i).size() > 0) {
                M.at(stoi(x.at(i).at(0))) = (x.at(i));
                
            } else if (x.at(i).size() > 0) {
                L.push(make_pair(x.at(i), C + 1));
            }
        }
    }
    // Merge the rows
    vector<vector<string>> R;
    int counter = 0;
    // Set the correct values
    for (unsigned int i = 0; i < M.size(); i++) {
        if (!M.at(i).empty()) {
            vector<string> temp = {T.at(stoi(M.at(i).at(0))).at(0), to_string(counter)};
            R.push_back(temp);
            counter++;
        
            for (unsigned int j = 0; j < M.at(i).size(); j++) {
                for (unsigned int k = 0; k < T.size(); k++) {
                    for (unsigned int l = 2; l < T.at(k).size(); l++) {
                        if (T.at(k).at(l) == M.at(i).at(j)) {
                            T.at(k).at(l) = to_string(counter-1);
                        }
                    }
                    
                }
            }
        }
        
    }
    // Assign them to the correct spaces
    counter = 0;
    for (unsigned int i = 0; i < M.size(); i++) {
        if (!M.at(i).empty()) {
            
            for (unsigned int j = 2; j < T[0].size(); j++) {
                R[counter].push_back(T[stoi(M[i][0])][j]);
            }
            counter++;
        }
    }
    return R;
}


vector<vector<string>> read_file(string filename) {
    vector<vector<string>> NFA;
    ifstream my_file(filename);
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
    return NFA;
}