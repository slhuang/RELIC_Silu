#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include "../../include/profiling/schema_matching.h"

using namespace std;

struct triple {
    int ridA, ridB, cnt;
    triple(int _ridA, int _ridB, int _cnt) {
        ridA = _ridA;
        ridB = _ridB;
        cnt = _cnt;
    }
};
class vp_row_matching { //version pair
    vector<vector<string>> A, B;
    // number of cells in common between rows in A and B
    vector<vector<int>> common;
    unordered_map<int, int> row_A2B, col_g2l_B;
    int common_cells;
public:
    vp_row_matching(vector<vector<string>>& _A, vector<vector<string>>& _B, unordered_map<int, int>& _col_g2l_B) {
        A = _A;
        B = _B;
        col_g2l_B = _col_g2l_B;
        common_cells = 0;
        int rowA_cnt = A.size() - 1, rowB_cnt = B.size() - 1; // the first row maintains global_column_id
        common.resize(rowA_cnt);
        for (auto& ele : common) ele.resize(rowB_cnt, 0);
    }

    void construct_r2r_similarity();
    void print_r2r_similarity();
    void r2r_correspondence_greedy();
    int return_largest_common_cells() {return common_cells;}
};