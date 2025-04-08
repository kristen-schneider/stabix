#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "blocks.h"
#include "compress.h"
#include "curl_stream.h"
#include "stabix_except.h"
#include "utils.h"

using namespace std;
namespace fs = std::filesystem;

// TODO: make this autodetect the chrm and bp columns
//  OR include in config file in case not 1 and 2
const int chrm_idx = 0;
const int bp_idx = 1;

struct IndexEntry {
    double value;    // floating point value to sort by
    int blockNumber; // Block number in the compressed file
};

int parse_chromosome(const string &chrom_str) {
    try {
        return stoi(chrom_str);
    } catch (const invalid_argument &) {
        if (chrom_str == "X")
            return 23;
        if (chrom_str == "Y")
            return 24;
        if (chrom_str == "MT")
            return 25;
        throw invalid_argument("Invalid chromosome: " + chrom_str);
    }
}

vector<vector<vector<string>>>
make_blocks(string gwas_file, int num_columns, int block_size, char delim,
            vector<vector<unsigned int>> &genomic_index) {

    vector<vector<vector<string>>> all_blocks;

    unique_ptr<istream> gwas_ptr;
    if (is_url(gwas_file)) {
        gwas_ptr = make_unique<CurlStream>(gwas_file);
    } else {
        gwas_ptr = make_unique<ifstream>(gwas_file);
        if (!static_cast<ifstream *>(gwas_ptr.get())->is_open()) {
            throw StabixExcept("Failed to open file: " + gwas_file);
        }
    }

    istream &gwas = *gwas_ptr;

    string line;
    int line_count = 0;
    int block_count = 0;
    vector<vector<string>> curr_block(num_columns);

    getline(gwas, line); // Ignore header

    vector<unsigned int> block_genomic_index(4, 0);

    while (getline(gwas, line)) {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        if (line_count < block_size) {
            vector<string> line_vector = split_string(line, delim);
            // Process chromosome and update genomic index
            istringstream line_stream(line);
            string column_value;
            int column_idx = 0;

            int curr_chrm;
            try {
                curr_chrm = parse_chromosome(line_vector[chrm_idx]);
            } catch (const exception &e) {
                cerr << "Skipping invalid line: " << e.what() << endl;
                continue;
            }
            // Add to curr_block
            for (size_t i = 0; i < line_vector.size(); ++i) {
                if (i >= num_columns) {
                    throw StabixExcept("Invalid number of columns (" +
                                       std::to_string(line_vector.size()) +
                                       ") in line: " + line);
                }
                curr_block[i].push_back(line_vector[i]);
            }
            // Update genomic index on first line of block
            if (line_count == 0) {
                // if this is the first line in the block, store the genomic
                // index starting chrm, starting bp, and line number
                block_genomic_index[0] = curr_chrm;
                block_genomic_index[1] = stoi(line_vector[bp_idx]);
                block_genomic_index[2] = 1 + block_count * block_size;
                genomic_index.push_back(block_genomic_index);
            }
            line_count++;
        } else {
            all_blocks.push_back(curr_block);
            curr_block = vector<vector<string>>(num_columns);
            line_count = 0;
            block_count++;

            // Process current line for new block

            vector<string> line_vector = split_string(line, delim);
            int curr_chrm;
            try {
                curr_chrm = parse_chromosome(line_vector[chrm_idx]);
            } catch (const exception &e) {
                cerr << "Skipping invalid line: " << e.what() << endl;
                continue;
            }

            for (size_t i = 0; i < line_vector.size(); ++i) {
                if (i >= num_columns) {
                    throw StabixExcept("Invalid number of columns (" +
                                       std::to_string(line_vector.size()) +
                                       ") in line: " + line);
                }
                curr_block[i].push_back(line_vector[i]);
            }
            line_count = 1;
            // Update genomic index
            try {
                // store the genomic index
                // starting chrm, starting bp, and line number
                // try and convert chrm and bp to ints
                block_genomic_index[0] = curr_chrm;
                block_genomic_index[1] = stoi(line_vector[bp_idx]);
                block_genomic_index[2] = 1 + block_count * block_size;
                genomic_index.push_back(block_genomic_index);
            }
            // catch stoi exceptions for sex chromosomes
            catch (const std::invalid_argument &e) {
                // do nothing, just catch
            } catch (const std::out_of_range &e) {
                // do nothing, just catch
            }
        }
    }

    if (!curr_block.empty() && !curr_block[0].empty()) {
        all_blocks.push_back(curr_block);
    }

    return all_blocks;
}

