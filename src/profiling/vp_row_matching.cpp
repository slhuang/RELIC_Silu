#include "../../include/preserving_ops/preserving_ops.h"


void vp_row_matching::construct_r2r_similarity() {
    // per column
    for (int c = 1; c < A[0].size(); ++c) {
        int global_cid = stoi(A[0][c]);
        if (col_g2l_B.count(global_cid) == 0)
        {
            //cout << j << " no matched column " << global_cid << endl;
            continue;
        } // no matched column
        int cid_B = col_g2l_B[global_cid];
        unordered_map<string, vector<int>> val2rids_A, val2rids_B;
        for (int r = 1; r < A.size(); ++r) {
            val2rids_A[A[r][c]].push_back(r - 1); 
        }
        for (int r = 1; r < B.size(); ++r) {
            val2rids_B[B[r][cid_B + 1]].push_back(r - 1);
        }
        // for each the same value, update $common
        for (const auto& val_rids_A : val2rids_A) {
            string same_val = val_rids_A.first;
            if (val2rids_B.count(same_val) == 0) continue;
            const vector<int>& rids_A = val_rids_A.second, &rids_B = val2rids_B[same_val];
            for (const auto& rid_A : rids_A) {
                for (const auto& rid_B : rids_B) {
                    ++common[rid_A][rid_B];
                }
            }
        }
    }
    cout << "--------Finish constructing row2row similarity------------" << endl;
}

void vp_row_matching::print_r2r_similarity() {
    for (int i = 0; i < common.size(); ++i) {
        for (int j = 0; j < common[i].size(); ++j) {
            cout << i << " " << j << " " << common[i][j] << endl;
        }
    }
}

/*
compute row-to-row correspondence in a greedy way;
*/
void vp_row_matching::r2r_correspondence_greedy() {
    vector<triple> common_cnts;
    const auto& cmp = [](const triple& A, const triple& B) {
        return A.cnt > B.cnt;
    };
    for (int i = 0; i < common.size(); ++i) {
        for (int j = 0; j < common[i].size(); ++j) {
            common_cnts.push_back(triple(i, j, common[i][j]));
        }
    }
    sort(common_cnts.begin(), common_cnts.end(), cmp);
    unordered_set<int> selected_ridA, selected_ridB;
    for (const auto& ele : common_cnts) {
        if (selected_ridA.count(ele.ridA) != 0 || selected_ridB.count(ele.ridB) != 0) continue;
        selected_ridA.insert(ele.ridA);
        selected_ridB.insert(ele.ridB);
        row_A2B[ele.ridA] = ele.ridB;
        common_cells += ele.cnt;
        //cout << row_A2B.size() << " " << ele.cnt << endl;
        // if (row_A2B.size() == ) // early break 
    }
}