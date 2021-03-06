#include "../../include/preserving_ops/preserving_ops.h"


void preserving_ops::load_row_l2g_correspondence(const string &file) {
    ifstream fin(file);
    string line;
    char delimiter = ',';
    while (getline(fin, line))
    {
        istringstream ss(line);
        string file_name, fid, row_cnt;
        getline(ss, file_name, delimiter);
        getline(ss, fid, delimiter);
        getline(ss, row_cnt, delimiter);
        for (int i = 0; i < stoi(row_cnt); ++i)
        {
            getline(fin, line);
            string global_rid, local_rid;
            istringstream ss_line(line);
            getline(ss_line, local_rid, delimiter);
            getline(ss_line, global_rid, delimiter);
            row_l2g[stoi(fid)][stoi(local_rid)] = stoi(global_rid);
            row_g2l[stoi(fid)][stoi(global_rid)].push_back(stoi(local_rid));
        }
    }
    cout << "---------load row_l2g row_l2g from file ----------- \n";
    cout << row_g2l.size() << " files in total. \n";
} 

void preserving_ops::load_col_g2l_correspondence(const string& file) {
    ifstream fin(file);
    string line;
    char delimiter = ',';
    while (getline(fin, line))
    {
        if (line.empty()) break;
        istringstream ss(line);
        string file_name, fid, col_cnt;
        getline(ss, file_name, delimiter);
        getline(ss, fid, delimiter);
        getline(ss, col_cnt, delimiter);
        //cout << file_name << " " << fid << " " << col_cnt << endl;
        file_id2name[stoi(fid)] = file_name.substr(file_name.find_last_of("/\\") + 1);
        for (int i = 0; i < stoi(col_cnt); ++i)
        {
            getline(fin, line);
            string global_cid, local_cid;
            istringstream ss_line(line);
            getline(ss_line, global_cid, delimiter);
            getline(ss_line, local_cid, delimiter);
            col_g2l[stoi(fid)][stoi(global_cid)] = stoi(local_cid);
            col_l2g[stoi(fid)][stoi(local_cid)] = stoi(global_cid);
            //cout << global_cid << " " << local_cid << endl; 
        }
    }
    cout << "---------load col_g2l col_l2g from file ----------- \n";
    cout << col_g2l.size() << " files in total. \n";
}

/*
output: $matrixes -- the first row maintains the global_cid; 
        while the first column is now a place holder for global_rid
*/
void preserving_ops::load_matrixes()
{
    for (int fid = 0; fid < filePaths.size(); ++fid)
    {
        ifstream fin(filePaths[fid]);
        string line;
        bool header = true;
        while (getline(fin, line))
        {
            istringstream ss(line);
            vector<string> record;
            record.push_back("global_rid"); // the first column maintains global_rid
            int cid = 0, null_id = 0;
            while (ss.good() && ss >> std::ws)
            {
                string cell;
                if (ss.peek() == '"')
                {
                    ss >> quoted(cell);
                    string discard;
                    getline(ss, discard, ',');
                    if (!is_null_string(discard))
                        cell = "\"" + cell + "\"" + discard;
                }
                else
                {
                    getline(ss, cell, ',');
                }
                if (header)
                {
                    col_names[fid].push_back(cell);
                    int global_cid = col_l2g[fid][cid];
                    record.push_back(to_string(global_cid));
                    // the first row maintains the global_cid for each column
                }
                else
                {
                    if (is_null_string(cell))
                        cell = "null_" + to_string(null_id++);
                    record.push_back(cell);
                }
                ++cid;
            }
            if (!header && with_pk) {
                int rid = matrixes[fid].size() - 1; 
                record[0] = to_string(row_l2g[fid][rid]); //populate the placeholder -- global_rid
            }
            matrixes[fid].push_back(record);
            //cout << record[0] << endl;
            header = false;
        }
    }
    cout << "----------------load files---------------\n";
    cout << "In total " << matrixes.size() << " files \n";
}

