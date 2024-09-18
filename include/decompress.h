#ifndef GWAS_LOCAL_DECOMPRESS_H
#define GWAS_LOCAL_DECOMPRESS_H

#endif // GWAS_LOCAL_DECOMPRESS_H

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <zlib.h>


using namespace std;
namespace fs = std::filesystem;

string decompress_column(string compressed_column,
                         string codec,
                         size_t compressedSize,
                         size_t block_size);

string zlib_decompress(string in_data);

vector<uint32_t> fastpfor_vb_decompress(vector<uint32_t> in_data,
                                        size_t compressedSize,
                                        size_t block_size);

vector<uint32_t>
fastpfor_vb_delta_decompress(const std::vector<uint32_t> &encoded_deltas);

string deflate_decompress(string inputData);

string bz2_decompress(string inputData);

string xz_decompress(string inputData);

string zstd_decompress(string inputData);