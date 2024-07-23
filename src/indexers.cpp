#include "indexers.h"
#include "utils.h"
#include <set>
#include <unordered_map>
#include <vector>

// TODO: !!! possible bug creating multiple bins; seen on
// GCSD90179150_GRCH37.tsv

void Indexer::build_index(std::string inPath, std::string outPath) {
    std::ifstream file(inPath);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file");
    }

    std::string lineStr;
    if (!std::getline(file, lineStr)) {
        throw std::runtime_error("Missing header line");
    }

    int blockSize = 3;
    int queryCol = 9;

    std::unordered_map<int, std::vector<int>> index;

    int lineId = 0;
    bool moreBlocks = true;
    while (moreBlocks) {
        int blockId = lineId / blockSize;
        std::set<int> bins;

        for (int i = 0; i < blockSize; i++) {
            if (!std::getline(file, lineStr)) {
                moreBlocks = false;
                break;
            }

            lineId++;
            auto rowVals = split_string(lineStr, '\t');
            std::string queryVal = rowVals[queryCol];
            int bin = value_to_bin(queryVal);
            bins.insert(bin);
        }

        for (int bin : bins) {
            index[bin].push_back(blockId);
        }
    }

    std::ofstream indexFile(outPath);
    std::unordered_map<int, int> binPositions;

    for (auto &entry : index) {
        binPositions[entry.first] = indexFile.tellp();

        for (int i = 0; i < entry.second.size() - 1; i++) {
            indexFile << entry.second[i] << " ";
        }

        indexFile << entry.second[entry.second.size() - 1] << std::endl;
    }

    indexFile << std::endl;

    int pos0 = indexFile.tellp();

    for (auto &entry : binPositions) {
        std::string key = bin_to_value(entry.first);
        indexFile << key << " " << entry.second << std::endl;
    }

    int footerSize = (int)(indexFile.tellp()) - pos0;
    indexFile << footerSize << std::endl;

    indexFile.close();
}

int PValIndexer::value_to_bin(std::string line) {
    std::string digits;

    size_t dot_pos = line.find('.');
    if (dot_pos != std::string::npos) {
        digits = line.substr(0, dot_pos);
    }

    if (dot_pos + 1 < line.size()) {
        digits += line.substr(dot_pos + 1, 1);
    }

    return std::stoi(digits);
}

std::string PValIndexer::bin_to_value(int bin) {
    std::string out = std::to_string((bin / 10) % 1);
    out += "." + std::to_string(bin % 10);
    return out;
}

// TODO: extract genomic indexer from previous index_main
/*
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

    // write chrm_bp_byte to index file
    string index_file = compressed_file + ".idx";
    cout << "Writing index file to: " << index_file << endl;
    ofstream index;
    index.open(index_file);
    // write header
    index << "block_idx,chromosome,base_pair_location,byte_offset" << endl;
    // write chrm_bp_byte
    for (int i = 0; i < chrm_bp_byte.size(); i++) {
        index << i << "," << get<0>(chrm_bp_byte[i]) << ","
              << get<1>(chrm_bp_byte[i]) << "," << get<2>(chrm_bp_byte[i])
              << endl;
    }
    index.close();
    file.close();
    cout << "Done." << endl << endl;

    return 0;
}
*/
