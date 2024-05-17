#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "utils.h"
#include "compress.h"

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

/*
 * Function to get the length of a block
 * @param compressed_block: vector<string> of compressed columns
 * @return block_length_bytes: int of block length in bytes
 */
int get_block_length(vector<string> compressed_block){
    int block_length_bytes = 0;
    for (auto const& column : compressed_block) {
        block_length_bytes += column.length();
    }
    return block_length_bytes;
}

/*
 * Function to compress a block
 * @param block: vector<vector<string>> - block of data
 * @param codecs_list: vector<string> - list of codecs to use for each column
 * @return compressed_block: vector<string> - compressed block of data
 */
vector<string> compress_block(vector<vector<string>> block,
                              vector<string> codecs_list){
    vector<string> compressed_block;

    for (int col_i = 0; col_i < block.size(); col_i++){
        if (codecs_list[col_i] == "zlib"){
            // convert column from vector of strings to one string
            string column_string = convert_vector_str_to_string(block[col_i]);
            // compress string with zlib
            string compressed_string = zlib_compress(column_string);
            compressed_block.push_back(compressed_string);
        }
        else if(codecs_list[col_i] == "fpfVB"){
            // convert column from vector of strings to vector of integers
            vector<uint32_t> column_ints = convert_vector_string_to_vector_int(block[col_i]);
            // compress vector of integers with fastpfor
            size_t compressedSize = 0;
            vector<uint32_t> compressed_arr = fastpfor_vb_compress(column_ints, compressedSize);
            // convert compressed array to string
            string compressed_string = convert_vector_uint32_to_string(compressed_arr.data(), compressedSize);
            compressed_block.push_back(compressed_string);
        }
        else {
            cout << "ERROR: Codec not recognized: " << codecs_list[col_i] << endl;
            exit(1);
        }

    }
    return compressed_block;
}