/*
load cluster 
*/
void preserving_ops::load_cluster(const string &file)
{
    ifstream fin(file);
    string line;
    char delimiter = ',';
    while (getline(fin, line)) {
        if (line.empty()) break;
        istringstream ss(line);
        string cluster_id, cnt, file_id;
        getline(ss, cluster_id, delimiter);
        getline(ss, cnt, delimiter);
        vector<int> file_ids;
        for (int i = 0; getline(ss, file_id, delimiter), i < stoi(cnt); ++i)
        {
            file_ids.push_back(stoi(file_id));
        }
        clusters.push_back(cluster(stoi(cluster_id), file_ids));
    }
    cout << "---------------load cluster from file---------------\n";
    cout << "In total " << clusters.size() << " clusters\n";
}

////////////////////////////////////////////////////////////////////////
double cell_level_jaccard_for_version_pair(const vector<vector<string>> &A, const vector<vector<string>> &B, unordered_map<int, vector<int>> &row_g2l_B, unordered_map<int, int> &col_g2l_B)
{
    int common_cnt = 0, cnt = 0;
    cout << A.size() - 1 << " rows; " << A[0].size() - 1 << " columns." << endl;
    unordered_map<int,int> matched_rows_B;
    for (int i = 1; i < A.size(); ++i)
    { // row i
        //cout << A[i][0] << endl;
        int global_rid = stoi(A[i][0]);
        if (row_g2l_B.count(global_rid) == 0)
        {
            //cout << i << " no matched row " << global_rid << endl;
            continue;
        } // no matched row
        vector<int> rid_Bs = row_g2l_B[global_rid];
        unordered_map<int,int> rid_2_common_cnt; // determine which rid to choose from rid_Bs
        // cout << global_rid << " " << rid_B << endl;
        for (int j = 1; j < A[i].size(); ++j)
        {
            ++cnt;
            int global_cid = stoi(A[0][j]);
            //cout << i << " " << j << ";" << global_rid << " " << global_cid;
            if (col_g2l_B.count(global_cid) == 0)
            {
                //cout << j << " no matched column " << global_cid << endl;
                continue;
            } // no matched column
            int cid_B = col_g2l_B[global_cid];
            //cout << global_cid << " -- " << cid_B << endl;
            for (const auto& rid_B : rid_Bs) {
                if (rid_B >= B.size() || cid_B >= B[0].size())
                {
                    cout << global_rid << " " << rid_B << ">=" << B.size() << ";" << global_cid << " " << cid_B << ">=" << B[0].size() << endl;
                    exit(1);
                }
                if (cid_B + 1 >= B[rid_B + 1].size())
                {
                    cout << "---------ERROR " << i << "," << j << ";" << rid_B << "," << cid_B << endl;
                    cout << global_rid << " ----- " << global_cid << endl;
                    exit(1);
                    continue; // this row does not have this column
                }
                //cout << rid_B << endl;
                if (A[i][j] == B[rid_B + 1][cid_B + 1])
                    ++rid_2_common_cnt[rid_B];
            }
        }
        if (rid_2_common_cnt.empty()) continue; //no matched
        //cout << " here \n";
        int selected_rid = rid_2_common_cnt.begin()->first;
        //cout << selected_rid << endl;
        for (const auto& ele : rid_2_common_cnt) {
            if (matched_rows_B.count(ele.first)) continue; // already matched by other row in A
            if (ele.second > rid_2_common_cnt[selected_rid] || matched_rows_B.count(selected_rid)) {
                selected_rid = ele.first;
            }
        }
        if (matched_rows_B.count(selected_rid)) {
            int delta = rid_2_common_cnt[selected_rid] - matched_rows_B[selected_rid];
            if (delta > 0) { //optimization, prefer the latter one
                matched_rows_B[selected_rid] += delta;
                common_cnt += delta;
            }
            continue;
        }
        common_cnt += rid_2_common_cnt[selected_rid];
        matched_rows_B[selected_rid] = rid_2_common_cnt[selected_rid]; // this row has been matched
    }
    
    int total_cell_A = (A.size() - 1) * (A[0].size() - 1), total_cell_B = (B.size() - 1) * (B[0].size() - 1);
    cout << "Finish naiveCompareCell " << common_cnt << " common cells. " << (total_cell_A + total_cell_B) << " total cells." << endl;
    return (double)(common_cnt) / (total_cell_A + total_cell_B - common_cnt);
}


