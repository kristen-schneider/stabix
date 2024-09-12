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

    // 0. read config options
    if (argc != 2) {
        // prevent seg faults
        cout << "1 argument required: config_path" << endl;
        return -1;
    }
    string config_file = argv[1];
    cout << "Reading config options from: " << config_file << endl;
    map<string, string> config_options = read_config_file(config_file);
    add_default_config_options(config_options);

    string gwas_file = config_options["gwas_file"];
    // block size
    int block_size = -1;
    // if block_size cannot be converted to an int, it is a map file
    try {
        block_size = stoi(config_options["block_size"]);
    } catch (invalid_argument &e) {
        block_size = -1;
    }
    // queries
    vector<string> index_types = {"genomic"};
    string query_genomic = config_options["genomic"];
    vector<string> genomic_queries = read_bed_file(query_genomic);
    // TODO: get query types for other optional queries
    string extra_indices = config_options["extra_indices"];

    // codecs by data type
    string codec_int = config_options["int"];
    string codec_float = config_options["float"];
    string codec_str = config_options["string"];
    map<string, string> data_type_codecs = {
            {"int", codec_int},
            {"float", codec_float},
            {"string", codec_str}};

    auto gwas_path = fs::path(config_options["gwas_file"]);
    auto out_dir = gwas_path.parent_path() / (gwas_path.stem().string() + "_output");
    fs::create_directories(out_dir);
    string compressed_file = out_dir / (gwas_path.stem().string() + ".grlz");

    cout << "\t...gwas_file: " << gwas_file << endl;
    cout << "\t...block_size: " << block_size << endl;
    cout << "\t...indexes: " << convert_vector_str_to_string(index_types) << endl;
    cout << "\t...compressed_file: " << compressed_file << endl;

    cout << "Done." << endl << endl;

    // ----------------------------------------------------------------------
    //      Parse Header used for old version of genomic indexing,
    //      but now genomic index is created during compression
    // ----------------------------------------------------------------------

//    // 1. READ COMPRESSED FILE
//    // open compressed file and reading header
//    cout << "Opening compressed file and reading header..." << endl;
//    ifstream file(compressed_file);
//    // start at beginning and read 4 bytes
//    file.seekg(0, ios::beg);
//    char header_length_bytes[4];
//    file.read(header_length_bytes, 4);
//    // convert 4 bytes to int
//    int header_length = bytes_to_int(header_length_bytes);
//    //    cout << "header length: " << header_length << endl;
//
//    // read header_length bytes starting at byte 4
//    file.seekg(4, ios::beg);
//    char header_bytes[header_length];
//    file.read(header_bytes, header_length);
//    // convert header bytes to string
//    string header_string = string(header_bytes, header_length);
//    // decompress header
//    string header = zlib_decompress(header_string);
//    //    cout << "header: " << header << endl;
//    vector<string> header_list = split_string(header, ',');
//    string num_columns = parse_header_list(header_list, "num columns")[0];
//    string num_blocks = parse_header_list(header_list, "num blocks")[0];
//    vector<string> column_names_list =
//        parse_header_list(header_list, "column names");
//    vector<string> codecs_list = parse_header_list(header_list, "codecs");
//    vector<string> block_header_lengths_list =
//        parse_header_list(header_list, "block header end bytes");
//    vector<string> block_lengths_list =
//        parse_header_list(header_list, "block end bytes");
//    vector<string> block_sizes_list =
//        parse_header_list(header_list, "block sizes");

    cout << "Done." << endl << endl;

    // initialize line -> blockID map for specialized indexes
    // get paths for index files
    vector<string> indexNames = {"genomic", "pval"};
    auto indexPaths = index_paths_of(gwas_file, indexNames);
    string genomicIndexPath = indexPaths[0];
    auto blockLineMap = BlockLineMap(genomicIndexPath);

    // INFO:
    // ----------------------------------------------------------------------
    //      Hardcoded query parameters
    // ----------------------------------------------------------------------
    // TODO: creation of specialized index not working
    // TODO: include these options in the config file when appropriate
    string pValIndexPath = indexPaths[1];
    cout << "Writing p-value index file to: " << pValIndexPath << endl;
    auto bins = std::vector<float>{0.5, 0.1, 1e-8};
    auto pValIndexer = PValIndexer(pValIndexPath, blockLineMap, bins);
    pValIndexer.build_index(gwas_file, block_size, 9);
    cout << "Done." << endl;
    // ----------------------------------------------------------------------

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
