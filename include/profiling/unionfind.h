#pragma once
#include <string.h>
#include <unordered_map>
using namespace std;

class unionfind
{
    unordered_map<string, string> parent;

  public:
    string find(string col)
    {
        if (parent.count(col) == 0)
            parent[col] = col;
        while (parent[col] != col)
        {
            parent[col] = parent[parent[col]]; // half the path
            col = parent[col];
        }
        return col;
    }
    void un(const string &col_a, const string &col_b)
    { // union two names
        string pa = find(col_a), pb = find(col_b);
        if (pa == pb)
            return;
        // merge
        parent[pa] = pb;
    }
};