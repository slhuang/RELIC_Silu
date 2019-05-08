#pragma
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../profiling/unionfind.h"

struct edge {
    string u, v;
    double w; // u <-> v with weight w
    edge(string _u, string _v, double _w) {
        u = _u; v = _v; w = _w;
    }
};

struct graph {
    int m, n; // number of edges, number of nodes
    vector<edge> E;
};

// given a graph G, and a partially constructed mst (based on some constraint)
// construct the remaining mst 
class kruskal {
    graph G, mst; // the constructed graph G
    unionfind uf;
    public:
        kruskal(){};
        kruskal(int _n, const vector<edge>& _E){
            initialize_G(_n, _E);
        }
        kruskal(int _n, const vector<edge>& _E, const vector<edge>& _mst_E)
        {
            G.n = _n; G.m = _E.size(); G.E = _E;
            mst.n = _n; 
            insert_mst_edges(_mst_E);
            // mst.m = _mst_E.size(); mst.E = _mst_E; // mst is partially initialized with edges in _mst
            // for (const auto& e : mst.E) {
            //     uf.un(e.u, e.v); // construct the unionfind for existing mst
            // }
        }
        void initialize_G(int _n, const vector<edge>& _E) {
            G.n = _n; G.m = _E.size(); G.E = _E;
        }
        void insert_mst_edges(const vector<edge>& es) {
            for (const auto& e : es) {
                mst.E.push_back(e); ++mst.m;
                uf.un(e.u, e.v);
            }
        }
        void kruskalMST();
        vector<edge>& get_mst_edges() {return mst.E;}
        void printMST(const string &file = "./src/preserving_ops/infered_mst.csv");
};