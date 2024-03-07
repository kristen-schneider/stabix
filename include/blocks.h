#ifndef GWAS_COMPRESS_CPP_BLOCKS_H
#define GWAS_COMPRESS_CPP_BLOCKS_H

#endif //GWAS_COMPRESS_CPP_BLOCKS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

vector<vector<vector<string>>> make_blocks(
        string gwas_file,
        int num_columns,
        int block_size,
        char delim);

vector<string> get_block_header(
        vector<string> compressed_block);

int get_block_length(
        vector<string> compressed_block);