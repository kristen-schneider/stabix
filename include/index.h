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

map<int, map<int, vector<unsigned int>>> read_genomic_index_by_location(
        string genomic_index_file);

map<int, vector<unsigned int>> read_genomic_index_by_block(
        string genomic_index_file);

int get_block_idx(
        int q_chrm,
        int q_bp,
        map<int, map<int, vector<unsigned int>>> index_file_map);

tuple<int, int> get_start_end_block_idx_single(
        int gene_chrm,
        int gene_bp_start,
        int gene_bp_end,
        map<int, map<int, vector<unsigned int>>> genomic_index_info_by_location);

vector<tuple<int, int>> get_start_end_block_idx(
        vector<string> query_list,
        map<int, map<int, vector<int>>> genomic_index_info_by_location);

unsigned int get_start_byte(
        int block_idx,
        map<int, vector<unsigned int>> index_block_map);