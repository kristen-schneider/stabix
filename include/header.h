#ifndef GWAS_COMPRESS_CPP_HEADER_H
#define GWAS_COMPRESS_CPP_HEADER_H

#endif //GWAS_COMPRESS_CPP_HEADER_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

vector<string> parse_header_list(
        vector<string> header_list,
        string header_query);

string remove_zlib_header(
        string compressed_string);
