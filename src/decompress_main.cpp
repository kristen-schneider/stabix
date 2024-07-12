#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "decompress.h"
#include "header.h"
#include "index.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    // DECOMPRESSION STEPS

    // 0. read config options
    if (argc != 2) {
        // prevent seg faults
        cout << "1 argument required: config_path" << endl;
        return -1;
    }
    string config_file = argv[1];
    cout << "Reading config options..." << endl;
    cout << "\t..." << config_file << endl;
    map<string, string> config_options;
    config_options = read_config_file(config_file);
    string query_type = config_options["query_type"];
    vector<string> col_codecs = split_string(config_options["codecs"], ',');

    // open output file in truncate mode
    ofstream output_file;
    string output_file_name = config_options["gwas_file"] + ".query";
    cout << "Opening output file: " << output_file_name << endl;
    output_file.open(output_file_name, ios::trunc);
    if (!output_file.is_open()) {
        cout << "Error: could not open output file" << endl;
        return 1;
    }
    // clear contents of output file and close
    output_file.close();
    output_file.open(output_file_name, ios::app);
    cout << "Done." << endl << endl;

    // 1. open compressed file and read header
    cout << "Opening compressed file and reading header..." << endl;
    string compressed_file = config_options["gwas_file"] + ".grlz";
    ifstream file(compressed_file);
    // start at beginning and read 4 bytes
    file.seekg(0, ios::beg);
    char header_length_bytes[4];
    file.read(header_length_bytes, 4);
    // convert 4 bytes to int
    int header_length = bytes_to_int(header_length_bytes);

    // read header_length bytes starting at byte 4
    file.seekg(4, ios::beg);
    char header_bytes[header_length];
    file.read(header_bytes, header_length);
    // convert header bytes to string
    string header_string = string(header_bytes, header_length);
    // decompress header
    string header = zlib_decompress(header_string);
    vector<string> header_list = split_string(header, ',');

    // 2. parse header
    string num_columns = parse_header_list(header_list, "num columns")[0];
    string num_blocks = parse_header_list(header_list, "num blocks")[0];
    vector<string> column_names_list =
        parse_header_list(header_list, "column names");
    vector<string> block_header_end_bytes_list =
        parse_header_list(header_list, "block header end bytes");
    vector<string> block_end_bytes_list =
        parse_header_list(header_list, "block end bytes");
    vector<string> block_sizes_list =
        parse_header_list(header_list, "block sizes");
    cout << "Done." << endl << endl;

    if (query_type == "coordinate") {
        // 3. opening index file
        string index_file = compressed_file + ".idx";
        cout << "Opening index file..." << endl;
        cout << "\t..." << index_file << endl;
        ifstream index(index_file);
        // make map of index file
        map<int, map<int, tuple<int, int>>> index_file_map =
            read_index_file(index_file);
        map<int, int> index_block_map = make_index_block_map(index_file);
        index.close();
        cout << "Done." << endl << endl;

        // 4. get start byte, end byte, start block idx, and end block idx for
        // each query
        vector<string> query_list =
            split_string(config_options["query_coordinate"], ',');
        vector<tuple<int, int>> all_query_block_indexes =
            get_start_end_block_idx(query_list, index_file_map,
                                    index_block_map);

        // 5. decompress all blocks for each query
        size_t compressedSize = 0; // Define compressedSize
        cout << "Decompressing all blocks for each query..." << endl;
        for (int q_idx = 0; q_idx < all_query_block_indexes.size(); q_idx++) {
            cout << "...decompressing query " << q_idx + 1 << ": "
                 << query_list[q_idx] << endl;

            output_file << "Query: " << query_list[q_idx] << endl;

            tuple<int, int> query_start_end_byte =
                all_query_block_indexes[q_idx];
            int start_block_idx = get<0>(query_start_end_byte);
            int end_block_idx = get<1>(query_start_end_byte);
            // if start_block_idx == -1, or end_block_idx == -1 then query is
            // not in index
            if (start_block_idx == -1 || end_block_idx == -1) {
                cout << "Query not found in index" << endl;
                output_file << " !! Query not found in index !! " << endl;
                continue;
            }
            int block_header_length;
            int block_length;
            for (int block_idx = start_block_idx; block_idx <= end_block_idx;
                 block_idx++) {
                size_t block_size = -1;
                // if there are only two block sizes, block size is fixed except
                // for last block
                if (block_sizes_list.size() == 2) {
                    if (block_idx < stoi(num_blocks) - 1) {
                        block_size = stoi(block_sizes_list[0]);
                    } else if (block_idx == stoi(num_blocks) - 1) {
                        block_size = stoi(block_sizes_list[1]);
                    }
                } else {
                    // if there are more than two block sizes, block size is
                    // variable
                    block_size = stoi(block_sizes_list[block_idx]);
                }
                vector<string> decompressed_block;
                if (block_idx == 0) {
                    block_header_length =
                        stoi(block_header_end_bytes_list[block_idx]);
                    block_length = stoi(block_end_bytes_list[block_idx]) -
                                   block_header_length;
                } else {
                    block_header_length =
                        stoi(block_header_end_bytes_list[block_idx]) -
                        stoi(block_end_bytes_list[block_idx - 1]);
                    block_length = stoi(block_end_bytes_list[block_idx]) -
                                   stoi(block_end_bytes_list[block_idx - 1]) -
                                   block_header_length;
                }
                int start_byte = get_start_byte(block_idx, index_block_map);
                cout << "......decompressing block " << block_idx
                     << ", size: " << block_size << endl;

                output_file << "Block: " << block_idx << endl;

                file.seekg(start_byte, ios::beg);
                char block_header_bytes[block_header_length];
                file.read(block_header_bytes, block_header_length);
                // decompress block header
                string block_header_bitstring =
                    string(block_header_bytes, block_header_length);
                string block_header = zlib_decompress(block_header_bitstring);
                vector<string> block_header_list =
                    split_string(block_header, ',');
                // read in compressed block
                char block_bytes[block_length];
                file.read(block_bytes, block_length);
                // convert to string
                string block_bitstring = string(block_bytes, block_length);
                // decompress block by column
                int curr_block_byte = 0;
                int col_bytes;
                for (int col_idx = 0; col_idx < stoi(num_columns); col_idx++) {
                    string col_codec = col_codecs[col_idx];
                    if (col_idx == 0) {
                        col_bytes = stoi(block_header_list[col_idx]);
                    } else if (col_idx == stoi(num_columns) - 1) {
                        col_bytes =
                            block_length - stoi(block_header_list[col_idx - 1]);
                    } else {
                        col_bytes = stoi(block_header_list[col_idx]) -
                                    stoi(block_header_list[col_idx - 1]);
                    }
                    string col_bitstring =
                        block_bitstring.substr(curr_block_byte, col_bytes);
                    curr_block_byte += col_bytes;
                    compressedSize = col_bitstring.size();
                    string col_decompressed = decompress_column(
                        col_bitstring, col_codec, compressedSize, block_size);
                    decompressed_block.push_back(col_decompressed);
                }

                // write decompressed block to output file
                cout << "......writing decompressed block to output file" << endl; for (int record_i = 0;
                record_i <= block_size-1; record_i++) {
                    for (int col_i = 0; col_i <=
                    stoi(num_columns)-1; col_i++) {
                        vector<string> block_list =
                        split_string(decompressed_block[col_i], ',');
                        // debug statemnt
//                        if (col_i == 0){
//                            cout << record_i << " " << col_i << " " << block_list.size() << "\n";
//                        }
                        output_file << block_list[record_i] << ',';
                    }
                    output_file << endl;
                }
            }
        }
    } else if (query_type == "statistic") {
        cout << "statistic-based indexing not yet set up" << endl;
    }

    output_file.close();
    return 0;
}
