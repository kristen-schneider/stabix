#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

using namespace std;

char *int_to_bytes(int value);

vector<string> split_string(string str, char delimiter);

map<string, string> read_config_file(string config_file) {
    map<string, string> config_options;

    // check if file exists
    ifstream config_stream(config_file);
    if (!config_stream.good()) {
        cout << "ERROR: Config file does not exist: " << config_file << endl;
        exit(1);
    }
    // read and parse config file
    string option_name;
    string option_item;
    while (getline(config_stream, option_name)) {
        // remove ":" from option name
        option_name.erase(remove(option_name.begin(), option_name.end(), ':'),
                          option_name.end());
        getline(config_stream, option_item);
        // remove leading whitespace from option item
        option_item.erase(0, option_item.find_first_not_of(' '));
        config_options[option_name] = option_item;
    }
    config_stream.close();
    return config_options;
}

void add_default_config_options(map<string, string> &config_options) {
    // add default config options if option is empty
    if (config_options["block_size"].empty()) {
        config_options["block_size"] = "20";
    }
    if (config_options["query_type"].empty()) {
        // exit program with error message
        cout << "ERROR: query_type not specified in config file." << endl;
        exit(1);
    }
}

int get_index(vector<string> vec, string str) {
    int idx = -1;
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == str) {
            idx = i;
            break;
        }
    }
    return idx;
}

string get_data_types(string line, char delimiter) {

    string column_types_str;
    // split line by delimiter find data type and store in comma separated
    // string
    stringstream ss(line);
    string item;
    while (getline(ss, item, delimiter)) {
        // check if item is int
        // check if item is float
        // else  item stays as string
        // if none of the above, throw error
        // search for '.' in item
        if (item.find('.') != string::npos) {
            column_types_str += "float,";
        } else {
            try {
                stoi(item);
                column_types_str += "int,";
            } catch (invalid_argument &e) {
                column_types_str += "string,";
            }
        }
    }
    // remove last comma
    column_types_str.pop_back();
    return column_types_str;
}

char get_delimiter(string line) {
    char delimiter;
    // determine delimiter
    if (line.find('\t') != string::npos) {
        delimiter = '\t';
    } else if (line.find(',') != string::npos) {
        delimiter = ',';
    } else if (line.find(' ') != string::npos) {
        delimiter = ' ';
    } else {
        cout << "Error: delimiter not found.\n"
                " Please use files with delimiters as tab, comma, or space."
             << endl;
        exit(1);
    }
    return delimiter;
}

string convert_vector_str_to_string(vector<string> vec) {
    string str = "";
    for (int i = 0; i < vec.size(); i++) {
        str += vec[i] + ",";
    }
    str.pop_back();
    return str;
}

string convert_vector_int_to_string(vector<int> vec) {
    string str;
    for (int i = 0; i < vec.size(); i++) {
        str += to_string(vec[i]) + ",";
    }
    str.pop_back();
    return str;
}

string convert_vector_uint32_to_string(uint32_t *compressed_arr,
                                       size_t compressed_size) {
    string compressed_string;
    for (int i = 0; i < compressed_size; i++) {
        compressed_string += to_string(compressed_arr[i]) + ",";
    }
    compressed_string.pop_back();
    return compressed_string;
}

vector<string> convert_string_to_vector_string(string str) {
    vector<string> vec;
    istringstream line_stream(str);
    string column_value;
    while (getline(line_stream, column_value, ',')) {
        // remove newline character from column_value
        column_value.erase(
            remove(column_value.begin(), column_value.end(), '\r'),
            column_value.end());
        vec.push_back(column_value);
    }
    return vec;
}

uint32_t *convert_string_to_vector_uint32(string in_string, char delimiter) {
    // split string by comma
    vector<string> vec = split_string(in_string, delimiter);
    // convert string to vector of uint32_t
    uint32_t *compressed_arr = new uint32_t[vec.size()];
    for (int i = 0; i < vec.size(); i++) {
        compressed_arr[i] = stoul(vec[i]);
    }
    return compressed_arr;
}

vector<uint32_t> convert_string_to_vector_unsignedlong(string str) {
    vector<uint32_t> out_vec;
    istringstream line_stream(str);
    string column_value;
    while (getline(line_stream, column_value, ',')) {
        // remove newline character from column_value
        column_value.erase(
            remove(column_value.begin(), column_value.end(), '\r'),
            column_value.end());
        // convert data to uint32_t
        out_vec.push_back(stoul(column_value));
    }
    return out_vec;
}

int bytes_to_int(char bytes[4]) {
    int value = 0;
    value += (unsigned char)bytes[0];
    value += (unsigned char)bytes[1] << 8;
    value += (unsigned char)bytes[2] << 16;
    value += (unsigned char)bytes[3] << 24;
    return value;
}

char *int_to_bytes(int value) {
    char *bytes = new char[4];
    bytes[0] = (char)(value & 0xFF);
    bytes[1] = (char)((value >> 8) & 0xFF);
    bytes[2] = (char)((value >> 16) & 0xFF);
    bytes[3] = (char)((value >> 24) & 0xFF);
    return bytes;
}

vector<string> split_string(string str, char delimiter) {
    vector<string> vec;
    istringstream line_stream(str);
    string column_value;
    while (getline(line_stream, column_value, delimiter)) {
        // remove newline character from column_value
        column_value.erase(
            remove(column_value.begin(), column_value.end(), '\r'),
            column_value.end());
        vec.push_back(column_value);
    }
    return vec;
}

vector<uint32_t> convert_vector_string_to_vector_int(vector<string> vec) {
    vector<uint32_t> vec_int;
    for (int i = 0; i < vec.size(); i++) {
        vec_int.push_back(stoi(vec[i]));
    }
    return vec_int;
}

map<int, vector<uint32_t>> get_chrm_block_bp_ends(string map_file) {
    map<int, vector<uint32_t>> chrm_block_bp_ends;
    int BLOCK_CM_SIZE = 1;
    // open map file, exit if file does not exist
    ifstream map_stream(map_file);
    if (!map_stream.good()) {
        cout << "ERROR: Map file does not exist: " << map_file << endl;
        exit(1);
    }
    // read map file
    int block_count = 0;
    int max_cm = block_count * BLOCK_CM_SIZE + 1;
    float prev_bp = 0;
    int prev_chrm = 0;
    string line;
    while (getline(map_stream, line)) {
        // split line by white space
        vector<string> vec = split_string(line, ' ');
        int chrm = stoi(vec[0]);
        // new chromosome, reset block count
        if (chrm != prev_chrm) {
            chrm_block_bp_ends[chrm] = {};
            block_count = 0;
            max_cm = block_count * BLOCK_CM_SIZE + 1;
            prev_chrm = chrm;
        }
        float cm = stof(vec[2]);
        uint32_t bp = stoul(vec[3]);
        // if cm is greater than max_cm, add new block
        if (cm >= max_cm) {
            if (cm == max_cm) {
                prev_bp = bp;
            }
            chrm_block_bp_ends[chrm].push_back(prev_bp);
            block_count++;
            max_cm = block_count * BLOCK_CM_SIZE + 1;
        }
        prev_bp = bp;
    }
    map_stream.close();
    return chrm_block_bp_ends;
}

vector<int> get_block_sizes(vector<vector<vector<string>>> all_blocks) {
    vector<int> block_sizes;
    for (int i = 0; i < all_blocks.size(); i++) {
        block_sizes.push_back(all_blocks[i][0].size());
    }
    return block_sizes;
}
