#pragma once

#include "compression_types.hpp"
#include <string>
#include <vector>

using namespace std;

vector<string> parse_header_list(
        vector<string> header_list,
        string header_query);

string remove_zlib_header(
        string compressed_string,
        string zlib_header);

string add_zlib_header(
        string compressed_string,
        string zlib_header);

string magicNumberOf(
        bxz::Compression codec);

int magicNumberCullSize(
        bxz::Compression codec);