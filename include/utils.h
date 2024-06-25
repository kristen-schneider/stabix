#ifndef GWAS_COMPRESS_CPP_UTILS_H
#define GWAS_COMPRESS_CPP_UTILS_H

#endif // GWAS_COMPRESS_CPP_UTILS_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

/*
 * Read config file
 * @param config_file
 * @return vector of config options
 */
map<string, string> read_config_file(string config_file);

/*
 * Add default config options if not present
 * @param config_options
 * @return void
 */
void add_default_config_options(map<string, string> &config_options);

template <typename T>
bool in_array(const T &value, const std::vector<T> &array);

// Python equivalent:  `return value in array`.
// For std::vector
template <typename T>
bool in_array(const T &value, const std::vector<T> &array);

// Python equivalent:  `return value in array`.
// For C-style arrays
template <typename T, size_t N>
bool in_array(const T &value, const T (&array)[N]);

/*
 * Get index value of a string in a vector
 * @param vec: vector in which to look
 * @param str: string to search for
 * @return index: index value of string in vector
 */
int get_index(vector<string> vec, string str);

/*
 * Get the column types of a file
 * @param line: string
 * @param delimiter: char to split by
 * @return column_types_str: string of comma separated data types
 */
string get_data_types(string line, char delimiter);

/*
 * Get the delimiter of a file
 * @param line: string of first line of file
 * @return delimiter: char to split by
 */
char get_delimiter(string line);

/*
 * Convert vector string to string
 * @param vec: vector of strings
 * @return str: string of comma separated values
 */
string convert_vector_str_to_string(vector<string> vec);

/*
 * Convert vector of ints to string
 * @param vec: vector of integers
 * @return str: string of comma separated values
 */
string convert_vector_int_to_string(vector<int> vec);

/*
 * Convert string to vector
 * @param str: string of comma separated values
 * @return vector of strings
 */
vector<string> convert_string_to_vector_string(string str);

/*
 * Convert string to vector
 * @param str: string of comma separated values
 * @return vector of int
 */
vector<uint32_t> convert_string_to_vector_unsignedlong(string str);

/*
 * Convert a vector of strings to vector of integers
 * @param vec: vector of strings
 * @return vector of integers
 */
vector<uint32_t> convert_vector_string_to_vector_int(vector<string> vec);

/* Convert vector of uint32_t to string
 * @param compressed_arr: uint32_t array
 * @param compressed_size: size of compressed array
 * @return compressed_string: string of comma separated values
 */
string convert_vector_uint32_to_string(uint32_t *compressed_arr,
                                       size_t compressedSize);

/* Convert string to vector of uint32_t
 * @param in_string: string of comma separated values
 * @param delimiter: char to split by
 * @return compressed_arr: uint32_t array
 */
uint32_t *convert_string_to_vector_uint32(string in_string, char delimiter);

/*
 * Convert bytes to int
 * @param line: bytes
 * @return value: int value
 */
int bytes_to_int(char bytes[4]);

/*
 * Convert int to bytes
 * @param value: int
 * @return bytes: char *
 */
char *int_to_bytes(int value);

/*
 * Function to read a map file and find bp end of blocks
 * @param map_file: string
 * @return map of chrm: <bp, bp, bp...>
 */
map<int, vector<uint32_t>> get_chrm_block_bp_ends(string map_file);

/*
 * Function to get the sizes of blocks when using a map file
 * @param all_blocks: vector of blocks
 * @return vector of block sizes
 */
vector<int> get_block_sizes(vector<vector<vector<string>>> all_blocks);

/*
 * Split string by delimiter
 * @param str: string
 * @param delimiter: char to split by
 * @return vector of strings
 */
vector<string> split_string(string str, char delimiter);
