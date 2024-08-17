#include "decompress.h"
#include "header.h"
#include "index.h"
#include "indexers.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {

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
    vector<string> column_names_list =
        parse_header_list(header_list, "column names");
    vector<string> block_header_lengths_list =
        parse_header_list(header_list, "block header end bytes");
    vector<string> block_lengths_list =
        parse_header_list(header_list, "block end bytes");
    vector<string> block_sizes_list =
        parse_header_list(header_list, "block sizes");

    cout << "Done." << endl << endl;

    // for each block, determine the start chromosome and genomic position
    int chrm_idx = get_index(column_names_list, "chromosome");
    int bp_idx = get_index(column_names_list, "base_pair_location");

    vector<tuple<int, int, int>> chrm_bp_byte =
        get_chrm_bp_byte(compressed_file, ',', header_length, chrm_idx, bp_idx,
                         block_header_lengths_list, block_lengths_list,
                         codecs_list, block_sizes_list);

    // get paths for index files
    vector<string> indexNames = {"master", "pval"};
    auto indexPaths = index_paths_of(gwas_file, indexNames);

    // write chrm_bp_byte to (master) index file
    string masterIndexPath = indexPaths[0];
    cout << "Writing master index file to: " << masterIndexPath << endl;
    ofstream masterIndexFile;
    masterIndexFile.open(masterIndexPath);
    // write header
    masterIndexFile << "block_idx,chromosome,base_pair_location,byte_offset"
                    << endl;
    // write chrm_bp_byte
    for (int i = 0; i < chrm_bp_byte.size(); i++) {
        masterIndexFile << i << "," << get<0>(chrm_bp_byte[i]) << ","
                        << get<1>(chrm_bp_byte[i]) << ","
                        << get<2>(chrm_bp_byte[i]) << endl;
    }
    masterIndexFile.close();
    file.close();
    cout << "Done." << endl << endl;

    // write p value index file

    string pValIndexPath = indexPaths[1];
    cout << "Writing p-value index file to: " << pValIndexPath << endl;
    vector<float> bins = {0.0, 0.5, 1.0};
    auto pValIndexer = PValIndexer(pValIndexPath, bins);
    int blockSize =
        2000; // TODO: blockSize needs to be block_sizes, from config
    pValIndexer.build_index(gwas_file, blockSize, 9);
    cout << "Done." << endl;

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
