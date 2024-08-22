#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "compress.h"
#include "utils.h"

using namespace std;

struct IndexEntry {
    double value; // floating point value to sort by
    int blockNumber; // Block number in the compressed file
};

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
        char delim,
        int index_col){

    // read in gwas file and make a new block every block_size lines
    // return a vector of blocks
    // creates an index for floating point values in the index column
    vector<vector<vector<string>>> all_blocks;
//    vector<IndexEntry> indexEntries;

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
            for (auto const &column_value : line_vector) {
                curr_block[column_idx].push_back(column_value);
//                // if column is index column, store the value of the indexed column
//                if (column_idx == index_col){
//                    // index value is the value of the index column
//                    double index_value = stod(column_value);
//                    // store index value and block number in index entry
//                    IndexEntry index_entry = {index_value, block_count};
//                    indexEntries.push_back(index_entry);
//                }
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
            // add line to curr_block
            istringstream line_stream(line);
            string column_value;
            int column_idx = 0;
            while (getline(line_stream, column_value, delim)) {
                curr_block[column_idx].push_back(column_value);
//                if (column_idx == index_col){
//                    // index value is the value of the index column
//                    double index_value = stod(column_value);
//                    // store index value and block number in index entry
//                    IndexEntry index_entry = {index_value, block_count};
//                    indexEntries.push_back(index_entry);
//                }
                column_idx++;
            }
            line_count = 1;
            block_count++;
        }
    }
    // add last block to all_blocks if it is not empty
    if (!curr_block.empty()) {
        all_blocks.push_back(curr_block);
        block_count++;
    }
    gwas.close();

//    // sort index entries by value
//    sort(indexEntries.begin(), indexEntries.end(), [](const IndexEntry &a, const IndexEntry &b) {
//        return a.value < b.value;
//    });

//    // write index entries to file
//    string index_file = gwas_file + ".float.idx";
//    cout << "Writing index file to: " << index_file << endl;
//    ofstream index;
//    index.open(index_file);
//    // write header
//    index << "value,block_number" << endl;
//    // write index entries


    return all_blocks;
}

vector<vector<vector<string>>>
make_blocks_map(string gwas_file, int num_columns,
                map<int, vector<uint32_t>> chrm_block_bp_ends, char delim) {

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

    int block_chrm = 1;
    uint32_t curr_bp = 0;
    int curr_block_idx = 0;
    int curr_bp_end = chrm_block_bp_ends[block_chrm][curr_block_idx];

    // read in lines
    while (getline(gwas, line)) {
        // if line_count is less than block_size, split line by column and add
        // to block remove newline character from line
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        // read line and get chrm and bp
        istringstream line_stream(line);
        string column_value;
        vector<string> line_vector = split_string(line, '\t');
        int curr_chrm = stoi(line_vector[1]);
        curr_bp = stoul(line_vector[2]);
        // if new chromosome and not first chromosome
        if (curr_chrm != block_chrm) {
            // add block to all_blocks
            if (!curr_block[0].empty()) {
                all_blocks.push_back(curr_block);
            }
            curr_block.clear();
            // make curr_block a vector of empty strings
            for (int i = 0; i < num_columns; i++) {
                vector<string> column;
                curr_block.push_back(column);
            }
            block_chrm = curr_chrm;
            curr_block_idx = 0;
            curr_bp_end = chrm_block_bp_ends[curr_chrm][curr_block_idx];
        }
        // keep adding to current block until bp is greater than bp_end
        if (curr_bp <= curr_bp_end) {
            int column_idx = 0;
            // split line by delimiter and add to curr_block
            vector<string> line_vector = split_string(line, '\t');
            for (auto const &column_value : line_vector) {
                curr_block[column_idx].push_back(column_value);
                column_idx++;
            }
        } else {
            if (!curr_block[0].empty()) {
                all_blocks.push_back(curr_block);
            }
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
            block_count++;
            curr_block_idx++;
            curr_bp_end = chrm_block_bp_ends[curr_chrm][curr_block_idx];
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
vector<string> get_block_header(vector<string> compressed_block) {
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
int get_block_length(vector<string> compressed_block) {
    int block_length_bytes = 0;
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
vector<string> compress_block(vector<vector<string>> block,
                              vector<string> codecs_list) {
    // TODO: Somewhere, have a help message of available codecs and,
    // Confirm codec is installed before attempting compression
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
            cout << "ERROR: Codec not recognized: " << codecs_list[col_i]
                 << endl;
            exit(1);
        }
    }
    return compressed_block;
}
