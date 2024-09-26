#include "decompress.h"
#include "header.h"
#include "index.h"
#include "indexers.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

//    // set bins for pval
//    auto bins = std::vector<float>{5e-1, 5e-5, 5.1e-8};
//    int pval_col_idx = -1;

    // 0. read config options
    // open file, exit
    if (argc != 2) {
        // prevent seg faults
        cout << "1 argument required: config_path" << endl;
        return -1;
    }
    string config_file = argv[1];
    cout << "Reading config options from: " << config_file << endl;
    map<string, string> config_options = read_config_file(config_file);
    add_default_config_options(config_options);

    // - input gwas file
    string gwas_file = config_options["gwas_file"];
    // - queries
    // genomic
    vector<string> index_types = {"genomic"};
    string query_genomic = config_options["genomic"];

    // other
    string extra_index = config_options["extra_index"];
    int second_index_col_idx;
    auto second_index_bins = std::vector<float> {};
    string second_index_threshold = "";

    if (extra_index != "None") {
        vector<string> extra_indices_list = split_string(extra_index, ',');
        index_types.insert(index_types.end(), extra_indices_list.begin(),
                           extra_indices_list.end());

        // get second index col idx from config
        second_index_col_idx = stoi(config_options["col_idx"]);
        // get bins for second index
        string second_index_bins_string = config_options["bins"];
        vector<string> bin_string = split_string(second_index_bins_string, ',');
        for (auto &bin : bin_string) {
            second_index_bins.push_back(stof(bin));
        }
    }
    else {
        // return error
        cout << "No extra indices provided." << endl;
        return -1;
    }

    int block_size;
    try {
        block_size = stoi(config_options["block_size"]);
    } catch (invalid_argument &e) {
        block_size = -1;
    }

    // - codecs (by data type)
    string codec_int = config_options["int"];
    string codec_float = config_options["float"];
    string codec_str = config_options["string"];
    map<string, string> data_type_codecs = {
            {"int", codec_int},
            {"float", codec_float},
            {"string", codec_str}};

    // -out
    string output_dir = config_options["out_directory"];
    auto gwas_path = fs::path(config_options["gwas_file"]);
    auto out_dir_path = fs::path();
    string compressed_file;
    if (block_size == -1) {
        out_dir_path = gwas_path.parent_path() /
                       (gwas_path.stem().string() +
                        "_map" +
                        "_" + config_options["out_name"]);

        compressed_file = out_dir_path / (gwas_path.stem().string() +
                                                 "_map" +
                                                 "_" + config_options["out_name"] +
                                                 ".grlz");
    } else {
        out_dir_path = gwas_path.parent_path() /
                       (gwas_path.stem().string() +
                        "_" + to_string(block_size) +
                        "_" + config_options["out_name"]);

        compressed_file = out_dir_path / (gwas_path.stem().string() +
                                                 "_" + config_options["block_size"] +
                                                 "_" + config_options["out_name"] +
                                                 ".grlz");
    }


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
    auto pValIndexer = PValIndexer(pValIndexPath, blockLineMap, second_index_bins);
    pValIndexer.build_index(gwas_file, second_index_col_idx);
    cout << "Done." << endl;
    // ----------------------------------------------------------------------

    delete blockLineMap;
    return 0;
}

// Simon's old index main
/*
int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw runtime_error("1 argument required: config_path");
    }

    string configPath = argv[1];
    cout << "Reading config options from: " << configPath << endl;
    map<string, string> configOptions = read_config_file(configPath);
    add_default_config_options(configOptions);
    // TODO: block_size via map file not implemented
    int blockSize = stoi(configOptions["block_size"]);
    auto gwasPath = configOptions["gwas_file"];
    cout << "...Success." << endl;
    auto columnNames = column_names(gwasPath);
    auto indexPaths = index_paths_of(gwasPath, columnNames);
    cout << "...Successfully read GWAS file columns headers." << endl;
    cout << "Building indices..." << endl;

    // TODO: need to config indexing algorithms
    vector<float> bins = {0.0, 0.5, 1.0};
    Indexer *indexers[] = {
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, new PValIndexer(indexPaths[9], bins)};

    for (int i = 0; i < columnNames.size(); i++) {
        // TODO: parallelize
        Indexer *indexer = indexers[i];

        if (indexer == NULL) {
            cout << "...Skipping column: " << columnNames[i] << endl;
            continue;
        }

        string columnName = columnNames[i];
        cout << "...Building index for column: " << columnName << endl;

        // big operation
        auto outPath = indexPaths[i];
        indexer->build_index(gwasPath);
        cout << "......Success." << endl;

        delete indexer;
    }

    cout << "Complete." << endl;
    return 0;
}
*/
