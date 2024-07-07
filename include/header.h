#ifndef GWAS_COMPRESS_CPP_HEADER_H
#define GWAS_COMPRESS_CPP_HEADER_H
#endif // GWAS_COMPRESS_CPP_HEADER_H

#include "compression_types.hpp"
#include <string>
#include <vector>

using namespace std;

/*
 * Parse header list
 * @param header_list: vector<string>
 * @param header_query: string
 * @return header_query_list: vector<string>
 */
vector<string> parse_header_list(vector<string> header_list,
                                 string header_query);

/*
 * Remove the common header from
 * a string compressed with zlib
 */
string remove_zlib_header(string compressed_string, string zlib_header);

string add_zlib_header(string compressed_string, string zlib_header);

string magicNumberOf(bxz::Compression codec);

int magicNumberCullSize(bxz::Compression codec);
