#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

#include "decompress.h"
#include "header.h"
#include "index.h"
#include "indexers.h"

using namespace std;
namespace fs = std::filesystem;

unordered_set<int> query_genomic_idx(vector<string> query_list,
                                     map<int, map<int, vector<int>>> genomic_index_info_by_location,
                                     map<int, vector<int>> genomic_index_info_by_block) {

    vector<tuple<int, int>> all_query_block_indexes = get_start_end_block_idx(
        query_list,
        genomic_index_info_by_location,
        genomic_index_info_by_block);
    unordered_set<int> blocks;

    for (int q_idx = 0; q_idx < all_query_block_indexes.size(); q_idx++) {
        // cout << "...decompressing query " << q_idx + 1 << ": "
        //      << query_list[q_idx] << endl;
        //
        // output_file << "Query: " << query_list[q_idx] << endl;

        tuple<int, int> query_start_end_byte = all_query_block_indexes[q_idx];
        int start_block_idx = get<0>(query_start_end_byte);
        int end_block_idx = get<1>(query_start_end_byte);

        // TODO: should this be (doubly) inclusive as implemented?
        for (int block_idx = start_block_idx; block_idx <= end_block_idx; block_idx++) {
            blocks.insert(block_idx);
        }

        if (start_block_idx == -1 || end_block_idx == -1) {
            cout << "Query not found in index" << endl;
            continue;
        }
    }

    return blocks;
}

unordered_set<int> query_abs_idx(string path,
                                 auto config_bins,
                                 string config_query,
                                 BlockLineMap block_line_map) {

    // parse config_bins
    vector<float> bins;

    for (string bin : config_bins) {
        try {
            bins.push_back(stof(bin));
        } catch (const invalid_argument &e) {
            throw invalid_argument("Bin must be a float: " + bin);
        }
    }

    // parse config_query
    int val;
    ComparisonType op;

    regex re("/(>|<)(=?)\\s*(\\d*\\.?.*)/");
    smatch matches;

    if (regex_search(config_query, matches, re)) {
        if (matches[1].str() == "<") {
            op = matches[2].str() == "=" ? ComparisonType::LessThanOrEqual
                                         : ComparisonType::LessThan;
        } else {
            op = matches[2].str() == "=" ? ComparisonType::GreaterThanOrEqual
                                         : ComparisonType::GreaterThan;
        }

        string val_exp = matches[3].str();

        try {
            val = stof(val_exp);
        } catch (const invalid_argument &e) {
            throw invalid_argument("Value must be a float: " + val_exp);
        }
    }

    auto index = PValIndexer(path, block_line_map, bins);
    return index.compare_query(val, op);
}

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

    // open output file
    string gwas_file = config_options["gwas_file"];
    auto gwas_path = fs::path(config_options["gwas_file"]);
    auto out_dir =
        gwas_path.parent_path() / (gwas_path.stem().string() + "_output");
    ofstream query_output_stream;
    string query_output_file_name =
        out_dir / (gwas_path.stem().string() + ".query");
    cout << "Opening output file: " << query_output_file_name << endl;
    query_output_stream.open(query_output_file_name, ios::trunc);
    if (!query_output_stream.is_open()) {
        cout << "Error: could not open output file" << endl;
        return 1;
    }

    // INFO:
    // ----------------------------------------------------------------------
    //      Hardcoded query parameters
    // ----------------------------------------------------------------------
    vector<string> genomic_query_list = {"2:100-150000"};
    // And:  query parameters need to be externally provided to this module
    // INFO:
    // ----------------------------------------------------------------------
    //      Hardcoded query parameters
    // ----------------------------------------------------------------------
    auto config_bins = vector<string>{"0.5", "0.1", "1e-8"};
