#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

vector<vector<vector<string>>> make_blocks(
        string gwas_file,
        int num_columns,
        int block_size,
        char delim,
        vector<vector<unsigned int>> &genomic_index);


vector<vector<vector<string>>> make_blocks_map(
        string gwas_file,
        int num_columns,
        map<int, vector<uint32_t>> chrm_block_bp_ends,
        char delim,
        vector<vector<unsigned int>> &genomic_index);


void get_byte_start_of_blocks(
        int compressed_header_size,
        vector<string> block_end_bytes,
        vector<vector<unsigned int>> &genomic_index);


vector<string> get_block_header(
        vector<string> compressed_block);

unsigned int get_block_length(
        vector<string> compressed_block);

vector<string> compress_block(
        fs::path col_sizes_file,
        int block_idx,
        vector<vector<string>> block,
        vector<string> codecs_list);