double jaccard_sim_for_two_columns(const unordered_set<string>& a, const unordered_set<string>& b) {
    int common = 0;
    for (const auto& ele : a) {
        if (b.count(ele)) ++common;
    }
    return double(common) / (a.size() + b.size() - common);
}

double column_level_jaccard_for_version_pair(const vector<colStats>& A, const vector<colStats>& B, const unordered_map<int,int>& col_g2l_B) {
    vector<double> sim_cols;
    double total_sim = 0;
    for (const auto& col_A : A) {
        int global_cid = col_A.global_cid;
        if (col_g2l_B.count(global_cid) == 0) continue; // no matched column
        int cid_B = col_g2l_B.at(global_cid);
        double sim = jaccard_sim_for_two_columns(col_A.domain, B[cid_B].domain); // this column's jaccard sim
        total_sim += sim;
        sim_cols.push_back(sim);
    }
    return total_sim / (A.size() + B.size() - sim_cols.size());
}

pair<int, int> construct_network(const vector<vector<string>> &A, const vector<vector<string>> &B, unordered_map<int, vector<int>> &row_g2l_B, unordered_map<int, int> &col_g2l_B, vector<unordered_map<int, int>> &rGraph)
{
    clock_t tt = clock();
    vector<pair<int, int>> commonRids, commonCids; // (A,B) rid, cid
    for (int i = 1; i < A.size(); ++i)
    {
        int global_rid = stoi(A[i][0]);
        if (row_g2l_B.count(global_rid) > 0)
            commonRids.push_back({i - 1, row_g2l_B[global_rid][0]}); // TODO!!
    }
    for (int j = 1; j < A[0].size(); ++j)
    {
        int global_cid = stoi(A[0][j]);
        if (col_g2l_B.count(global_cid) > 0)
            commonCids.push_back({j - 1, col_g2l_B[global_cid]});
    }
    int u1_cnt = commonRids.size(), u2_cnt = commonCids.size(), s = 0, t = u1_cnt + u2_cnt + 1, e_cnt = 0;
    if (u1_cnt == 0 || u2_cnt == 0)
        return {u1_cnt, u2_cnt}; // no common row or no common column
    int w_u1 = max(1, u2_cnt / u1_cnt), w_u2 = max(1, u1_cnt / u2_cnt);
    rGraph.resize(t + 1);
    // connect source s to every node in u1
    for (int u1 = 1; u1 <= u1_cnt; ++u1)
    {
        rGraph[s][u1] = w_u1;
        rGraph[u1][s] = 0;
    }
    // connect u1 to u2 if there is edge between them (if this cell is not the same in A and B)
    for (int u1 = 1; u1 <= u1_cnt; ++u1)
    {
        int rid_A = commonRids[u1 - 1].first, rid_B = commonRids[u1 - 1].second;
        for (int u2 = u1_cnt + 1; u2 < t; ++u2)
        {
            int cid_A = commonCids[u2 - u1_cnt - 1].first, cid_B = commonCids[u2 - u1_cnt - 1].second;
            // if (cid_B + 1 >= B[rid_B + 1].size()) {
            //     cout << "---------ERROR " << rid_A << "," << rid_B << ";" << cid_A << "," << cid_B << endl;
            //     cout << A[rid_A + 1][0] << "," << B[rid_B + 1][0] << " ----- " << A[0][cid_A + 1] << "," << B[0][cid_B + 1] << endl;
            //     exit(1);
            //     continue; // this row does not have this column
            // }
            // if these two cells are the same, then there is an edge in the bipartite graph for maximum node biclique; but no edge in the complementary bi-graph for the Indepedent set problem
            if (cid_B + 1 >= B[rid_B + 1].size() || A[rid_A + 1][cid_A + 1] != B[rid_B + 1][cid_B + 1])
            {
                rGraph[u1][u2] = INT_MAX;
                rGraph[u2][u1] = 0;
                ++e_cnt;
            }
        }
    }
    // connect every node in u2 to the sink t
    for (int u2 = u1_cnt + 1; u2 < t; ++u2)
    {
        rGraph[u2][t] = w_u2;
        rGraph[t][u2] = 0;
    }
    tt = clock() - tt;
    cout << "Finish constructNetwork (number of nodes in U1: " << u1_cnt << "; U2:" << u2_cnt << "; total: " << t + 1 << "). with " << e_cnt << " total bipartite edges. " << endl;
    cout << "wU1:" << w_u1 << "; wU2:" << w_u2 << ". Taking " << (double)(tt * 1000 / CLOCKS_PER_SEC) << " ms." << endl;
    return {u1_cnt, u2_cnt};
}

