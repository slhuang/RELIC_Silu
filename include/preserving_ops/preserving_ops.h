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
#include "../../include/profiling/vp_row_matching.h"
#include "../../include/pre_clustering/pre_clustering.h"
#include "../../include/index/minHash.h"
#include "mincut.h"
#include "kruskal.h"
using namespace std;

struct metrics {
    double cell_jaccard;
    double col_jaccard;
    double relational; //# of instructions
    double get_metric(string metric = "cell") {
        if (metric == "rel") return cell_jaccard;
        if (metric == "col") return col_jaccard;
        return cell_jaccard;
    }
};

struct colStats {
    int global_cid;
    unordered_set<string> domain; // set of values 
};

class preserving_ops {
    vector<unordered_map<int,int>> row_l2g, col_l2g, col_g2l;
    vector<unordered_map<int,vector<int>>> row_g2l; //row_g2l,
    vector<vector<vector<string>>> matrixes;
    unordered_map<int, string> file_id2name; //fid -> file_name
    vector<string> filePaths;
    vector<vector<string>> col_names; // the column names in each file
    vector<cluster> clusters;
    vector<vector<metrics>> exact_scores; // score based on different metrics
    vector<vector<colStats>> file_columns;
    vector<vector<int>> sigs_cell;
    vector<vector<vector<int>>> sigs_col;
    bool with_pk, k_mins;
  public: 
    preserving_ops(vector<string>& _filePaths, bool _with_pk = true): filePaths(_filePaths), with_pk(_with_pk){
        int file_cnt = filePaths.size();
        matrixes.resize(file_cnt);
        col_names.resize(file_cnt);
        col_l2g.resize(file_cnt);
        col_g2l.resize(file_cnt);
        exact_scores.resize(file_cnt);
        for (auto& ele: exact_scores) ele.resize(file_cnt);
        if (with_pk) {
            row_g2l.resize(file_cnt);
            row_l2g.resize(file_cnt);
            load_row_l2g_correspondence();
        }
        load_col_g2l_correspondence();
        load_matrixes();
        load_cluster();
        k_mins = true;
    };
    preserving_ops(vector<string> &_filePaths, vector<unordered_map<int, vector<int>>> &_row_g2l, vector<unordered_map<int, int>> &_row_l2g, vector<unordered_map<int, int>> &_col_l2g, vector<unordered_map<int, int>> &_col_g2l, vector<vector<vector<string>>> &_matrixes, vector<vector<string>>& _col_names, vector<cluster> _clusters)
    {
        filePaths = _filePaths;
        row_g2l = _row_g2l;
        row_l2g = _row_l2g;
        col_l2g = _col_l2g;
        col_g2l = _col_g2l;
        matrixes = _matrixes;
        col_names = _col_names;
        clusters = _clusters;
        exact_scores.resize(filePaths.size());
        for (auto &ele : exact_scores)
            ele.resize(filePaths.size());
        k_mins = true;
    }
    void load_row_l2g_correspondence(const string& file = "./result/row_matching.csv");//
    void load_col_g2l_correspondence(const string &file = "./result/schema_matching.csv");
    void load_matrixes();
    void load_cluster(const string& file = "./result/clusters.csv");
    void cell_level_jaccard_no_pk(); //row correspondence + cell_level_jaccard
    void cell_level_jaccard();
    void aggregate_each_column_set();
    void column_level_jaccard();
    void cell_level_jaccard_approx(int K = 20);
    void column_level_jaccard_approx(int K = 20);
    void relational_sim();
    void print_sim_scores(const string& output_dir = "./result/");
    // helper
    void convert_adj_matrix_2_edge_list(vector<edge> &all_edges, vector<vector<edge>> &cluster_edges, const string &metric = "cell");
    // construct lineage graph based on MDL principle
    void lineage_construction_mst(const string &metric = "cell", const string& output_dir = "./result/"); // intra-cluster & inter-cluster
    // some other approaches e.g., threshold-based
    void lineage_construction_threshold();
    /*Use sketch*/
    void initialize_sketch(int K);
    void set_sketch_type(const string& sketch_type = "kMins");
    void construct_sketches_cell_level(int K);
    void construct_sketches_col_level(int K);
};