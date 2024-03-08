#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <map>

// include files from include directory
#include "utils.h"
#include "header.h"
#include "index.h"
#include "decompress.h"

using namespace std;

int main(int argc, char* argv[]) {
    // DECOMPRESSION STEPS

    // 0. read config options
    string config_file = argv[1];
    cout << "Reading config options..." << endl;
    cout << "\t..." << config_file << endl;
    map<string, string> config_options;
    config_options = read_config_file(config_file);
    vector<string> col_codecs = split_string(config_options["codecs"], ',');
    cout << "Done." << endl << endl;

    // 1. open compressed file
    cout << "Opening compressed file and reading header..." << endl;
    string compressed_file = config_options["gwas_file"] + ".grlz";
    ifstream file(compressed_file);
    // start at beginning and read 4 bytes
    file.seekg(0, ios::beg);
    char header_length_bytes[4];
    file.read(header_length_bytes, 4);
    // convert 4 bytes to int
    int header_length = bytes_to_int(header_length_bytes);
//    cout << "header length: " << header_length << endl;

    // read header_length bytes starting at byte 4
    file.seekg(4, ios::beg);
    char header_bytes[header_length];
    file.read(header_bytes, header_length);
    // convert header bytes to string
    string header_string = string(header_bytes, header_length);
    // decompress header
    string header = zlib_decompress(header_string);
    cout << "...header: " << header << endl;
    vector<string> header_list = split_string(header, ',');

    // 2. parse header
    string num_columns = parse_header_list(header_list, "num columns")[0];
    string num_blocks = parse_header_list(header_list, "num blocks")[0];
    vector<string> column_names_list = parse_header_list(header_list, "column names");
    vector<string> block_header_end_bytes_list = parse_header_list(header_list, "block header end bytes");
    vector<string> block_end_bytes_list = parse_header_list(header_list, "block end bytes");
    vector<string> block_sizes_list = parse_header_list(header_list, "block sizes");
    cout << "Done." << endl << endl;

    // 3. opening index file
    string index_file = compressed_file + ".idx";
    cout << "Opening index file..." << endl;
    cout << "\t..." << index_file << endl;
    ifstream index(index_file);
    // make map of index file
    map<string, map<string, tuple<string, string>>> index_map = read_index_file(index_file);
    cout << "Done." << endl << endl;

    // 4. search for queries
    vector<string> query_list = split_string(config_options["query_coordinate"], ',');
    cout << "Searching for queries..." << endl;
g    for (int q_idx = 0; q_idx < query_list.size(); q_idx++){
        int q_chrm = stoi(split_string(query_list[q_idx], ':')[0]);
        int q_bp = stoi(split_string(query_list[q_idx], ':')[1]);
        // byte_start is the first element in the tuple from index_map
        tuple<int, int> query_byte_block;
        int q_byte = get<0>(find_query_start_byte(q_chrm, q_bp, index_map));
        int block_idx = get<1>(find_query_start_byte(q_chrm, q_bp, index_map));
//        cout << "Query " << q_idx << " start byte: " << q_byte << endl;

        // 5. decompress (query) block
        cout << "Decompressing block..." << endl;
        // read in compressed block
        ifstream file(compressed_file);
        file.seekg(q_byte, ios::beg);

        // get block header and block length
        int block_header_length;
        int block_length;

        if (block_idx == 0){
            block_header_length = stoi(block_header_end_bytes_list[block_idx]);
            block_length = stoi(block_end_bytes_list[block_idx]) - block_header_length;
        }
        else {
            block_header_length = stoi(block_header_end_bytes_list[block_idx]) - stoi(block_end_bytes_list[block_idx-1]);
            block_length = stoi(block_end_bytes_list[block_idx]) - stoi(block_end_bytes_list[block_idx-1]) - block_header_length;
        }

        // read in compressed block header
        char block_header_bytes[block_header_length];
        file.read(block_header_bytes, block_header_length);
        // decompress block header
        string block_header_bitstring = string(block_header_bytes, block_header_length);
        string block_header = zlib_decompress(block_header_bitstring);
//        cout << "block header: " << block_header << endl;
        // split block header
        vector<string> block_header_list = split_string(block_header, ',');
        // read in compressed block
        char block_bytes[block_length];
        file.read(block_bytes, block_length);
        // convert to string
        string block_bitstring = string(block_bytes, block_length);
        // decompress block by column
        int curr_block_byte = 0;
        int col_bytes;
        for (int col_idx = 0; col_idx < stoi(num_columns); col_idx++){
            string col_codec = col_codecs[col_idx];
            if (col_idx == 0){
                col_bytes = stoi(block_header_list[col_idx]);
            }
            else {
                col_bytes = stoi(block_header_list[col_idx]) - stoi(block_header_list[col_idx-1]);
            }
            string col_bitstring = block_bitstring.substr(curr_block_byte, col_bytes);
            curr_block_byte += col_bytes;
            string col_decompressed = decompress_column(col_bitstring, col_codec);
//            cout << "column " << col_idx << ": " << col_decompressed << endl;
        }
    }

    return 0;
}