double relational_sim_for_version_pair(const vector<vector<string>> &A, const vector<vector<string>> &B, unordered_map<int, vector<int>> &row_g2l_B, unordered_map<int, int> &col_g2l_B)
{
    //clock_t t = clock();
    vector<unordered_map<int, int>> rGraph;
    pair<int, int> u1_u2_cnt = construct_network(A, B, row_g2l_B, col_g2l_B, rGraph);
    int u1_cnt = u1_u2_cnt.first, u2_cnt = u1_u2_cnt.second;
    if (u1_cnt == 0 || u2_cnt == 0) // {u1_cnt, u2_cnt}
        return 0;
    vector<bool> S = minCut(rGraph, 0, rGraph.size() - 1); // s = 0; t = rGraph.size() - 1, S is of the same size as t
    vector<int> selectedRows, selectedCols;
    for (int i = 0; i < S.size(); ++i)
    {
        if (S[i] && i <= u1_cnt && i > 0) // U1 \intersect reachable_nodes_from_s
        {
            selectedRows.push_back(i);
        }
        if (!S[i] && i > u1_cnt && i < rGraph.size() - 1) // U2 \intersect not_reachable_nodes_from_s
        {
            selectedCols.push_back(i);
            //cout << "selected col: " << i << endl;
        }
    }
    //t = clock() - t;
    cout << "The maximum node biclique: ( " << selectedRows.size() << ", " << selectedCols.size() << " )" << endl;
    //cout << " InstructionSimCompute(constructNetwork + minCut): taking " << ((double)t * 1000) / CLOCKS_PER_SEC << " ms \t";
    // {(int)selectedRows.size(), (int)selectedCols.size()}
    double total_area = selectedRows.size() * selectedCols.size();
    int total_cell_A = (A.size() - 1) * (A[0].size() - 1), total_cell_B = (B.size() - 1) * (B[0].size() - 1);
    return total_area / (total_cell_A + total_cell_B - total_area);
}

/*
Estimate similarity using kmins
input: $sigA and $sigB of the same size
output: similarity metric
*/
double estimatorKmins(const vector<int> &sigA, const vector<int> &sigB)
{
    if (sigA.size() != sigB.size())
    {
        fprintf(stderr, "=========sigA (%ld) != sigB (%ld)", sigA.size(), sigB.size());
    }
    //clock_t t = clock();
    int common_sig = 0;
    for (int k = 0; k < sigA.size(); ++k)
    {
        if (sigA[k] == sigB[k])
            ++common_sig;
    }
    // t = clock() - t;
    // fprintf(stderr, "===========pairwiseJaccard %f, using %f ms=========\n", double(common_sig) / sigA.size(), (double)(t * 1000) / CLOCKS_PER_SEC);
    return double(common_sig) / sigA.size();
}

