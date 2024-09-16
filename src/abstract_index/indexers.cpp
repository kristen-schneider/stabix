#include "indexers.h"
#include <algorithm>

float PValIndexer::nearest_bin(float value) {
    // TODO: at query time, these bins should be provided from the index file
    for (float bin : this->bins) {
        if (value >= bin) {
            return bin;
        }
    }

    return -HUGE_VALF;
}

Indexer::Indexer(std::string indexPath, BlockLineMap *map) {
    this->indexPath = indexPath;
    this->blockLineMap = map;
}

PValIndexer::PValIndexer(std::string indexPath, BlockLineMap *map,
                         vector<float> bins)
    : Indexer(indexPath, map) {
    // sort bins in descending order
    std::sort(bins.begin(), bins.end(), std::greater<float>());
    this->bins = bins;
}

bool badFloatSemaphore = false;
float PValIndexer::value_to_bin(std::string line) {
    char *end;
    errno = 0;
    float value = std::strtof(line.c_str(), &end);

    if (end == line.c_str()) {
//        throw std::runtime_error("Invalid float format.");
        // skip NA values
        return -HUGE_VALF;
    }

    if (errno == ERANGE && !badFloatSemaphore) {
        std::cerr << "Warning: rounding some values because they cannot fit in "
                     "float, such as: "
                  << line << std::endl;
        badFloatSemaphore = true;
    }

    return this->nearest_bin(value);
}

unordered_set<int> PValIndexer::compare_query(float threshold,
                                              ComparisonType compType) {

    float pivotBin = this->nearest_bin(threshold);

    switch (compType) {
    case ComparisonType::LessThan:
    case ComparisonType::LessThanOrEqual:
        return query_index([pivotBin](float val) { return val <= pivotBin; });
    case ComparisonType::Equal:
        return query_index([pivotBin](float val) { return val == pivotBin; });
    case ComparisonType::GreaterThan:
    case ComparisonType::GreaterThanOrEqual:
        return query_index([pivotBin](float val) { return val >= pivotBin; });
    }

    // unreachable
    throw std::runtime_error("Invalid comparison type.");
}

// int PValIndexer::value_to_bin(std::string line) {
//     std::string digits;
//
//     auto dotPos = line.find('.');
//     if (dotPos != std::string::npos) {
//         digits = line.substr(0, dotPos);
//         digits += line.substr(dotPos + 1);
//     } else {
//         digits = line;
//     }
//
//     int expo = 0;
//     auto expPos = digits.find('e');
//     if (expPos != std::string::npos) {
//         std::string newDigits = digits.substr(0, expPos);
//         expo = std::stof(digits.substr(expPos + 1));
//         digits = newDigits;
//
//         if (expo <= -2) {
//             return 0;
//         } else if (expo >= 2) {
//             throw std::runtime_error("P-value out of range. Magnitude >
//             10^2");
//         }
//     }
//
//     expo += 1; // .7 -> 7
//     std::string finalRepresentation = digits.substr(0, dotPos + expo);
//     return std::stoi(finalRepresentation);
// }

std::string PValIndexer::bin_to_value(float bin) { return std::to_string(bin); }

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
