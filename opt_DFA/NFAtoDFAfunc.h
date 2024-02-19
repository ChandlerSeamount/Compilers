#ifndef NFATODFAFUNC_H
#define NFATODFAFUNC_H

#include <iostream>
#include <vector>
#include <stack>
#include <utility>
#include <string>
#include <map>
#include <set>
#include "merge_states.h"

using namespace std;

struct edge {
    string accepting;
    string from;
    string to;
    vector<string> transition;
};

vector<vector<string>> convertDFA(vector<edge> edges, map<string, unsigned int> alphabet, int num_state, string lambda);

set<string> follow_lambda(vector<vector<edge>> edges_sort, string lambda, string start, set<string> searched);

vector<vector<string>> prune_unreach(vector<vector<string>> DFA);

vector<vector<string>> prune_dead(vector<vector<string>> DFA);

int check_valid(vector<vector<string>> DFA, string token, map<string, unsigned int> alphabet);

#endif