#pragma once
#include <string.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <iostream>
#include <queue>

using namespace std;

const int mersenne_prime = ((long long)(1) << 31) - 1; // change to longlong 1 << 61 -1?


struct hashFunc {
    int a;
    int b;
    int c;
    bool is_prime(long input){
        for(int i=2; i<sqrt(input);i++){
            if(input%i==0)
                return false;
        }
        return true;
    }
    hashFunc(){
        a = rand() % mersenne_prime;
        b = rand() % mersenne_prime;
        long cur_c = rand() % mersenne_prime + (long)mersenne_prime;
		//fprintf(stderr,"hash function %d: (%d*x+%d) mod %d\n", i+1, a[i],b[i],cur_c);
		while(!is_prime(cur_c)){
			cur_c = rand() % mersenne_prime + (long)mersenne_prime;
		}
		c = cur_c;
        cout << a << " --------- " << b << endl;
    };
    hashFunc(int _a, int _b, int _c): a(_a), b(_b), c(_c){};
};

class minHash {
protected:
    static int K;
    vector<int> sig;
public:
    static int get_K() {return K;}
    static void set_K(int _K) {
        K = _K;
    }
    virtual void computeSig(const unordered_set<string>& vals) = 0;  
    virtual void computeSig(const vector<vector<string>>& matrix) = 0;
    vector<int> return_sig() {return sig;}
    virtual ~minHash() {}
};

class kmins: public minHash {
    static vector<hashFunc> permutation; // K hash_function
public:
    kmins() {}
    static void initialize(int _K) {
        set_K(_K);
        permutation.clear();
        for (int i = 0; i < _K; ++i) {
            permutation.push_back(hashFunc());
        }
    }
    static vector<hashFunc> get_perm() {return permutation;}
    void computeSig(const unordered_set<string>& vals);  
    void computeSig(const vector<vector<string>>& matrix);
    ~kmins(){}
};

class bottomK: public minHash {
    static hashFunc permutation; // one hash_function
public:
    bottomK() {}
    static void initialize (int _K) {
        set_K(_K);
        permutation = hashFunc();
    }
    static hashFunc get_perm() {return permutation;}
    void computeSig(const unordered_set<string>& vals);  
    void computeSig(const vector<vector<string>>& matrix);
    ~bottomK(){}
};

