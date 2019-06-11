#include "../../include/index/minHash.h"

int minHash::K = 20;
vector<hashFunc> kmins::permutation = vector<hashFunc>(minHash::K, hashFunc()); // need to first initialize it
hashFunc bottomK::permutation = hashFunc();

void kmins::computeSig(const unordered_set<string>& vals) {
    sig.clear();
    sig.resize(K, INT_MAX);
    for (const auto& val : vals) {
        int cell = hash<string>{}(val) % mersenne_prime;
        for (int k = 0; k < K; ++k) {
            int hash_k = (int)(((long long)(permutation[k].a) * cell + permutation[k].b) % permutation[k].c); //% mersenne_prime);
            sig[k] = min(sig[k], hash_k);
        }
    }
    //cout << "--------kmins column level----------" << endl;
}

void kmins::computeSig(const vector<vector<string>>& matrix) {
    sig.clear();
    sig.resize(K, INT_MAX);
    // cout << "---------------" << endl;
    for (int r = 1; r < matrix.size(); ++r) {
        string rid = matrix[r][0];
        for (int col = 1; col < matrix[r].size(); ++col)
        {
            string cid = matrix[0][col];
            string val = matrix[r][col];
            string combined = cid + " " + rid + " " + val;
            int cell = hash<string>{}(combined) % mersenne_prime;
            //cout << combined << " " << cell;
            for (int k = 0; k < K; ++k) {
                int hash_k = (int)(((long long)(permutation[k].a) * cell + permutation[k].b) % permutation[k].c);//mersenne_prime);
                sig[k] = min(sig[k], hash_k);
                //cout << permutation[k].a << " " <<permutation[k].b << "," <<  cell << "," << hash_k << "," << sig[k] << ":" << k << endl;
            }
        }
    }
    //cout << "--------kmins cell level----------" << endl;
}

/*
the size of $sig can be smaller than K
*/
void bottomK::computeSig(const unordered_set<string>& vals) {
    sig.clear();
    priority_queue<int, vector<int>, std::greater<int>> bottom_k;
    for (const auto& val : vals) {
        int cell = hash<string>{}(val) % mersenne_prime;
        int hash_val = (int)(((long long)(permutation.a) * cell + permutation.b) % permutation.c);//mersenne_prime);
        bottom_k.push(hash_val);
        if (bottom_k.size() > K) bottom_k.pop();
    }
    // is it with small cardinality?? < K
    while (!bottom_k.empty()) {
        sig.push_back(bottom_k.top());
        bottom_k.pop();
    }
    //cout << "--------bottom_k col level----------" << endl;
}

void bottomK::computeSig(const vector<vector<string>>& matrix) {
    sig.clear();
    priority_queue<int, vector<int>, std::greater<int>> bottom_k;
    for (int r = 1; r < matrix.size(); ++r) {
        string rid = matrix[r][0];
        for (int col = 1; col < matrix[r].size(); ++col)
        {
            string cid = matrix[0][col];
            string val = matrix[r][col];
            string combined = cid + " " + rid + " " + val;
            int cell = hash<string>{}(combined) % mersenne_prime;
            int hash_val = (int)(((long long)(permutation.a) * cell + permutation.b) % permutation.c); ;//mersenne_prime);
            bottom_k.push(hash_val);
            if (bottom_k.size() > K) bottom_k.pop();
        }
    }
    // is it with small cardinality?? < K
    while (!bottom_k.empty()) {
        sig.push_back(bottom_k.top());
        bottom_k.pop();
    }
    //cout << "--------bottom_k cell level----------" << endl;
}
