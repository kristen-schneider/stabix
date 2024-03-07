#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <fstream>
#include <sstream>
#include <map>
#include <tuple>

using namespace std;

vector<tuple<int, int, int>> get_chrm_bp_byte(
        string gwas_file,
        char delimiter,
        int header_length,
        int chrm_col,
        int bp_col,
        vector<string> block_header_end_bytes,
        vector<string> block_end_bytes);

map<string, map<string, tuple<string, string>>> read_index_file(
        string index_file);

tuple<int, int> find_query_start_byte(
        int q_chrm,
        int q_bp,
        map<string, map<string, tuple<string, string>>> index_map);