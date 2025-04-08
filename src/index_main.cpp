#include "index_main.h"
#include "decompress.h"
#include "header.h"
#include "indexers.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

int index_main(string config_path) {
    cout << "Reading config options from: " << config_path << endl;
    map<string, string> config_options = read_config_file(config_path);
    add_default_config_options(config_options);
    return index_main_by_map(config_options);
}

int index_main_by_map(map<string, string> config_options) {
    // - queries

    // other
    string extra_index = config_options["extra_index"];
    int second_index_col_idx;
    auto second_index_bins = std::vector<float>{};
    string second_index_threshold = "";

    if (extra_index != "None") {
        vector<string> extra_indices_list = split_string(extra_index, ',');

        // get second index col idx from config
        second_index_col_idx = stoi(config_options["col_idx"]);
        // get bins for second index
        string second_index_bins_string = config_options["bins"];
        vector<string> bin_string = split_string(second_index_bins_string, ',');
        for (auto &bin : bin_string) {
            second_index_bins.push_back(stof(bin));
        }
    } else {
        // return error
        cout << "No extra indices provided." << endl;
        return -1;
    }

    // -out
    auto out_dir_path = fs::path(config_options["index_dir"]);
    auto compressed_file = out_dir_path / "rows.grlz"; // grlz?
    cout << "Done." << endl << endl;

    // 1. open compressed file and read header
    cout << "Opening compressed file and reading header..." << endl;
    // TODO: There needs to be a system to vet quality inputs (such as config)
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

    // parse header
    string num_columns = parse_header_list(header_list, "num columns")[0];
    string num_blocks = parse_header_list(header_list, "num blocks")[0];
    vector<string> block_sizes_list =
        parse_header_list(header_list, "block sizes");

    // get paths for index files
    vector<string> indexNames = {"genomic"};
    if (extra_index != "None") {
        indexNames.push_back(extra_index);
    }
    auto indexPaths = index_paths_of(out_dir_path, indexNames);
    string genomicIndexPath = indexPaths[0];
    BlockLineMap *blockLineMap = new BlockLineMap(genomicIndexPath);

    string pValIndexPath = indexPaths[1];
    cout << "Writing p-value index file to: " << pValIndexPath << endl;
    auto pValIndexer = PValIndexer(pValIndexPath, blockLineMap);
    string gwas_file = config_options["gwas_file"];
    pValIndexer.build_index(gwas_file, second_index_col_idx, second_index_bins);
    cout << "Done." << endl;
    cout << endl << "---Indexing Complete---" << endl;

    delete blockLineMap;
    return 0;
}
