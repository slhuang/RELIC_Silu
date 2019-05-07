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
#include "unionfind.h"
using namespace std;

struct col {
    string name;
    string type;
    col(string _name, string _type = "null"): name (_name), type(_type) {}
};

class schema_matching {
    unionfind synonyms;
    vector<string> filePaths;
    vector<vector<col>> fileCols; // a set of files, and a set of columns in each file
    unordered_map<string, int> globalName2Id; // global_name -> global_cid
    unordered_map<int, string> globalId2Name; // correspond to globalName2Id
    vector<unordered_map<int, int>> col_g2l, col_l2g;
  public:
    schema_matching(vector<string> _filePaths) : filePaths(_filePaths)
    {
        fileCols.resize(filePaths.size());
        col_g2l.resize(filePaths.size());
        col_l2g.resize(filePaths.size());
    }
    void initialize_synonyms(const string &file, const char &delimiter = ',');
    void load_header_and_sample_value(int sampleCnt);
    void schema_match_by_name_and_type();
    void write_mapping_to_file(const string& file = "./src/profiling/schema_matching.csv");
    vector<unordered_map<int, int>> return_col_l2g() {return col_l2g;}
};

bool is_null_string(const string &s);