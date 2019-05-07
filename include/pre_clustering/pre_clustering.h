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
using namespace std;

struct cluster {
    int id;
    vector<int> file_ids; 
    cluster(int _id, vector<int> _file_ids): id(_id), file_ids(_file_ids){}
};

class pre_clustering {
        vector<vector<int>> file_global_cids;
        unordered_map<int, string> file_id2name;
    // divide the input files into clusters
        vector<cluster> clusters; 
    public:
        pre_clustering() {}
        void load_col_matching_from_file(const string &file = "./src/profiling/schema_matching.csv");
        void cluster_with_exact_schema();
        void cluster_with_partial_schema();
        void cluster_with_pk();
        void all_in_one_cluster();
        void write_clusters_to_file(const string& file = "./src/pre_clustering/clusters.csv");
};