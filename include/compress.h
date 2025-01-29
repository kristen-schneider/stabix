#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <zlib.h>

using namespace std;

vector<string> compress_block(vector<vector<string>> block,
                              vector<string> codecs_list);

string zlib_compress(string in_data);

vector<uint32_t> fastpfor_vb_compress(vector<uint32_t> in_data,
                                      size_t &compressedSize);

vector<uint32_t> fastpfor_vb_delta_compress(vector<uint32_t> in_data,
                                            size_t &compressedSize);

string deflate_compress(string inputData);

string bz2_compress(string inputData);

string xz_compress(string inputData);

string zstd_compress(string inputData);

// Calls libzippp -> libzip with compression method: uncompressed.
// Used as a baseline for testing the other compression methods.
string raw_compress(string inputData);
