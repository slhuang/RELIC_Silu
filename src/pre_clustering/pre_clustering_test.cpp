#include "../../include/pre_clustering/pre_clustering.h"

/* compile
g++ -o src/pre_clustering/pre_clustering src/pre_clustering/pre_clustering_test.cpp src/pre_clustering/pre_clustering.cpp -std=c++14 -O3
*/
bool EXACT_SCHEMA, PARTIAL_SCHEMA, PK;
string result_dir;

void initialize() {
    EXACT_SCHEMA = true;
    PARTIAL_SCHEMA = false;
    PK = false;
	result_dir = "./result/";
}

void parser(int argc, char *argv[])
{
	initialize();
    for (int ptr = 1; ptr < argc; ++ptr)
    {
        string cur = argv[ptr];
        if (cur == "-exact_schema") 
        { //default
            EXACT_SCHEMA = true;
            PARTIAL_SCHEMA = false;
            PK = false;
        } else 
        if (cur == "-partial_schema") {
            EXACT_SCHEMA = false;
            PARTIAL_SCHEMA = true;
            PK = false;
        } else 
        if (cur == "-PK") {
            EXACT_SCHEMA = false;
            PARTIAL_SCHEMA = false;
            PK = true;
        } else 
        if (cur == "-no_pre_cluster") {
            EXACT_SCHEMA = false;
            PARTIAL_SCHEMA = false;
            PK = false;
        } else 
			if (cur == "-result") {
				++ptr;
				result_dir = argv[ptr];
			}
    }
}

int main(int argc, char *argv[])
{
    parser(argc, argv);
    pre_clustering pc;
    pc.load_col_matching_from_file();
    if (EXACT_SCHEMA)
        pc.cluster_with_exact_schema();
    else if (PARTIAL_SCHEMA)
        pc.cluster_with_partial_schema();
    else if (PK)
        pc.cluster_with_pk();
    else
        pc.all_in_one_cluster();
    pc.write_clusters_to_file(result_dir + "/clusters.csv", result_dir + "/clusters_with_filename.csv");
}