/*
one estimator using bottomK -- first estimate "intersection" & "union"
then calculate jaccard based on set estimator
*/
double unionEstimatorBottomK(const vector<int> &sigA, const vector<int>& sigB, int K) {
    vector<int> AandB, AorB; // AorB is of size K, HashCnt
    int ptr1 = 0, ptr2 = 0;
    while (ptr1 < sigA.size() && ptr2 < sigB.size())
    {
        if (sigA[ptr1] == sigB[ptr2])
        {
            AandB.push_back(sigA[ptr1]);
            AorB.push_back(sigA[ptr1]);
            ++ptr1;
            ++ptr2;
        } else {
            if (sigA[ptr1] < sigB[ptr2])
            {
                AorB.push_back(sigA[ptr1++]);
            } else {
                AorB.push_back(sigB[ptr2++]);
            }
        }
    }
    if (AorB.size() > K) AorB.resize(K);
    int common = 0;
    ptr1 = 0; ptr2 = 0;
    while (ptr1 < AandB.size() && ptr2 < AorB.size()) {
        if (AandB[ptr1] == AorB[ptr2]) {
            ++common; ++ptr1; ++ptr2;
        } else {
            if (AandB[ptr1] < AorB[ptr2]) {
                ++ptr1;
            } else {
                ++ptr2;
            }
        }
    }
    return double(common) / AorB.size();
}

/*
the other estimator via bottomK sketch;
simply treat sigA and sigB as two sets, and calculate jaccard
*/
double estimatorBottomK(const vector<int> &sigA, const vector<int> &sigB)
{
    //clock_t t = clock();
    int common_sig = 0;
    unordered_set<int> exist(sigB.begin(), sigB.end());
    if (sigB.size() != exist.size()) // exist duplicates in sigB -- different values are hashed to the same sig
        cout << "--------bottom-k sketch:" << sigB.size() << " != " << exist.size() << endl;
    for (const int& ele : sigA)
    {
        if (exist.count(ele))
            ++common_sig;
    }
    // t = clock() - t;
    // fprintf(stderr, "===========pairwiseJaccard %f, using %f ms=========\n", double(common_sig) / sigA.size(), (double)(t * 1000) / CLOCKS_PER_SEC);
    return double(common_sig) / (sigA.size() + sigB.size() - common_sig);
}


///////////////////////////////////////////////////////////////////////
/*
Aggregate each column set per file
input: $matrixes
output: $file_columns
*/
void preserving_ops::aggregate_each_column_set()
{
    int file_cnt = filePaths.size();
    file_columns.resize(file_cnt);
    for (int fid = 0; fid < file_cnt; ++fid) {
        vector<vector<string>>& mat = matrixes[fid];
        file_columns[fid].clear();
        for (int cid = 1; cid < mat[0].size(); ++cid) {
            colStats cur_col;
            cur_col.global_cid = stoi(mat[0][cid]);
            for (int rid = 1; rid < mat.size(); ++rid) {
                if (cid < mat[rid].size()) // some record only has a fraction of columns
                    cur_col.domain.insert(mat[rid][cid]);
            }
            file_columns[fid].push_back(cur_col);
        }
    }
    cout << "-----------finish aggregating each column set--------------\n";
}

void preserving_ops::cell_level_jaccard()
{
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i) {
        exact_scores[i][i].cell_jaccard = 1;
        for (int j = 0; j < i; ++j) {
            string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            cout << "=======" << file_i << " vs.(cell-jaccard) " << file_j << "=======" << endl;
            double sim = cell_level_jaccard_for_version_pair(matrixes[i], matrixes[j], row_g2l[j], col_g2l[j]);
            exact_scores[i][j].cell_jaccard = sim;
            exact_scores[j][i].cell_jaccard = sim;
        }
    }
    /*instead of calculating within each cluster*/
