#ifndef GWAS_COMPRESS_CPP_UTILS_H
#define GWAS_COMPRESS_CPP_UTILS_H

#endif //GWAS_COMPRESS_CPP_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

map<string, string> read_config_file(
        string config_file);

string convert_vector_to_string(
        vector<string> vec);

vector<string> convert_string_to_vector(
        string str);

int bytes_to_int(char bytes[4]);

char * int_to_bytes(int value);

vector<string> split_string(string str, char delimiter);

int get_index(vector<string> vec, string str);