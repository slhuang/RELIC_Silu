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


class row_matching
{
    vector<string> filePaths;
    vector<vector<vector<string>>> matrixes;
    vector<vector<string>> col_names; // the column names in each file
    vector<vector<int>> hyperloglog;
    bool useHLL;
    // ideally, the same pk for all files
    // not exist pk, simply pick one?
    vector<pair<int, int>> pks; // {local_cid, global_cid}
    // row correspondence
    unordered_map<string, int> globalName2Id;
    vector<unordered_map<int,int>> row_l2g, col_l2g; 
    vector<unordered_map<int, vector<int>>> row_g2l; // global_rid can be mapped to multiple local_rid 
  public:
    row_matching(vector<string>& _filePaths, vector<unordered_map<int,int>> _col_l2g, bool _useHLL = false){
        filePaths = _filePaths;
        col_l2g = _col_l2g;
        matrixes.resize(filePaths.size());
        col_names.resize(filePaths.size());
        row_g2l.resize(filePaths.size());
        row_l2g.resize(filePaths.size());
        useHLL = _useHLL; // use hyperloglog to calculate the number of distinct values
    }
    // populate $matrix
    void load_files_and_calculate_stats();
    void detect_pk_by_hyperloglog();
    void detect_pk_by_sampling(int sampleCnt = 1000);
    void row_match_by_pk_name_and_value();
    void write_matching_to_file(const string &file = "./result/row_matching.csv");
    void print_pk(const string& file = "./result/pk.csv");
};
