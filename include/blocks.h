#ifndef GWAS_COMPRESS_CPP_BLOCKS_H
#define GWAS_COMPRESS_CPP_BLOCKS_H

#endif //GWAS_COMPRESS_CPP_BLOCKS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

vector<vector<vector<string>>> make_blocks(
        string gwas_file,
        int num_columns,
        int block_size,
        char delim,
        vector<vector<int>> &genomic_index);


vector<vector<vector<string>>> make_blocks_map(
        string gwas_file,
        int num_columns,
        map<int, vector<uint32_t>> chrm_block_bp_ends,
        char delim,
        vector<vector<int>> &genomic_index);


void get_byte_start_of_blocks(
        int compressed_header_size,
        vector<string> block_header_end_bytes,
        vector<string> block_end_bytes,
        vector<vector<int>> &genomic_index);


vector<string> get_block_header(
        vector<string> compressed_block);

int get_block_length(
        vector<string> compressed_block);

vector<string> compress_block(
        vector<vector<string>> block,
        vector<string> codecs_list);