/*     for (const auto& each_cluster : clusters) {
        const vector<int>& file_ids = each_cluster.file_ids;
        for (int i = 0; i < file_ids.size(); ++i) {
            int u = file_ids[i];
            exact_scores[u][u].cell_jaccard = 1;
            for (int j = 0; j < i; ++j) {
                int v = file_ids[j];
                string file_u = filePaths[u].substr(filePaths[u].find_last_of("/\\") + 1);
                string file_v = filePaths[v].substr(filePaths[v].find_last_of("/\\") + 1);
                cout << "=======" << file_u << " vs.(cell-jaccard) " << file_v << "=======" << endl;
                double sim = cell_level_jaccard_for_version_pair(matrixes[u], matrixes[v], row_g2l[v], col_g2l[v]);
                exact_scores[u][v].cell_jaccard = sim;
                exact_scores[v][u].cell_jaccard = sim;
            }
        }
    }
 */    
    cout << "------------finish calculating cell level jaccard similarity-----------\n";
}

void print_sig(const vector<int>& sig) {
    for (const auto& ele : sig) {
        cout << ele << ",";
    }
    cout << endl;
}

void preserving_ops::cell_level_jaccard_approx(int K) {
    construct_sketches_cell_level(K);
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i) {
        exact_scores[i][i].cell_jaccard = 1;
        //cout << i << ":";
        //print_sig(sigs_cell[i]);
        for (int j = 0; j < i; ++j) {
            string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            cout << "=======" << file_i << " vs.(cell-jaccard) " << file_j << "=======" << endl;
            double sim = (k_mins?estimatorKmins(sigs_cell[i], sigs_cell[j]):estimatorBottomK(sigs_cell[i], sigs_cell[j]));
            exact_scores[i][j].cell_jaccard = sim;
            exact_scores[j][i].cell_jaccard = sim;
        }
    }
    cout << "------------finish calculating cell level jaccard similarity approximately-----------\n";
}

void preserving_ops::cell_level_jaccard_no_pk() {
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i) {
        exact_scores[i][i].cell_jaccard = 1;
        for (int j = 0; j < i; ++j) {
            string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            cout << "=======" << file_i << " vs.(cell-jaccard) " << file_j << "=======" << endl;
            vp_row_matching vp(matrixes[i], matrixes[j], col_g2l[j]);
            vp.construct_r2r_similarity();
            vp.r2r_correspondence_greedy();
            int common_cnt = vp.return_largest_common_cells();
            int total_cell_A = (matrixes[i].size() - 1) * (matrixes[i][0].size() - 1), total_cell_B = (matrixes[j].size() - 1) * (matrixes[j][0].size() - 1);
            double sim = (double)(common_cnt) / (total_cell_A + total_cell_B - common_cnt);
            exact_scores[i][j].cell_jaccard = sim;
            exact_scores[j][i].cell_jaccard = sim;
        }
    }
    cout << "------------finish calculating cell level jaccard similarity with no pk-----------\n";
}
void preserving_ops::column_level_jaccard() {
    aggregate_each_column_set();
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i)
    {
        exact_scores[i][i].col_jaccard = 1;
        for (int j = 0; j < i; ++j)
        {
            string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            //cout << "=======" << file_i << " vs.(col-jaccard) " << file_j << "=======" << endl;
            double sim = column_level_jaccard_for_version_pair(file_columns[i], file_columns[j], col_g2l[j]);
            exact_scores[i][j].col_jaccard = sim;
            exact_scores[j][i].col_jaccard = sim;
        }
    }
    cout << "------------finish calculating column level jaccard similarity------------\n";
}


