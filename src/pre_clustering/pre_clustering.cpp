#include "../../include/pre_clustering/pre_clustering.h"

/*load schema matching result from file;
output: $file_global_cids (a bunch of files, where each file consists of its corresponding global cids)
*/
void pre_clustering::load_col_matching_from_file(const string &file)
{
    ifstream fin(file);
    string line;
    char delimiter = ',';
    while (getline(fin, line)) {
        istringstream ss(line);
        string file_name, fid, col_cnt;
        getline(ss, file_name, delimiter);
        getline(ss, fid, delimiter);
        getline(ss, col_cnt, delimiter);
        file_id2name[stoi(fid)] = file_name.substr(file_name.find_last_of("/\\") + 1);
        vector<int> global_ids;
        for (int i =0; i < stoi(col_cnt); ++i) {
            getline(fin, line);
            string global_id;
            istringstream ss_line(line);
            getline(ss_line, global_id, delimiter);
            global_ids.push_back(stoi(global_id));
        }
        sort(global_ids.begin(), global_ids.end());
        file_global_cids.push_back(global_ids);
    }
    cout << "---------load global_cids from file ----------- \n";
    cout << file_global_cids.size() << " files in total. \n";
}

/*
input: $file_global_cids;
output: $clusters
*/
void pre_clustering::cluster_with_exact_schema() {
    // cluster is uniquely identified by the concatenation of global cids
    unordered_map<string, vector<int>> cluster2files; 
    for (int fid = 0; fid < file_global_cids.size(); ++fid) {
        vector<int>& file = file_global_cids[fid];
        string unique_name; // for identifying this file
        for (const auto& cid : file) {
            unique_name += to_string(cid) + " ";
        }
        //cout << unique_name << " " << fid << endl;
        cluster2files[unique_name].push_back(fid);
    }
    // populate $cluster
    int id = 0;
    for (const auto& ele : cluster2files) {
        //cout << id << ":" << ele.first << endl;
        clusters.push_back(cluster(id, ele.second));
        ++id;
    }
    cout << "--------- cluster with the exact schema ---------\n";
    cout << cluster2files.size() << " clusters\n"; 
}

void pre_clustering::cluster_with_partial_schema() {
    cout << "------cluster with partial_schema-------" << endl;
}

void pre_clustering::cluster_with_pk() {
    cout << "------cluster with pk-------" << endl;
}

void pre_clustering::all_in_one_cluster() {
    cout << "-------all files are in one cluster-------" << endl;
    vector<int> ids;
    for (int fid = 0; fid < file_global_cids.size(); ++fid){
        ids.push_back(fid);
    }
    clusters.push_back(cluster(0,ids));
}
    /*
input: $clusters
output: $file
*/
void pre_clustering::write_clusters_to_file(const string &file)
{
    ofstream fout(file);
    ofstream fout2("./src/pre_clustering/clusters_with_filename.csv");
    for (const auto& cluster: clusters) {
        fout << cluster.id << "," << cluster.file_ids.size();
        fout2 << cluster.id << "," << cluster.file_ids.size();
        for (const auto& file_id : cluster.file_ids) {
            fout << "," << file_id;
            fout2 << "," << file_id2name[file_id];
        }
        fout << endl;
        fout2 << endl;
    }
}

