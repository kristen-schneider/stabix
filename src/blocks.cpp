#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "utils.h"

using namespace std;

/*
 * Function to split a file into blocks
 * @param gwas_file: string of gwas file
 * @param num_columns: int of number of columns in gwas file
 * @param block_size: int of block size
 * @param delim: char of delimiter
 * @return all_blocks: vector<vector<vector<string>>> of blocks in gwas file
 */
vector<vector<vector<string>>> make_blocks(
        string gwas_file,
        int num_columns,
        int block_size,
        char delim){
    // read in gwas file and make a new block every block_size lines
    // return a vector of blocks
    vector<vector<vector<string>>> all_blocks;
    ifstream gwas(gwas_file);
    string line;
    int line_count = 0;
    int block_count = 0;
    vector<vector<string>> curr_block;
    // make curr_block a vector of empty strings
    for (int i = 0; i < num_columns; i++) {
        vector<string> column;
        curr_block.push_back(column);
    }

    // ignore header
    getline(gwas, line);

    // read in lines
    while (getline(gwas, line)) {
        // if line_count is less than block_size, split line by column and add to block
        // remove newline character from line
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        if (line_count < block_size) {
            istringstream line_stream(line);
            string column_value;
            int column_idx = 0;
            // split line by delimiter and add to curr_block
            vector<string> line_vector = split_string(line, '\t');
            for (auto const& column_value : line_vector) {
                curr_block[column_idx].push_back(column_value);
                column_idx++;
            }
            line_count++;
        }
        // if line_count is equal to block_size, add block to all_blocks and reset line_count and curr_block
        else {
            all_blocks.push_back(curr_block);
            curr_block.clear();
            // make curr_block a vector of empty strings
            for (int i = 0; i < num_columns; i++) {
                vector<string> column;
                curr_block.push_back(column);
            }
            // add line to curr_block
            istringstream line_stream(line);
            string column_value;
            int column_idx = 0;
            while (getline(line_stream, column_value, delim)) {
                curr_block[column_idx].push_back(column_value);
                column_idx++;
            }
            line_count = 0;
            block_count++;
        }
    }
    // add last block to all_blocks if it is not empty
    if (!curr_block.empty()) {
        all_blocks.push_back(curr_block);
        block_count++;
    }
    gwas.close();

    return all_blocks;
}

/*
 * Function to compress a block
 * @param block: vector<vector<string>> of block
 * @param codecs_list: vector<string> of codecs
 * @return compressed_block: vector<string> of compressed columns
 */
vector<string> get_block_header(vector<string> compressed_block){
    vector<string> block_header_column_end_bytes;
    int curr_byte_idx = 0;
    // iterate over all column bitstrings and store the end byte of each column
    for (auto const& column : compressed_block) {
        int column_length = column.length();    // length of bitstring
        curr_byte_idx += column_length;         // end byte of column
        block_header_column_end_bytes.push_back(to_string(curr_byte_idx));
    }
    return block_header_column_end_bytes;
}

int get_block_length(vector<string> compressed_block){
    int block_length_bytes = 0;
    for (auto const& column : compressed_block) {
        block_length_bytes += column.length();
    }
    return block_length_bytes;
}
