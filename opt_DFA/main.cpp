#include<iostream>
#include<vector>
#include<fstream>
#include<stack>
#include<utility>
#include<unordered_set>

using namespace std;

vector<vector<char>> merge_states(vector<vector<char>> T) {
    vector<vector<char>> M;
    stack<pair<vector<char>, int>> L;
    vector<char> accepting;
    vector<char> non_accepting;
    for (unsigned int i = 0; i < T.size(); i++) {
        if (T.at(i).at(0) == '-') {
            non_accepting.push_back(T.at(i).at(1));
        } else {
            accepting.push_back(T.at(i).at(1));
        }
    }
    L.push(make_pair(accepting, T.at(0).size()));
    L.push(make_pair(non_accepting, T.at(0).size()));
    while (!L.empty()) {
        unordered_set<char> states;
        vector<vector<char>> x;
        pair<vector<char>, int> next = L.top();
        L.pop();
        x.resize(T.size() + 1);
        for (unsigned int i = 0; i < next.first.size(); i++) {
            if ((char)T.at((int)(next.first.at(i))-48).at(next.second) == 'E') {
                x.at(x.size() - 1).push_back(next.first.at(i));
            } else {
                x.at((int)(next.first.at(i))-48).push_back(next.first.at(i));
            }
        }
        for (unsigned int i = 0; i < x.size(); i++) {
            if (!x.at(i).empty()) {
                if (next.second == 2) {
                    M.push_back(x.at(i));
                }else {
                    L.push(make_pair(x.at(i), next.second - 1));
                }
                
            }
        }
    }
    vector<vector<char>> R(T);
    for (unsigned int i = 0; i < M.size(); i++) {
        for (unsigned int j = 1; j < M.at(i).size(); j++) {
            for (unsigned int k = 0; k < T.size(); k++) {
                for (unsigned int l = 0; l < T.at(0).size(); l++) {
                    if (T.at(k).at(l) == M.at(i).at(j)) {
                        R.at(k).at(l) = M.at(i).at(0);
                    }
                }
                
            }
            R.erase((int)M.at(i).at(j) - 48);
        }
        
    }
    return R;
}

int main(int argc, char *argv[]) {
    vector<vector<char>> NFA;
    if (argc > 1) {
        ifstream my_file(argv[1]);
        char accept;
        my_file >> accept;
        while (!my_file.eof()) {
            vector<char> row;
            row.push_back(accept);
            my_file >> accept;
            while (accept != '-' && accept != '+' && !my_file.eof()) {
                row.push_back(accept);
                my_file >> accept;
            }
            NFA.push_back(row);
        }
    }

    // for (unsigned int i = 0; i < NFA.size(); i++) {
    //     for (unsigned int j = 0; j < NFA.at(i).size(); j++) {
    //         cout << NFA.at(i).at(j);
    //     }
    //     cout << endl;
    // }
    return 0;
}