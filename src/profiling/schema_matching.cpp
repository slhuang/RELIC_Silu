#include "../../include/profiling/schema_matching.h"

bool is_number(const string& s) {
    if (s == "NaN") return true;
    int dot_cnt = 0;
    for (const auto& c : s) {
        if (c == '.') {
            ++dot_cnt;
            if (dot_cnt > 1) return false;
        }else if (isdigit(c) == false)   return false;
    }
    return s.empty() || s.size() == 1 || dot_cnt == 1 || s[0] != '0'; //0, 0.0, 001
}

bool is_null_string(const string &s)
{
    for (const auto& c : s) {
        if (c != ' ') return false;
    }
    return true;
}

/*only consider "string" and "number", can be extended to "time", "path",etc. */
string infer_data_type(const string& val, string& curDataType) {
    if(is_null_string(val) && curDataType == "null") return "null";
    bool isNumber = is_number(val);
    if ((curDataType == "number" || curDataType == "null") && isNumber) return "number";
    return "string";
}

/*
Consider synonyms when schema_matching;
TODO: use embedding for schema_matching
*/
void schema_matching::initialize_synonyms(const string &file, const char& delimiter)
{
    ifstream fin(file);
    string line;
    while (getline(fin, line)) {
        istringstream ss(line); // each line is a set of synonyms
        string name1, name2;
        getline(ss, name1, delimiter);
        while (getline(ss, name2, delimiter)) {
            synonyms.un(name1, name2);
        }
    }
    cout << "-----------initialize synonyms------------\n";
}

/*load each file in the directory;
  for each file, load a bunch of sample records.
  Output (populate): $fileCols.
*/
void schema_matching::load_header_and_sample_value(int sampleCnt){
    for (int fid = 0; fid < filePaths.size(); ++fid) {
        ifstream fin(filePaths[fid]);
        string line;
        //cout << "----------------" << filePaths[fid] << "---------------\n";
        for (int lineId = 0; getline(fin, line), lineId < sampleCnt; ++lineId) {
            istringstream ss(line);
            int cid = 0, null_id = 0;  // column id
            while (ss.good() && ss >> std::ws) {
                string cell;
                if (ss.peek() == '"') { // handle quotes
                    if (ss.peek() == '"')
                    { // in case of [ \"Primary, Secondary, Third\", \"Primary\", , \"Secondary\", 18, 4, 0, 0, 0 ]
                        ss >> quoted(cell); // "Primary, Secondary, Third"
                        string discard;
                        getline(ss, discard, ',');
                        if (!is_null_string(discard)) cell = "\"" + cell + "\"" +discard; 
                    }
                } else {
                    getline(ss, cell, ',');
                }
                //cout << cid << " " << cell << endl;
                if (lineId == 0) {
                    if(is_null_string(cell)) cell = "null_" + to_string(null_id++);
                    fileCols[fid].push_back(col(cell));  // header
                    // column id, column name
                    //cout << fid << " " << fileCols[fid].size() << " " << cell << endl;
                }
                else {
                    fileCols[fid][cid].type = infer_data_type(cell, fileCols[fid][cid].type);
                    ++cid;
                }
            }
        }
    }
    cout << "--------schema matching load_header_and_sample_value-----------\n";
}

/* 
match schema based on the exact match on both name and data type.
input: $fileCols
output: $globalName2Id, $globalId2Name, $col_l2g, $col_g2l
$globalName2Id: globalName is 1-1 correspondance with the global_cid
*/
void schema_matching::schema_match_by_name_and_type(){
    string sep = ";";
    for (int fid = 0; fid < fileCols.size(); ++fid) { //
        for (int cid = 0; cid < fileCols[fid].size(); ++cid)
        {
            col column = fileCols[fid][cid];
            // global_name is consist of both data type and the column name in th eunionfind
            string global_name = column.type + sep + synonyms.find(column.name);
            //cout << fid << " " << cid << " " << global_name << endl;
            if (globalName2Id.count(global_name) == 0) {
                int id = globalName2Id.size();
                globalName2Id[global_name] = id;
                globalId2Name[id] = global_name;
            }
            int global_cid = globalName2Id[global_name];
            if (col_g2l[fid].count(global_cid))
            {
                int another_cid = col_g2l[fid][global_cid]; // another local cid mapped to the same global_cid
                cout << "===========ERROR=============" << endl;
                cout << column.name << " and " << fileCols[fid][another_cid].name << " are mapped to the same global_cid " << global_name << endl;
                exit(1);
            }
            col_g2l[fid][global_cid] = cid;
            col_l2g[fid][cid] = global_cid;
        }
    }
    cout << "----------------finish schema matching by name and type -------------------\n"; 
}

/*

*/
void schema_matching::write_mapping_to_file(const string &file, const string& file_cid2name)
{
    ofstream fout(file);
    for (int fid = 0; fid < col_g2l.size(); ++fid)
    { // file_name, file_id, # of columns
        fout << filePaths[fid] << "," << fid << "," << col_g2l[fid].size() << endl;
        for (const auto &global2local : col_g2l[fid])
        {
            fout << global2local.first << "," << global2local.second << endl; // global,local
        }
    }
    ofstream fout2(file_cid2name);
    for (const auto ele : globalId2Name) {
        fout2 << ele.first << "," << ele.second << endl; // cid, name
    }
}