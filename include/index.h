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
        vector<string> block_end_bytes,
        vector<string> codecs_list,
        vector<string> block_sizes_list);

map<int, map<int, tuple<int, int>>> read_index_file(
        string index_file);

map<int, map<int, vector<int>>> read_genomic_index_by_location(
        string genomic_index_file);

map<int, vector<int>> read_genomic_index_by_block(
        string genomic_index_file);

map<int, int> make_index_block_map(
        string index_file);

int get_block_idx(
        int q_chrm,
        int q_bp,
        map<int, map<int, tuple<int, int>>> index_file_map);

vector<tuple<int, int>> get_start_end_block_idx(
        vector<string> query_list,
        map<int, map<int, vector<int>>> genomic_index_info_by_location,
        map<int, vector<int>> genomic_index_info_by_block);

int get_start_byte(
        int block_idx,
        map<int, vector<int>> index_block_map);