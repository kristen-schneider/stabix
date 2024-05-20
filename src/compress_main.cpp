#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

#include "blocks.h"
#include "compress.h"
#include "header.h"
#include "utils.h"

using namespace std;

int main(int argc, char* argv[]) {
    // COMPRESSION STEPS

    // 0. read config options
    string config_file = argv[1];
    cout << "Reading config options from: " << config_file << endl;
    map<string, string> config_options;
    config_options = read_config_file(config_file);
    add_default_config_options(config_options);

    string gwas_file = config_options["gwas_file"];
    // if block_size == "map", get block sizes by map file
    int block_size = -1;
    if (config_options["block_size"] == "map") {
        string map_file = config_options["map_file"];
        cout << "Making blocks by cM distance in map file: " << map_file << endl;
//        vector<int> chrm_block_bp_ends =
        block_size = 5000;
    }else{
        block_size = stoi(config_options["block_size"]);
    }
    string query_type = config_options["query_type"];
    string compressed_file = config_options["gwas_file"] + ".grlz";
    vector<string> codecs_list = split_string(config_options["codecs"], ',');

    cout << "\t...gwas_file: " << gwas_file << endl;
    cout << "\t...block_size: " << block_size << endl;
    cout << "\t...query_type: " << query_type << endl;
    cout << "\t...codecs_list: " << convert_vector_str_to_string(codecs_list) << endl;
    cout << "\t...compressed_file: " << compressed_file << endl;

    cout << "Done." << endl << endl;

    // 1. get file information to store in first half of compressed file header
    /* HEADER FIRST HALF
     * num_columns = None
     * num_blocks = None
     * column_names_list = None
     * codecs_list = None
     * codec_headers_list = None
     * block_sizes_tuple = None
     * */
    cout << "Reading file header..." << endl;
    // open gwas file and get first two lines
    ifstream file(gwas_file);
    string line_1;
    string line_2;
    getline(file, line_1);
    getline(file, line_2);
    // close file
    file.close();

    // get delimiter
    char delimiter = get_delimiter(line_1);
    // get column names
    vector<string> column_names_list = split_string(line_1, delimiter);
    string column_names_str = convert_vector_str_to_string(column_names_list);
    cout << "\t...column names: " << column_names_str << endl;
    // get number of columns
    int num_columns = column_names_list.size();
    cout << "\t...num columns: " << num_columns << endl;
    cout << "Done." << endl << endl;

    // setting up variables for block compression
    vector<vector<vector<string>>> all_blocks;
    vector<vector<string>> compressed_blocks;
    int num_blocks;

    // 2. sort file as needed
    // if query type is coordinate, sort file by chromosome and genomic position
    if (query_type == "coordinate") {
        cout << "File already sorted by chromosome and genomic position." << endl;

        // 3. get blocks
        cout << "Making blocks..." << endl;
        all_blocks = make_blocks(gwas_file, num_columns, block_size, delimiter);
        num_blocks = all_blocks.size();
        cout << "\t...made " << num_blocks << " blocks of size " << block_size  << " or less." << endl;
        cout << "Done." << endl << endl;


    }else if (query_type == "statistic") {
        string statistic = split_string(config_options["query_statistic"], ':')[0];
        int statistic_idx = get_index(column_names_list, statistic);
        cout << "Sorting file by " << statistic << " (column index: " << statistic_idx << ")" << endl;
        cout << "Done." << endl << endl;
    }

    // 4. compress blocks AND get second half of header
    cout << "Compressing blocks..." << endl;
    // compress each block and add to compressed_blocks
    for (auto const& block : all_blocks) {
        vector<string> compressed_block = compress_block(block, codecs_list);
        compressed_blocks.push_back(compressed_block);
    }

    // get block headers
    vector<vector<string>> block_headers;
    for (auto const& block : compressed_blocks) {
        vector<string> block_header = get_block_header(block);
        block_headers.push_back(block_header);
    }

    // compress block headers
    vector<string> compressed_block_headers;
    for (auto const& block_header : block_headers) {
        string string_block_header = convert_vector_str_to_string(block_header);
        string compressed_block_header = zlib_compress(string_block_header);
        compressed_block_headers.push_back(compressed_block_header);
    }
    cout << "Done." << endl << endl;

    // 5. get second half of header
    /* HEADER SECOND HALF
     * compressed block header ends
     * compressed block ends
     * block sizes (tuple)
     */
    // get ending index of each compressed block header and compressed block
    vector<string> block_header_end_bytes;
    vector<string> block_end_bytes;
    int curr_byte_idx = 0;
    for (int curr_block_idx = 0; curr_block_idx < num_blocks; curr_block_idx++) {
        int curr_block_header_length = compressed_block_headers[curr_block_idx].length();
        curr_byte_idx += curr_block_header_length;
        block_header_end_bytes.push_back(to_string(curr_byte_idx));
        int curr_block_length = get_block_length(compressed_blocks[curr_block_idx]);
        curr_byte_idx += curr_block_length;
        block_end_bytes.push_back(to_string(curr_byte_idx));
    }

    // get size of first and last block
    int first_block_size = all_blocks[0][0].size();
    int last_block_size = all_blocks[all_blocks.size() - 1][0].size();
    string block_sizes = to_string(first_block_size) + "," + to_string(last_block_size);

    vector<string> header = {
            to_string(num_columns),
            to_string(num_blocks),
            column_names_str,
            convert_vector_str_to_string(block_header_end_bytes),
            convert_vector_str_to_string(block_end_bytes),
            block_sizes
    };

    // 6. compress header
    cout << "Compressing header..." << endl;
    string header_str = convert_vector_str_to_string(header);
    string compressed_header = zlib_compress(header_str);
    cout << "Done." << endl << endl;


    // 7. open file to write compressed header and compressed blocks
    cout << "Writing compressed file to: " << compressed_file << endl;
    ofstream compressed_gwas;
    compressed_gwas.open(compressed_file);
    // clear file contents
    compressed_gwas.clear();

    // 8. write compressed header to file
    // reserve first 4 bytes for size of compressed header
    int compressed_header_size = compressed_header.length();
    char * compressed_header_size_bytes = int_to_bytes(compressed_header_size);
    compressed_gwas.write(compressed_header_size_bytes, 4);
    // write compressed header to file
    compressed_gwas << compressed_header;

    // 9. write compressed blocks to file
    int block_idx = 0;
    for (auto const& block : compressed_blocks) {
        // write compressed block header
        string compressed_block_header = compressed_block_headers[block_idx];
        compressed_gwas << compressed_block_header;

        // write compressed block
        for (auto const& column : block) {
            compressed_gwas << column;
        }
        block_idx++;
    }
    compressed_gwas.close();
    cout << "Done." << endl;

    return 0;
}
