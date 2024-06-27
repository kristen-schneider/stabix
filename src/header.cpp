#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "compress.h"
#include "compression_types.hpp"
#include "header.h"

using namespace std;

vector<string> parse_header_list(vector<string> header_list,
                                 string header_query) {
    vector<string> header_query_list;
    map<string, int> switch_case;
    switch_case["num columns"] = 0;
    switch_case["num blocks"] = 1;
    switch_case["column names"] = 2;
    switch_case["block header end bytes"] = 3;
    switch_case["block end bytes"] = 4;
    switch_case["block sizes"] = 5;

    int num_blocks = stoi(header_list[1]);

    // switch statement for query element
    switch (switch_case[header_query]) {
    case 0:
        header_query_list.push_back(header_list[0]);
        break;
    case 1:
        header_query_list.push_back(header_list[1]);
        break;
    case 2:
        for (int i = 2; i < stoi(header_list[0]) + 2; i++) {
            header_query_list.push_back(header_list[i]);
        }
        break;
    case 3:
        for (int i = stoi(header_list[0]) + 2;
             i < stoi(header_list[0]) + 2 + stoi(header_list[1]); i++) {
            header_query_list.push_back(header_list[i]);
        }
        break;
    case 4:
        for (int i = stoi(header_list[0]) + 2 + stoi(header_list[1]);
             i < stoi(header_list[0]) + 2 + stoi(header_list[1]) * 2; i++) {
            header_query_list.push_back(header_list[i]);
        }
        break;
    case 5:
        //            for (int i =
        //            stoi(header_list[0])+2+stoi(header_list[1])*2; i <
        //            stoi(header_list[0])+num_blocks+stoi(header_list[1])*2+num_blocks;
        //            i++){
        for (int i = stoi(header_list[0]) + 2 + stoi(header_list[1]) * 2;
             i <
             stoi(header_list[0]) + 2 + stoi(header_list[1]) * 2 + num_blocks;
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

string remove_zlib_header(string compressed_string, string zlib_header) {
    string zlib_header_removed = compressed_string.substr(zlib_header.length());
    return zlib_header_removed;
}

string add_zlib_header(string compressed_string, string zlib_header) {
    string zlib_header_added = zlib_header + compressed_string;
    return zlib_header_added;
}

string magicNumberOf(bxz::Compression codec) {
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

int magicNumberCullSize(bxz::Compression codec) {
    return magicNumberOf(codec).size();
}
