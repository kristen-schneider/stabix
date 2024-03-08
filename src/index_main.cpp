#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <map>

#include "decompress.h"
#include "header.h"
#include "index.h"
#include "utils.h"


using namespace std;

int main(int argc, char* argv[]) {

    // 0. read config options
    string config_file = argv[1];
    cout << "Reading config options from: " << config_file << endl;
    map<string, string> config_options;
    config_options = read_config_file(config_file);
    string gwas_file = config_options["gwas_file"];
    string compressed_file = gwas_file + ".grlz";
    vector<string> codecs_list = split_string(config_options["codecs"], ',');
    cout << "Done." << endl << endl;

    // 1. open compressed file
    cout << "Opening compressed file and reading header..." << endl;
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
//    cout << "header: " << header << endl;
    vector<string> header_list = split_string(header, ',');

    // parse header
    string num_columns = parse_header_list(header_list, "num columns")[0];
    string num_blocks = parse_header_list(header_list, "num blocks")[0];
    vector<string> column_names_list = parse_header_list(header_list, "column names");
    vector<string> block_header_lengths_list = parse_header_list(header_list, "block header lengths");
    vector<string> block_lengths_list = parse_header_list(header_list, "block lengths");
    vector<string> block_sizes_list = parse_header_list(header_list, "block sizes");

    cout << "Done." << endl << endl;

    // for each block, determine the start chromosome and genomic position
    int chrm_idx = get_index(column_names_list, "chromosome");
    int bp_idx = get_index(column_names_list, "base_pair_location");

    vector<tuple<int, int, int>> chrm_bp_byte = get_chrm_bp_byte(
            compressed_file,
            ',',
            header_length,
            chrm_idx,
            bp_idx,
            block_header_lengths_list,
            block_lengths_list);

    // write chrm_bp_byte to index file
    string index_file = compressed_file + ".idx";
    cout << "Writing index file to: " << index_file << endl;
    ofstream index;
    index.open(index_file);
    // write header
    index << "block_idx,chromosome,base_pair_location,byte_offset" << endl;
    // write chrm_bp_byte
    for (int i = 0; i < chrm_bp_byte.size(); i++) {
        index << i << "," << get<0>(chrm_bp_byte[i]) << "," << get<1>(chrm_bp_byte[i]) << "," << get<2>(chrm_bp_byte[i]) << endl;
    }
    index.close();
    file.close();
    cout << "Done." << endl << endl;

    return 0;
}