vector<vector<vector<string>>>
make_blocks_map(string gwas_file, int num_columns,
                const map<int, vector<uint32_t>> &chromosome_blocks, char delim,
                vector<vector<unsigned int>> &genomic_index) {
    vector<vector<vector<string>>> all_blocks;

    // Handle URL/file input
    unique_ptr<istream> gwas_ptr;
    if (is_url(gwas_file)) {
        gwas_ptr = make_unique<CurlStream>(gwas_file);
    } else {
        gwas_ptr = make_unique<ifstream>(gwas_file);
        if (!static_cast<ifstream *>(gwas_ptr.get())->is_open()) {
            throw StabixExcept("Failed to open file: " + gwas_file);
        }
    }
    istream &gwas = *gwas_ptr;

    // Skip header
    string line;
    getline(gwas, line);

    vector<vector<string>> curr_block(num_columns);
    int current_chromosome = 1;
    unsigned total_line_count = 0;
    size_t current_block_idx = 0;
    uint32_t current_bp_end =
        chromosome_blocks.at(current_chromosome).at(current_block_idx);

    auto commit_block = [&]() {
        if (!curr_block.empty() && !curr_block[0].empty()) {
            all_blocks.push_back(curr_block);
        }
        curr_block = vector<vector<string>>(num_columns);
    };

    while (getline(gwas, line)) {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        vector<string> line_vector = split_string(line, delim);

        // Validate columns
        if (line_vector.size() < num_columns) {
            throw StabixExcept("Insufficient columns in line: " + line);
        }

        // Parse chromosome and BP
        int chrom;
        uint32_t bp;
        try {
            chrom = parse_chromosome(line_vector[chrm_idx]);
            bp = stoul(line_vector[bp_idx]);
        } catch (const exception &e) {
            cerr << "Skipping invalid line: " << e.what() << endl;
            continue;
        }

        // Handle chromosome change
        if (chrom != current_chromosome) {
            commit_block();
            current_chromosome = chrom;
            current_block_idx = 0;
            try {
                current_bp_end = chromosome_blocks.at(current_chromosome)
                                     .at(current_block_idx);
            } catch (const out_of_range &) {
                throw StabixExcept("No blocks defined for chromosome " +
                                   to_string(current_chromosome));
            }
        }

        // Add to block while BP <= current_bp_end
        if (bp <= current_bp_end) {
            for (int i = 0; i < num_columns; ++i) {
                curr_block[i].push_back(line_vector[i]);
            }
            total_line_count++;
        } else {
            // Finalize current block and start new
            commit_block();
            genomic_index.push_back({
                static_cast<unsigned>(current_chromosome), bp,
                total_line_count + 1,
                0 // byte_offset (unsupported in this implementation)
            });

            // Advance to next block
            current_block_idx++;
            try {
                current_bp_end = chromosome_blocks.at(current_chromosome)
                                     .at(current_block_idx);
            } catch (const out_of_range &) {
                throw StabixExcept("No more blocks on chromosome " +
                                   to_string(current_chromosome));
            }

            // Add line to new block
            for (int i = 0; i < num_columns; ++i) {
                curr_block[i].push_back(line_vector[i]);
            }
            total_line_count++;
        }
    }

    // Add final block
    commit_block();
    return all_blocks;
}

/*
 * Add the byte starting of each block to the genomic index
 * @param compressed_header_size: int of compressed header size
 * @param block_end_bytes: vector<string> of block end bytes
 * @param genomic_index: vector<vector<int>> of genomic index
 */
void get_byte_start_of_blocks(int compressed_header_size,
                              vector<string> block_end_bytes,
                              vector<vector<unsigned int>> &genomic_index) {

    // for each block, start byte =
    // header bytes + compressed header size + end of last block
    // 4 + compressed_header_size + block_end_bytes[block_idx - 1]
    for (int block_idx = 0; block_idx < block_end_bytes.size(); block_idx++) {
        if (block_idx == 0) {
            genomic_index[block_idx][3] = 4 + compressed_header_size;
        } else {
            genomic_index[block_idx][3] = 4 + compressed_header_size +
                                          stoul(block_end_bytes[block_idx - 1]);
        }
    }
}

/*
 * Return the header of a block
 * @param compressed_block: vector<string> of compressed columns
 * @return block_header_column_end_bytes: vector<string> of block header column
 * end bytes
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
vector<string> compress_block(int block_idx, vector<vector<string>> block,
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