void preserving_ops::column_level_jaccard_approx(int K) {
    aggregate_each_column_set();
    construct_sketches_col_level(K);
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i)
    {
        exact_scores[i][i].col_jaccard = 1;
        for (int j = 0; j < i; ++j)
        {
            string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            //cout << "=======" << file_i << " vs.(col-jaccard) " << file_j << "=======" << endl;
            // iterate through the columns 
            double total_sim = 0;
            vector<double> sim_cols;
            for (int c = 0; c < file_columns[i].size(); ++c) {
                int global_cid = file_columns[i][c].global_cid;
                if (col_g2l[j].count(global_cid) == 0) continue; // no matched column
                int cid_B = col_g2l[j].at(global_cid);
                // can change to another estimator estimatorBottomK
                double sim = (k_mins?estimatorKmins(sigs_col[i][c], sigs_col[j][cid_B]):estimatorBottomK(sigs_col[i][c], sigs_col[j][cid_B])); 
                total_sim += sim;
                sim_cols.push_back(sim);
            }
            double sim = total_sim / (sigs_col[i].size() + sigs_col[j].size() - sim_cols.size());
            exact_scores[i][j].col_jaccard = sim;
            exact_scores[j][i].col_jaccard = sim;
        }
    }
    cout << "------------finish calculating column level jaccard similarity using sketch------------\n";
}


void preserving_ops::relational_sim() {
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i)
    {
        exact_scores[i][i].relational = 1;
        for (int j = 0; j < i; ++j)
        {
            string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            //cout << "=======" << file_i << " vs.(relational-jaccard) " << file_j << "=======" << endl;
            double sim = relational_sim_for_version_pair(matrixes[i], matrixes[j], row_g2l[j], col_g2l[j]);
            exact_scores[i][j].relational = sim;
            exact_scores[j][i].relational = sim;
        }
    }
    // for (const auto& each_cluster : clusters) {
    //     const vector<int>& file_ids = each_cluster.file_ids;
    //     for (int i = 0; i < file_ids.size(); ++i) {
    //         int u = file_ids[i];
    //         exact_scores[u][u].relational = 1;
    //         for (int j = 0; j < i; ++j) {
    //             int v = file_ids[j];
    //             string file_u = filePaths[u].substr(filePaths[u].find_last_of("/\\") + 1);
    //             string file_v = filePaths[v].substr(filePaths[v].find_last_of("/\\") + 1);
    //             cout << "=======" << file_u << " vs.(relational-jaccard) " << file_v << "=======" << endl;
    //             double sim = relational_sim_for_version_pair(matrixes[u], matrixes[v], row_g2l[v], col_g2l[v]);
    //             exact_scores[u][v].relational = sim;
    //             exact_scores[v][u].relational = sim;
    //         }
    //     }
    // }
}

void preserving_ops::print_sim_scores(const string& output_dir)
{
    ofstream fout_cell(output_dir + "/cell_sim_" + (with_pk?"PK":"noPK") + ".csv");
    ofstream fout_col(output_dir + "/col_sim.csv");
    ofstream fout_relation(output_dir + "/relation_sim.csv");
    fout_cell << "name";
    fout_col << "name";
    fout_relation << "name";
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i) {
        string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
        fout_cell << "," + file_i;
        fout_col << "," + file_i;
        fout_relation << "," + file_i;
    }
    fout_cell << endl;
    fout_col << endl;
    fout_relation << endl; 
    //////////finish header, start with value
    for (int i = 0; i < artifact_cnt; ++i)
    {
        string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
        fout_cell << file_i;
        fout_col << file_i;
        fout_relation << file_i;
        for (int j = 0; j < artifact_cnt; ++j)
        {
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            fout_cell << "," << exact_scores[i][j].cell_jaccard;
            fout_col << "," << exact_scores[i][j].col_jaccard;
            fout_relation << "," << exact_scores[i][j].relational;
        }
        fout_cell << endl;
        fout_col << endl;
        fout_relation << endl;
    }
	cout << "--------finish printing similarity scores----------\n";
}

/////////////////////////////////////////////////////////////////////////
// healper
void get_file_2_cluster(vector<int> &file2cluster, const vector<cluster> &clusters)
{
    for (const auto& each_cluster : clusters) {
        int id = each_cluster.id;
        for (const auto& fid : each_cluster.file_ids) {
            file2cluster[fid] = id;
        }
    }
}

