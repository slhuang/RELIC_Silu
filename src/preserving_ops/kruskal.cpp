#include "../../include/preserving_ops/kruskal.h"

void kruskal::kruskalMST() {
    auto cmp = [](const edge& e1, const edge&e2) {
        return e1.w < e2.w; // break the tie?
    };
    sort(G.E.begin(), G.E.end(), cmp);
    for (const auto& e : G.E) {
        if (e.w == 1) break; // there is no similarity between these two artifacts
        string parent_u = uf.find(e.u), parent_v = uf.find(e.v);
        if (parent_u == parent_v) continue; // will lead to a cycle, ignore this edge
        mst.E.push_back(e); ++mst.m;
        uf.un(e.u, e.v);
        if (mst.m == mst.n - 1) break; // already form a tree
    }
}

void kruskal::printMST(const string& file) {
    ofstream fout(file);
    for (const auto& e : mst.E) {
        fout << e.u << "," << e.v << "," << 1- e.w << endl;
        cout << e.u << "," << e.v << "," << 1- e.w << endl;
    }
}