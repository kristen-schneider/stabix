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

vector<string> header_first_half(
        string in_file);

string get_gzip_header();

vector<string> parse_header_list(
        vector<string> header_list,
        string header_query);
