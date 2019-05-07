#include "../../include/profiling/row_matching.h"

/*
if $useHLL=true, then calculate hyperloglog
output: $matrixes -- the first row maintains the global_cid; 
        while the first column is now a place holder for global_rid
*/
void row_matching::load_files_and_calculate_stats() {
    for (int fid = 0; fid < filePaths.size(); ++fid) {
        ifstream fin(filePaths[fid]);
        string line;
        bool header = true;
        while (getline(fin, line)) {
            if (line.empty()) break;
            istringstream ss(line);
            vector<string> record;
            record.push_back("global_rid"); // the first column maintains global_rid
            int cid = 0, null_id = 0;
            while (ss.good() && ss >> std::ws) {
                string cell;
                if (ss.peek() == '"') {
                    ss >> quoted(cell);
                    string discard;
                    getline(ss, discard, ',');
                    if (!is_null_string(discard))
                        cell = "\"" + cell + "\"" + discard;
                } else {
                    getline(ss, cell, ',');
                }
                if (header) {
                    if (is_null_string(cell))
                        cell = "null_" + to_string(null_id++);
                    col_names[fid].push_back(cell);
                    int global_cid = col_l2g[fid][cid];
                    record.push_back(to_string(global_cid));
                    //cout << to_string(cid) << " " << to_string(global_cid) << endl;
                    // the first row maintains the global_cid for each column
                } else {
                    record.push_back(cell);
                    if (useHLL) { // update $hyperloglog to calculate # of distinct values
                        /*TODO*/
                    }
                }
                ++cid;
            }
            // if the record does not conform to the schema, ignore
            //if (matrixes[fid].empty() || record.size() == matrixes[fid][0].size()) 
            matrixes[fid].push_back(record);
            header = false;
        }
        //cout << filePaths[fid] << " ========= finish loading ========\n";
    }
    cout << "----------------load files---------------\n";
    cout << "In total " << matrixes.size() << " files \n";
}


/*
randomly examine $sampleCnt samples 
*/
void row_matching::detect_pk_by_sampling(int sampleCnt) {
    for (int fid = 0; fid < matrixes.size(); ++fid) {
        // distinct values in each column
        int col_cnt = matrixes[fid][0].size() - 1; // excluding the first column: global_rid
        vector<unordered_set<string>> col_distincts(col_cnt);
        for (int i = 0; i < sampleCnt; ++i) {
            int rid = rand() % (matrixes[fid].size() - 1) + 1; // exclude the first row
            //cout << rid << " ----- " << matrixes[fid].size()- 1 << endl;
            for (int cid = 1; cid < matrixes[fid][rid].size(); ++cid)
            { // remember to -1 from matrixes index to other index
                col_distincts[cid - 1].insert(matrixes[fid][rid][cid]);
            }
        }
        int pk_cid  = 0;
        // the column with largest number of distinct value is selected as the pk
        for (int cid = 0; cid < col_cnt; ++cid) {
            //cout << filePaths[fid] << "," <<  col_names[fid][cid] << " " << col_distincts[cid].size() << endl;
            if (col_distincts[cid].size() > col_distincts[pk_cid].size()) {
                pk_cid = cid;
            }
            else if (col_distincts[cid].size() == col_distincts[pk_cid].size()) {
                // break the tie: the one with longer name is the pk
                if (col_names[fid][cid].size() > col_names[fid][pk_cid].size()) {
                    pk_cid = cid;
                }
            }
        }
        // pks: the global column id
        pks.push_back({pk_cid, col_l2g[fid][pk_cid]});
    }
    cout << "--------------finish identifying pks--------------\n";
}

/*
input: $matrixes, $pks
output: $globalName2Id, $row_g2l, $row_l2g
*/
void row_matching::row_match_by_pk_name_and_value() {
    string delimiter = ";";
    for (int fid = 0; fid < matrixes.size(); ++fid) {
        int cid = pks[fid].first;
        string pk = to_string(pks[fid].second);
        for (int rid = 1; rid < matrixes[fid].size(); ++rid) { //first row maintains global column id
            string name_val = pk + delimiter + matrixes[fid][rid][cid + 1];
            //if (rid % 1000 == 1) cout << name_val << " " << matrixes[fid].size() << endl;
            if (globalName2Id.count(name_val) == 0) {
                int id = globalName2Id.size();
                globalName2Id[name_val] = id;
                //if (id == 0) cout << name_val << "," << id << endl; 
            }
            int global_rid = globalName2Id[name_val];
            matrixes[fid][rid][0] = global_rid;
            row_g2l[fid][global_rid].push_back(rid - 1);
            row_l2g[fid][rid - 1] = global_rid;
        }
    }
    cout << "---------------finish row matching-----------------\n";
}

/*
write $row_l2g to file
*/
void row_matching::write_matching_to_file(const string &file) {
    ofstream fout(file);
    for (int fid = 0; fid < row_l2g.size(); ++fid)
    { // file_name, file_id, # of rows
        fout << filePaths[fid] << "," << fid << "," << row_l2g[fid].size() << endl;
        for (const auto &l2g : row_l2g[fid])
        {
            fout << l2g.first << "," << l2g.second << endl; // local,global
        }
    }
}

void row_matching::print_pk(const string &file)
{
    ofstream fout(file);
    fout << "file_name, local_cid, pk, global_cid\n"; 
    for (int fid = 0; fid < pks.size(); ++fid) {
        fout << filePaths[fid] << "," << pks[fid].first << "," << col_names[fid][pks[fid].first] << "," << pks[fid].second << endl;
    }
}