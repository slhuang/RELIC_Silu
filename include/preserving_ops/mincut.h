#pragma
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <queue>
#include <climits>
#include <unordered_set>
using namespace std;

bool bfs(vector<unordered_map<int, int>> &rGraph, int s, int t, vector<int> &parent);
void dfs(vector<unordered_map<int, int>> &rGraph, int s, vector<bool> &visited, int &reached);
vector<bool> minCut(vector<unordered_map<int, int>> rGraph, int s, int t);