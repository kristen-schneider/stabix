#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "compress.h"
#include "stabix_except.h"
#include "utils.h"

using namespace std;
namespace fs = std::filesystem;

// TODO: make this autodetect the chrm and bp columns
//  OR include in config file in case not 1 and 2
const int chrm_idx = 0;
const int bp_idx = 1;

struct IndexEntry {
    double value; // floating point value to sort by
    int blockNumber; // Block number in the compressed file
};

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
        vector<vector<unsigned int>> &genomic_index){

    // to return (stores all blocks)
    vector<vector<vector<string>>> all_blocks;

    // open gwas file
    ifstream gwas(gwas_file);

    // read in lines
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

    // create an empty block index
    // chrm, bp, line_number, byte_offset
    vector<unsigned int> block_genomic_index = {0, 0, 0, 0};

    // read in lines
    while (getline(gwas, line)) {
        // if line_count is less than block_size, split line by column and add
        // to block remove newline character from line
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        if (line_count < block_size) {
            istringstream line_stream(line);
            string column_value;
            int column_idx = 0;
            // split line by delimiter and add to curr_block
            vector<string> line_vector = split_string(line, '\t');

            int curr_chrm;
            try{
                curr_chrm = stoi(line_vector[chrm_idx]);
            }
                // catch stoi exceptions for X and Y chromosomes
                // X --> 23, Y --> 24, M --> 25
            catch (const std::invalid_argument& e){
                if (line_vector[chrm_idx] == "X"){
                    curr_chrm = 23;
                }
                else if (line_vector[chrm_idx] == "Y"){
                    curr_chrm = 24;
                }
                else if (line_vector[chrm_idx] == "MT"){
                    curr_chrm = 25;
                }
                else{
                    cout << "Invalid chromosome: " << line_vector[chrm_idx] << endl;
                    // skip this line
                    continue;
                }
            }

            // if this is the first line in the block, store the genomic index
            // starting chrm, starting bp, and line number
            if (line_count == 0){
                block_genomic_index[0] = curr_chrm;
                block_genomic_index[1] = stoi(line_vector[bp_idx]);
                block_genomic_index[2] = 1 + block_count * block_size;
                genomic_index.push_back(block_genomic_index);
            }
            for (auto const &column_value : line_vector) {
                curr_block[column_idx].push_back(column_value);
                column_idx++;
            }

            line_count++;
        }
        // if line_count is equal to block_size, add block to all_blocks and
        // reset line_count and curr_block
        else {
            all_blocks.push_back(curr_block);
            curr_block.clear();
            // make curr_block a vector of empty strings
            for (int i = 0; i < num_columns; i++) {
                vector<string> column;
                curr_block.push_back(column);
            }
            // add line to curr_block as first line
            istringstream line_stream(line);
            string column_value;
            int column_idx = 0;
            vector<string> line_vector = split_string(line, '\t');

            int curr_chrm;
            try{
                curr_chrm = stoi(line_vector[chrm_idx]);
            }
                // catch stoi exceptions for X and Y chromosomes
                // X --> 23, Y --> 24
            catch (const std::invalid_argument& e){
                if (line_vector[chrm_idx] == "X"){
                    curr_chrm = 23;
                }
                else if (line_vector[chrm_idx] == "Y"){
                    curr_chrm = 24;
                }
                else if (line_vector[chrm_idx] == "MT"){
                    curr_chrm = 25;
                }
                else{
                    cout << "Invalid chromosome: " << line_vector[chrm_idx] << endl;
                    // skip this line
                    continue;
                }
            }

            while (getline(line_stream, column_value, delim)) {
                curr_block[column_idx].push_back(column_value);
                column_idx++;
            }

            line_count = 1;
            block_count++;

            // store the genomic index
            // starting chrm, starting bp, and line number
            // try and convert chrm and bp to ints
            try{
                block_genomic_index[0] = curr_chrm;
                block_genomic_index[1] = stoi(line_vector[bp_idx]);
                block_genomic_index[2] = 1 + block_count * block_size;
                genomic_index.push_back(block_genomic_index);
            }
            // catch stoi exceptions for sex chromosomes
            catch (const std::invalid_argument& e){
                // do nothing, just catch
            }
            catch (const std::out_of_range& e) {
                // do nothing, just catch
            }
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
 * Split a file into blocks, using a MAP file.
 * --Read in gwas file and make a new block every 1cM
 * @param gwas_file: string of gwas file
 * @param num_columns: int of number of columns in gwas file
 * @param chrm_block_bp_ends: ending of each 1cM block
 * @param delim: char of delimiter
 * @param genomic_index: vector<vector<int>> genomic index: chrm, bp, line_number, byte_offset
 * @return all_blocks: vector<vector<vector<string>>> of blocks in gwas file
 */
vector<vector<vector<string>>> make_blocks_map(
        string gwas_file,
        int num_columns,
        map<int, vector<uint32_t>> chrm_block_bp_ends,
        char delim,
        vector<vector<unsigned int>> &genomic_index) {

    // read in gwas file and make a new block every block_size lines
    // return a vector of blocks
    vector<vector<vector<string>>> all_blocks;
    ifstream gwas(gwas_file);
    string line;
    int line_count = 0;
    int total_line_count = 0;
    int block_count = 0;
    vector<vector<string>> curr_block;
    // make curr_block a vector of empty strings
    for (int i = 0; i < num_columns; i++) {
        vector<string> column;
        curr_block.push_back(column);
    }

    // ignore header
    getline(gwas, line);

    int block_chrm = 1;
    uint32_t curr_bp;
    int curr_block_idx = 0;
    int curr_bp_end = chrm_block_bp_ends[block_chrm][curr_block_idx];

    // create an empty block index
    // chrm, bp, byte_offset,
    vector<unsigned int> block_genomic_index = {0, 0, 0, 0};

    // read in lines
    while (getline(gwas, line)) {

        line_count++;
//        total_line_count++;
        // to block remove newline character from line
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        // read line and get chrm and bp
        istringstream line_stream(line);
        string column_value;
        vector<string> line_vector = split_string(line, '\t');

        int curr_chrm=-1;
        try{
            curr_chrm = stoi(line_vector[chrm_idx]);
        }
        // catch stoi exceptions for X and Y chromosomes
        // X --> 23, Y --> 24
        catch (const std::invalid_argument& e){
            if (line_vector[chrm_idx] == "X"){
                curr_chrm = 23;
            }
            else if (line_vector[chrm_idx] == "Y"){
                curr_chrm = 24;
            }
            else if (line_vector[chrm_idx] == "MT"){
                curr_chrm = 25;
            }
            else{
                cout << "Invalid chromosome: " << line_vector[chrm_idx] << endl;
                // skip this line
                continue;
            }
        }
        curr_bp = stoul(line_vector[bp_idx]);

        // if new chromosome, current block ends
        // add block to all_blocks and start a new block
        if (curr_chrm != block_chrm) {
            // add block to all_blocks
            if (!curr_block.empty()) {
                all_blocks.push_back(curr_block);
                block_count++;
            }
            curr_block.clear();
            // make curr_block a vector of empty strings
            for (int i = 0; i < num_columns; i++) {
                vector<string> column;
                curr_block.push_back(column);
            }

            // store the genomic index
            // starting chrm, starting bp, and line number
            block_genomic_index[0] = curr_chrm;
            block_genomic_index[1] = curr_bp;
            block_genomic_index[2] = 1 + total_line_count;
            genomic_index.push_back(block_genomic_index);

            // add line to curr_block
            for (int col_idx = 0; col_idx < num_columns; col_idx++) {
                curr_block[col_idx].push_back(line_vector[col_idx]);
            }

            // update
            block_chrm = curr_chrm;
            curr_block_idx = 0;
            curr_bp_end = chrm_block_bp_ends[block_chrm][curr_block_idx];
            line_count = 1;
            total_line_count++;
        }
        // keep adding to current block until bp is greater than bp_end
        else if (curr_bp <= curr_bp_end) {

            // if beginning of block, store the genomic index
            if (line_count == 1){
                block_genomic_index[0] = curr_chrm;
                block_genomic_index[1] = curr_bp;
                block_genomic_index[2] = 1 + total_line_count;
                genomic_index.push_back(block_genomic_index);
            }

            // add line to curr_block
            for (int col_idx = 0; col_idx < num_columns; col_idx++) {
                curr_block[col_idx].push_back(line_vector[col_idx]);
            }
            total_line_count++;

        }
        else {
            // current bp >= bp_end, add block to all_blocks and start a new block
            // add block to all_blocks
            if (!curr_block.empty()) {
                all_blocks.push_back(curr_block);
                block_count++;
            }
            curr_block.clear();
            // make curr_block a vector of empty strings
            for (int i = 0; i < num_columns; i++) {
                vector<string> column;
                curr_block.push_back(column);
            }

            // add line to curr_block
            for (int col_idx = 0; col_idx < num_columns; col_idx++) {
                curr_block[col_idx].push_back(line_vector[col_idx]);
            }

            line_count = 1;
            // store the genomic index
            // starting chrm, starting bp, and line number
            block_genomic_index[0] = curr_chrm;
            block_genomic_index[1] = curr_bp;
            block_genomic_index[2] = 1 + total_line_count;
            genomic_index.push_back(block_genomic_index);

            block_count++;
            curr_block_idx++;
            total_line_count++;

            curr_bp_end = chrm_block_bp_ends[curr_chrm][curr_block_idx];
        }
    }
    // add last block to all_blocks if it is not empty
    if (!curr_block.empty()) {
        all_blocks.push_back(curr_block);
        block_count++;
    }
    // close gwas file
    gwas.close();

    return all_blocks;
}


/*
 * Add the byte starting of each block to the genomic index
 * @param compressed_header_size: int of compressed header size
 * @param block_end_bytes: vector<string> of block end bytes
 * @param genomic_index: vector<vector<int>> of genomic index
 */
void get_byte_start_of_blocks(
        int compressed_header_size,
        vector<string> block_end_bytes,
        vector<vector<unsigned int>> &genomic_index){

    // for each block, start byte =
    // header bytes + compressed header size + end of last block
    // 4 + compressed_header_size + block_end_bytes[block_idx - 1]
    for (int block_idx = 0; block_idx < block_end_bytes.size(); block_idx++){
        if (block_idx == 0){
            genomic_index[block_idx][3] = 4 + compressed_header_size;
        }
        else{
            genomic_index[block_idx][3] = 4 + compressed_header_size + stoul(block_end_bytes[block_idx - 1]);
        }
    }
}


/*
 * Return the header of a block
 * @param compressed_block: vector<string> of compressed columns
 * @return block_header_column_end_bytes: vector<string> of block header column end bytes
 */
vector<string> get_block_header(
        vector<string> compressed_block) {
    vector<string> block_header_column_end_bytes;
    int curr_byte_idx = 0;
    // iterate over all column bitstrings and store the end byte of each column
    for (auto const &column : compressed_block) {
        int column_length = column.length(); // length of bitstring
        curr_byte_idx += column_length;      // end byte of column
        block_header_column_end_bytes.push_back(to_string(curr_byte_idx));
    }
    return block_header_column_end_bytes;
}

/*
 * Function to get the length of a block
 * @param compressed_block: vector<string> of compressed columns
 * @return block_length_bytes: int of block length in bytes
 */
unsigned int get_block_length(vector<string> compressed_block) {
    unsigned int block_length_bytes = 0;
    for (auto const &column : compressed_block) {
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
vector<string> compress_block(
        int block_idx,
        vector<vector<string>> block,
        vector<string> codecs_list) {
    // TODO: Somewhere, have a help message of available codecs and,
    //   Confirm codec is installed before attempting compression
    vector<string> compressed_block;

    for (int col_i = 0; col_i < block.size(); col_i++) {
        string codec = codecs_list[col_i];
        // TODO: remove codec "zlib" in favor of "deflate"?
        if (codec == "zlib") {
            // convert column from vector of strings to one string
            string column_string = convert_vector_str_to_string(block[col_i]);
            // compress string with zlib
            string compressed_string = zlib_compress(column_string);
            compressed_block.push_back(compressed_string);
        } else if (codec == "deflate") {
            // convert column from vector of strings to one string
            string column_string = convert_vector_str_to_string(block[col_i]);
            string compressed_string = deflate_compress(column_string);
            compressed_block.push_back(compressed_string);
        } else if (codec == "bz2") {
            // convert column from vector of strings to one string
            string column_string = convert_vector_str_to_string(block[col_i]);
            string compressed_string = bz2_compress(column_string);
            compressed_block.push_back(compressed_string);
        } else if (codec == "xz") {
            // convert column from vector of strings to one string
            string column_string = convert_vector_str_to_string(block[col_i]);
            string compressed_string = xz_compress(column_string);
            compressed_block.push_back(compressed_string);
        } else if (codec == "zstd") {
            // convert column from vector of strings to one string
            string column_string = convert_vector_str_to_string(block[col_i]);
            string compressed_string = zstd_compress(column_string);
            compressed_block.push_back(compressed_string);
        } else if (codec == "fpfVB") {
            // convert column from vector of strings to vector of integers
            vector<uint32_t> column_ints =
                convert_vector_string_to_vector_int(block[col_i]);
            // compress vector of integers with fastpfor
            size_t compressedSize = 0;
            vector<uint32_t> compressed_arr =
                fastpfor_vb_compress(column_ints, compressedSize);
            // convert compressed array to string
            string compressed_string = convert_vector_uint32_to_string(
                compressed_arr.data(), compressedSize);
            compressed_block.push_back(compressed_string);
        } else {
            throw StabixExcept("Invalid codec. " + codecs_list[col_i]);
        }
    }

    return compressed_block;
}
