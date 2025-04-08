#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

/*
 * Split a file into blocks.
 * --Read in gwas file and make a new block every block_size lines
 * @param gwas_file: string of gwas file
 * @param num_columns: int of number of columns in gwas file
 * @param block_size: int of block size
 * @param delim: char of delimiter
 * @param genomic_index: vector<vector<int>> genomic index: chrm, bp, line_number, byte_offset
 * @return all_blocks: vector<vector<vector<string>>> of blocks in gwas file
 */
vector<vector<vector<string>>> make_blocks(
        string gwas_file,
        int num_columns,
        int block_size,
        char delim,
        vector<vector<unsigned int>> &genomic_index);

/*
 * Split a file into blocks, using a MAP file.
 * --Read in gwas file and make a new block every 1cM
 * @param gwas_file: string of gwas file
 * @param num_columns: int of number of columns in gwas file
 * @param chrm_block_bp_ends: ending of each 1cM block
 * @param delim: char of delimiter
 * @param genomic_index: vector<vector<int>> genomic index: chrm, bp,
 * line_number, byte_offset
 * @return all_blocks: vector<vector<vector<string>>> of blocks in gwas file
 */
vector<vector<vector<string>>> make_blocks_map(
        string gwas_file,
        int num_columns,
        const map<int, vector<uint32_t>>& chromosome_blocks,
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
        int block_idx,
        vector<vector<string>> block,
        vector<string> codecs_list);
