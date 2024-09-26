#ifndef GWAS_COMPRESS_CPP_UTILS_H
#define GWAS_COMPRESS_CPP_UTILS_H

#endif // GWAS_COMPRESS_CPP_UTILS_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

map<string, string> read_config_file(
        string config_file);

void add_default_config_options(
        map<string, string> &config_options);

// Python equivalent:  `return value in array`.
// For std::vector
template <typename T>
bool in_array(const T &value, const std::vector<T> &array) {
    return std::find(array.begin(), array.end(), value) != array.end();
}

// Python equivalent:  `return value in array`.
// For C-style arrays
// template <typename T, size_t N>
template <typename T, size_t N>
bool in_array(const T &value, const T (&array)[N]) {
    return std::find(std::begin(array), std::end(array), value) !=
           std::end(array);
}

int get_index(
        vector<string> vec,
        string str);

vector<string> get_data_types(
        string line,
        char delimiter);

vector<string> get_codecs_by_data_type(vector<string> data_types,
                                       map<string, string> codec_types);

map<string, vector<string>> read_bed_file(string bed_file);

char get_delimiter(
        string line);

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
        uint32_t *compressed_arr,
        size_t compressedSize);

uint32_t *convert_string_to_vector_uint32(
        string in_string,
        char delimiter);

int bytes_to_int(
        char bytes[4]);

char *int_to_bytes(
        int value);

map<int, vector<uint32_t>> read_cm_map_file(
        string map_file);

vector<int> get_block_sizes(
        vector<vector<vector<string>>> all_blocks);

vector<string> split_string(
        string str, char delimiter);

vector<string> column_names(
        string gwasPathString);

vector<string> index_paths_of(
        string gwasPathStr,
        vector<string> gwasColumns);

map<int, map<int, tuple<int, int, int>>> read_genomic_index_file(
        string index_file);

map<int, int> make_lineID_blockID_map(string index_file);

/*
 * get block ID from a line number
 */
int get_block_from_line(
        map<int, int> lineID_blockID_map,
        int line_number);

/*
 * Utility providing a line id -> block id map
 */
class BlockLineMap {
    private:
        map<int, int> lineID_blockID_map;
    public:
        BlockLineMap(string indexPath);
        BlockLineMap(map<int, int> map);
        int line_to_block(int line);
};
