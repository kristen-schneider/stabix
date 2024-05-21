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

void add_default_config_options(
        map<string, string> &config_options);

int get_index(
        vector<string> vec, string str);

char get_delimiter(
        string line);

string get_data_types(
        string line,
        char delimiter);

vector<string> split_string(
        string str, char delimiter);

string convert_vector_str_to_string(
        vector<string> vec);


string convert_vector_int_to_string(
        vector<int> vec);

vector<string> convert_string_to_vector_string(
        string str);

vector<uint32_t> convert_string_to_vector_unsignedlong(
        string str);

vector<uint32_t> convert_vector_string_to_vector_int(
        vector<string> vec);

string convert_vector_uint32_to_string(
        uint32_t * compressed_arr,
        size_t compressedSize);

uint32_t * convert_string_to_vector_uint32(
        string in_string,
        char delimiter);

int bytes_to_int(
        char bytes[4]);

char * int_to_bytes(
        int value);

map<int, vector<uint32_t>> get_chrm_block_bp_ends(
        string map_file);

vector<int> get_block_sizes(
        vector<vector<vector<string>>> all_blocks);
