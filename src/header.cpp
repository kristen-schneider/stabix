#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "compress.h"
#include "compression_types.hpp"
#include "header.h"

using namespace std;

/*
 * Parse header list
 * @param header_list: vector<string> header_list
 * @param header_query: header columm to query
 * @return header_query_list: vector<string> header from query
 */
vector<string> parse_header_list(
        vector<string> header_list,
        string header_query) {
    vector<string> header_query_list;
    map<string, int> switch_case;
    switch_case["num columns"] = 0;
    switch_case["num blocks"] = 1;
    switch_case["column names"] = 2;
    switch_case["codecs"] = 3;
    switch_case["block header end bytes"] = 4;
    switch_case["block end bytes"] = 5;
    switch_case["block sizes"] = 6;

    int num_blocks = stoi(header_list[1]);
    int already_processed = 0;

    // switch statement for query element
    switch (switch_case[header_query]) {
    case 0: // num columns
        header_query_list.push_back(header_list[0]);
        break;
    case 1: // num blocks
        header_query_list.push_back(header_list[1]);
        break;
    case 2: // column names
        for (int i = 2; i < stoi(header_list[0]) + 2; i++) {
            header_query_list.push_back(header_list[i]);
        }
        break;
    case 3: // codecs
        already_processed = 2 + stoi(header_list[0]);
        for (int i = 2 + stoi(header_list[0]);
        i < already_processed + stoi(header_list[0]);
        i++) {
            header_query_list.push_back(header_list[i]);
        }
        break;
    case 4: // block header end bytes
        already_processed = 2 + 2 * stoi(header_list[0]);
        for (int i = already_processed;
             i < already_processed + stoi(header_list[1]);
             i++) {
            header_query_list.push_back(header_list[i]);
        }
        break;
    case 5: // block end bytes
        already_processed = 2 + 2 * stoi(header_list[0]) + stoi(header_list[1]);
        for (int i = already_processed;
             i < already_processed + stoi(header_list[1]);
             i++) {
            header_query_list.push_back(header_list[i]);
        }
        break;
    case 6: // block sizes
        already_processed = 2 + 2 * stoi(header_list[0]) + 2 * stoi(header_list[1]);
        for (int i = already_processed;
             i < already_processed + num_blocks;
             i++) {
            if (header_list[i] != "") {
                header_query_list.push_back(header_list[i]);
            } else {
                break;
            }
        }
        break;
    default:
        cout << "Error: header_query not found." << endl;
        exit(1);
    }

    return header_query_list;
}


/*
 * Remove the common header from
 * a string compressed with zlib
 * @param compressed_string: string compressed with zlib
 * @param zlib_header: string zlib header
 * @return zlib_header_removed: string zlib header removed
 */
string remove_zlib_header(
        string compressed_string,
        string zlib_header) {
    string zlib_header_removed = compressed_string.substr(zlib_header.length());
    return zlib_header_removed;
}

/*
 * Add the common header to
 * a string compressed with zlib
 * @param compressed_string: string compressed with zlib
 * @param zlib_header: string zlib header
 * @return zlib_header_added: string zlib header added
 */
string add_zlib_header(
        string compressed_string,
        string zlib_header) {
    string zlib_header_added = zlib_header + compressed_string;
    return zlib_header_added;
}

/*
 * Return headers for given codecs
 * @param codec: bxz::Compression codec
 * @return magicNumberOf: string magic number of codec
 */
string magicNumberOf(
        bxz::Compression codec) {
    switch (codec) {
    case bxz::z:
        // Header for gzip
        return "\x1F\x8B";
    case bxz::bz2:
        return "\x42\x5a\x68";
    case bxz::lzma:
        return "\xFD\x37\x7A\x58\x5A\x00";
    case bxz::zstd:
        return "\x28\xB5\x2F\xFD";
    }
    throw std::runtime_error("Unknown compression codec.");
}

/*
 * Return size of headers for given codecs
 * @param codec: bxz::Compression codec
 * @return magicNumberCullSize: int size of magic number of codec
 */
int magicNumberCullSize(
        bxz::Compression codec) {
    return magicNumberOf(codec).size();
}