//    string config_query = "<= 0.5";
    string config_query = "";


    // clear contents of output file and close
    query_output_stream.close();
    query_output_stream.open(query_output_file_name, ios::app);
    cout << "Done." << endl << endl;

    // 1. open compressed file and read header
    cout << "Opening compressed file and reading header..." << endl;
    // TODO: There needs to be a system to vet quality inputs (such as config)
    string compressed_file = out_dir / (gwas_path.stem().string() + ".grlz");
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

    // 3. opening index file
    vector<string> index_names = {"genomic", "pval"};
    auto index_paths = index_paths_of(gwas_path, index_names);
    auto genomic_index_path = index_paths[0];
    auto pval_index_path = index_paths[1];

    // read genomic index
    cout << "Opening genomic index file..." << endl;
    cout << "\t..." << genomic_index_path << endl;
    ifstream genomic_index_file(
        genomic_index_path); // TODO: remove: these next fns
                             // should take path, not stream
    map<int, map<int, vector<int>>> genomic_index_info_by_location =
            read_genomic_index_by_location(
                    genomic_index_path);

    map<int, vector<int>> genomic_index_info_by_block =
            read_genomic_index_by_block(
                    genomic_index_path);

    auto block_line_map = BlockLineMap(genomic_index_path);
    genomic_index_file.close();
    cout << "Done." << endl << endl;

    // 4. get and aggregate blocks associated with each query
    auto total_blocks_to_decompress = vector<int>();
    // get genomic blocks
    auto genom_blocks = query_genomic_idx(
            genomic_query_list,
            genomic_index_info_by_location,
            genomic_index_info_by_block);
    // if there are no genomic blocks, return early. nothing found.
    if (genom_blocks.empty()) {
        cout << "No blocks found for query" << endl;
        return 0;
        // if there is no pvalue query, set the query to all genomic blocks
    }else if(config_query.empty()){
        cout << "No query found for second query..." << endl;
        // set total_blocks_to_decompress to genome blocks
        total_blocks_to_decompress = vector<int>(genom_blocks.begin(), genom_blocks.end());
        // sort blocks
        sort(total_blocks_to_decompress.begin(), total_blocks_to_decompress.end());
        // get blocks from pvalue query
    }else{
        auto pval_blocks = query_abs_idx(genomic_index_path,
                                     config_bins,
                                     config_query,
                                     block_line_map);
        auto total_blocks_to_decompress = vector<int>();

        for (int block : genom_blocks) {
            if (pval_blocks.contains(block)) {
                total_blocks_to_decompress.push_back(block);
            }
        }
        // sort blocks
        sort(total_blocks_to_decompress.begin(), total_blocks_to_decompress.end());
    }

    // 5. decompress all blocks for each query
    std::cout << "Decompressing " << total_blocks_to_decompress.size()
              << " blocks" << std::endl;
    for (int block_idx : total_blocks_to_decompress) {
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
        int block_header_length;
        int block_length;
        if (block_idx == 0) {
            block_header_length = stoi(block_header_end_bytes_list[block_idx]);
            block_length =
                stoi(block_end_bytes_list[block_idx]) - block_header_length;
        } else {
            block_header_length = stoi(block_header_end_bytes_list[block_idx]) -
                                  stoi(block_end_bytes_list[block_idx - 1]);
            block_length = stoi(block_end_bytes_list[block_idx]) -
                           stoi(block_end_bytes_list[block_idx - 1]) -
                           block_header_length;
        }
        int start_byte = get_start_byte(block_idx, genomic_index_info_by_block);
        cout << "......decompressing block " << block_idx
             << ", size: " << block_size << endl;

        query_output_stream << "Block: " << block_idx << endl;

        file.seekg(start_byte, ios::beg);
        char block_header_bytes[block_header_length];
        file.read(block_header_bytes, block_header_length);
        // decompress block header
        string block_header_bitstring =
            string(block_header_bytes, block_header_length);
        string block_header = zlib_decompress(block_header_bitstring);
        vector<string> block_header_list = split_string(block_header, ',');
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
                col_bytes = block_length - stoi(block_header_list[col_idx - 1]);
            } else {
                col_bytes = stoi(block_header_list[col_idx]) -
                            stoi(block_header_list[col_idx - 1]);
            }
            string col_bitstring =
                block_bitstring.substr(curr_block_byte, col_bytes);
            curr_block_byte += col_bytes;
            size_t compressed_size = col_bitstring.size();
            string col_decompressed = decompress_column(
                col_bitstring, col_codec, compressed_size, block_size);
            decompressed_block.push_back(col_decompressed);
        }

        // write decompressed block to output file
        cout << "......writing decompressed block to output file" << endl;
        int column_count = stoi(num_columns);
        vector<string> split_columns[column_count];
        for (int i = 0; i < column_count; i++) {
            split_columns[i] = split_string(decompressed_block[i], ',');
        }
        for (int record_i = 0; record_i <= block_size - 1; record_i++) {
            for (int col_i = 0; col_i <= column_count - 1; col_i++) {
                auto block_list = split_columns[col_i];
                string record = block_list[record_i];
                query_output_stream << record << ',';
            }
            query_output_stream << endl;
        }
    }

    query_output_stream.close();
    return 0;
}
