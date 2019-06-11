#include "../../include/preserving_ops/preserving_ops.h"

/* compile
g++ -o src/preserving_ops/preserving_ops src/preserving_ops/preserving_ops_test.cpp \
src/pre_clustering/pre_clustering.cpp src/preserving_ops/mincut.cpp src/preserving_ops/kruskal.cpp \
src/profiling/schema_matching.cpp src/preserving_ops/preserving_ops.cpp \
src/profiling/vp_row_matching.cpp src/index/minHash.cpp -std=c++14 -O3
*/

vector<string> file_paths;
string result_dir;
bool with_PK, APPROX;
int K;

void parser(int argc, char *argv[])
{
    with_PK = true; APPROX = false;
	result_dir = "./result/"; //initialize
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
        } else 
		if (cur == "-result") {
			++ptr;
			result_dir = argv[ptr];
		} else 
        if (cur == "-noPK") {
            with_PK = false;
        } else 
        if (cur == "-approx") {
            ++ptr;
            APPROX = true;
            K = stoi(argv[ptr]); // the sketch size
        }
    }
}

int main(int argc, char *argv[])
{
    parser(argc, argv);
    ofstream f_time("time.csv", ios::app);
    clock_t t_load = clock();
    preserving_ops ops(file_paths, with_PK);
    t_load = clock() - t_load;
    f_time << (APPROX?"APPROX,":"EXACT,") << t_load;
    if (APPROX) {
        ops.set_sketch_type("bottomK");
        ops.initialize_sketch(K);
        clock_t t_cell = clock();
        ops.cell_level_jaccard_approx(K);
        t_cell = clock() - t_cell;
        clock_t t_col = clock();
        ops.column_level_jaccard_approx(K);
        t_col = clock() - t_col;
        f_time << "," << t_cell << "," << t_col;
    } else {
        clock_t t_cell = clock();
        if (with_PK) {
            cout << "=========with PK===========" << endl;
            ops.cell_level_jaccard();
        }
        else {
            cout << "=========No PK===========" << endl;
            ops.cell_level_jaccard_no_pk();
        }
        t_cell = clock() - t_cell;
        clock_t t_col = clock();
        ops.column_level_jaccard();
        t_col = clock() - t_col;
        f_time << "," << t_cell << "," << t_col;
    }
    //ops.cell_level_jaccard();
    //ops.column_level_jaccard();
    //ops.relational_sim();
    if (APPROX) result_dir += "_approx";
    ops.print_sim_scores(result_dir);
    clock_t t_cell_mst = clock();
    ops.lineage_construction_mst("cell", result_dir);
    t_cell_mst = clock() - t_cell_mst;
    clock_t t_col_mst = clock();
    ops.lineage_construction_mst("col", result_dir);
    t_col_mst = clock() - t_col_mst;
    f_time << "," << t_cell_mst << "," << t_col_mst << endl;
    //ops.lineage_construction_mst("rel");
}