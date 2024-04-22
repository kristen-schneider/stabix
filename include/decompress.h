#ifndef GWAS_LOCAL_DECOMPRESS_H
#define GWAS_LOCAL_DECOMPRESS_H

#endif //GWAS_LOCAL_DECOMPRESS_H

#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <fstream>
#include <sstream>

using namespace std;

string decompress_column(string compressed_column, string codec, size_t compressedSize);
string zlib_decompress(string in_data);
vector<uint32_t> fastpfor_vb_decompress(uint32_t * in_data, size_t compressedSize);
vector<uint32_t> fastpfor_vb_delta_decompress(const std::vector<uint32_t>& encoded_deltas);