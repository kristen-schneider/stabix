#ifndef GWAS_COMPRESS_CPP_COMPRESS_H
#define GWAS_COMPRESS_CPP_COMPRESS_H

#endif //GWAS_COMPRESS_CPP_COMPRESS_H

#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <fstream>
#include <sstream>

using namespace std;

string zlib_compress(string in_data);
vector<string> compress_block(vector<vector<string>> block, vector<string> codecs_list);
