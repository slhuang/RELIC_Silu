#include "../../include/profiling/schema_matching.h"
#include "../../include/profiling/row_matching.h"

/* compile
g++ -o src/profiling/profiling src/profiling/profiling_test.cpp src/profiling/schema_matching.cpp src/profiling/row_matching.cpp -std=c++14 -O3
*/

vector<string> file_paths;
string result_dir;

void parser (int argc, char* argv[]) {
	result_dir = "./result/"; // initialize the folder path
    for (int ptr = 1; ptr < argc; ++ptr) {
        string cur = argv[ptr];
        if (cur =="-dir") {
            ++ptr;
            cout << argv[ptr] << endl;
            istringstream ss(argv[ptr]);
            while (ss.good()) {
                string path;
                getline(ss, path, ',');
                if (!path.empty()) file_paths.push_back(path);
            }
            cout << "--------- total number of files: " << file_paths.size() << " ---------" << endl; 
        } else {
        	if (cur == "-result") {
        		++ptr;
				result_dir = argv[ptr];
        	}
        }
    }
}

int main (int argc, char* argv[]) {
    parser(argc, argv);
    schema_matching sm(file_paths);
    sm.initialize_synonyms("./src/profiling/synonyms/synonyms.csv");
    sm.load_header_and_sample_value(10); // load top-10 lines
    sm.schema_match_by_name_and_type();
    sm.write_mapping_to_file(result_dir + "/schema_matching.csv", result_dir + "global_cid_2_name.csv");
    row_matching rm(file_paths, sm.return_col_l2g());
    rm.load_files_and_calculate_stats();
    rm.detect_pk_by_sampling();
    rm.print_pk(result_dir + "/pk.csv");
    rm.row_match_by_pk_name_and_value();
    rm.write_matching_to_file(result_dir + "/row_matching.csv");
}