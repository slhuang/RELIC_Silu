#include "../../include/profiling/schema_matching.h"
#include "../../include/profiling/row_matching.h"

/* compile
g++ -o src/profiling/profiling src/profiling/profiling_test.cpp src/profiling/schema_matching.cpp src/profiling/row_matching.cpp -std=c++14 -O3
*/

vector<string> file_paths;

void parser (int argc, char* argv[]) {
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
        }
    }
}

int main (int argc, char* argv[]) {
    parser(argc, argv);
    schema_matching sm(file_paths);
    sm.initialize_synonyms("./src/profiling/synonyms.csv");
    sm.load_header_and_sample_value(10); // load top-10 lines
    sm.schema_match_by_name_and_type();
    sm.write_mapping_to_file();
    row_matching rm(file_paths, sm.return_col_l2g());
    rm.load_files_and_calculate_stats();
    rm.detect_pk_by_sampling();
    rm.print_pk();
    rm.row_match_by_pk_name_and_value();
    rm.write_matching_to_file();
}