void preserving_ops::convert_adj_matrix_2_edge_list(vector<edge> &cross_edges, vector<vector<edge>> &within_edges, const string &metric)
{
    int artifact_cnt = filePaths.size();
    vector<int> file2cluster(artifact_cnt);
    get_file_2_cluster(file2cluster, clusters);
    within_edges.resize(clusters.size());
    for (int i = 0; i < artifact_cnt; ++i)
    { // iterate through file ids
        string file_i = filePaths[i].substr(filePaths[i].find_last_of("/\\") + 1);
        int cluster_i = file2cluster[i];
        for (int j = 0; j < i; ++j)
        {
            string file_j = filePaths[j].substr(filePaths[j].find_last_of("/\\") + 1);
            // use diff instead of similarity score
            edge e = edge(file_i, file_j, 1 - exact_scores[i][j].get_metric(metric));
            
            int cluster_j = file2cluster[j];
            if (cluster_i == cluster_j)
                within_edges[cluster_i].push_back(e);
            else cross_edges.push_back(e);
        }
    }
	cout << "--------finish constructing inter and intra-cluster edges--------\n";
}

void preserving_ops::lineage_construction_mst(const string& metric, const string& output_dir) {
    vector<edge> cross_edges;           // edges that with nodes from different clusters
    vector<vector<edge>> within_edges; // each cluster consist of a sub-graph with edges
    convert_adj_matrix_2_edge_list(cross_edges, within_edges, metric); //metric
    kruskal inferred_G;               // inferred lineage graph
    inferred_G.initialize_G(filePaths.size(), cross_edges);

    for (const auto& each_cluster: clusters) {
        int cluster_id = each_cluster.id;
        kruskal sub_mst(each_cluster.file_ids.size(), within_edges[cluster_id]);
        sub_mst.kruskalMST();
        //cout << "--------------------" << cluster_id << "-------------------\n";
        //sub_mst.printMST();
        inferred_G.insert_mst_edges(sub_mst.get_mst_edges());
    }
    inferred_G.kruskalMST();
    if (metric == "cell")
        inferred_G.printMST(output_dir + "/infered_mst_" + metric + "_" + (with_pk?"PK":"noPK") + ".csv");
    else
        inferred_G.printMST(output_dir + "/infered_mst_" + metric + ".csv");
}


/*use sketches*/
void preserving_ops::set_sketch_type(const string& sketch_type) {
    if (sketch_type == "kMins") {
        k_mins = true;
    } else {
        k_mins = false;
    }
}

void preserving_ops::construct_sketches_cell_level(int K) {
    sigs_cell.clear();
    minHash *sketch;
    if (k_mins) {
        kmins *kmins_sketch = new kmins();
        sketch = kmins_sketch;
    } else {
        bottomK *bottomK_sketch = new bottomK();
        sketch = bottomK_sketch;
    }
    // calculate version-wise sig
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i) {
        sketch->computeSig(matrixes[i]);
        sigs_cell.push_back(sketch->return_sig());
    }
    delete sketch;
}

void preserving_ops::initialize_sketch(int K) {
    if (k_mins) {
        kmins::initialize(K);
    } else {
        bottomK::initialize(K);
    }
}

void preserving_ops::construct_sketches_col_level(int K) {
    minHash *sketch;
    if (k_mins) {
        kmins *kmins_sketch = new kmins();
        sketch = kmins_sketch;
    } else {
        bottomK *bottomK_sketch = new bottomK();
        sketch = bottomK_sketch;
    }
    // calculate version-wise sig
    int artifact_cnt = filePaths.size();
    for (int i = 0; i < artifact_cnt; ++i) {
        vector<vector<int>> per_version;
        for (int cid = 0; cid < file_columns[i].size(); ++cid) {
            sketch->computeSig(file_columns[i][cid].domain);
            per_version.push_back(sketch->return_sig());
        }
        sigs_col.push_back(per_version);
    }
    delete sketch;
}

