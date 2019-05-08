#include "../../include/preserving_ops/preserving_ops.h"

/* compile
g++ -o src/preserving_ops/preserving_ops src/preserving_ops/preserving_ops_test.cpp \
src/pre_clustering/pre_clustering.cpp src/preserving_ops/mincut.cpp src/preserving_ops/kruskal.cpp \
src/profiling/schema_matching.cpp src/preserving_ops/preserving_ops.cpp -std=c++14 -O3
*/

vector<string> file_paths;

void parser(int argc, char *argv[])
{
    for (int ptr = 1; ptr < argc; ++ptr)
    {
        string cur = argv[ptr];
        if (cur == "-dir")
        {
            ++ptr;
            cout << argv[ptr];
            istringstream ss(argv[ptr]);
            while (ss.good())
            {
                string path;
                getline(ss, path, ',');
                if (!path.empty())
                    file_paths.push_back(path);
            }
            cout << "--------- total number of files: " << file_paths.size() << " ---------" << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    parser(argc, argv);
    preserving_ops ops(file_paths);
    ops.cell_level_jaccard();
    ops.column_level_jaccard();
    ops.relational_sim();
    ops.print_sim_scores();
    ops.lineage_construction_mst("cell");
    ops.lineage_construction_mst("col");
    ops.lineage_construction_mst("rel");
}