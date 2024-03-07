#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>

#include "utils.h"

using namespace std;

/*
 * Read config file
 * @param config_file
 * @return vector of config options
 */
map<string, string> read_config_file(
        string config_file) {
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
        option_name.erase(remove(option_name.begin(), option_name.end(), ':'), option_name.end());
        getline(config_stream, option_item);
        // remove leading whitespace from option item
        option_item.erase(0, option_item.find_first_not_of(' '));
        config_options[option_name] = option_item;
    }
    config_stream.close();
    return config_options;
}

string convert_vector_to_string(vector<string> vec){
    string str;
    for (int i = 0; i < vec.size(); i++){
        str += vec[i] + ",";
    }
    str.pop_back();
    return str;
}

vector<string> convert_string_to_vector(string str){
    vector<string> vec;
    istringstream line_stream(str);
    string column_value;
    while (getline(line_stream, column_value, ',')) {
        vec.push_back(column_value);
    }
    return vec;
}


int bytes_to_int(char bytes[4]){
    int value = 0;
    value += (unsigned char) bytes[0];
    value += (unsigned char) bytes[1] << 8;
    value += (unsigned char) bytes[2] << 16;
    value += (unsigned char) bytes[3] << 24;
    return value;
}

char * int_to_bytes(int value){
    char * bytes = new char[4];
    bytes[0] = (char) (value & 0xFF);
    bytes[1] = (char) ((value >> 8) & 0xFF);
    bytes[2] = (char) ((value >> 16) & 0xFF);
    bytes[3] = (char) ((value >> 24) & 0xFF);
    return bytes;
}

vector<string> split_string(string str, char delimiter){
    vector<string> vec;
    istringstream line_stream(str);
    string column_value;
    while (getline(line_stream, column_value, delimiter)) {
        vec.push_back(column_value);
    }
    return vec;
}

/*
 * Get index value of a string in a vector
 */
int get_index(vector<string> vec, string str){
    int idx = -1;
    for (int i = 0; i < vec.size(); i++){
        if (vec[i] == str){
            idx = i;
            break;
        }
    }
    return idx;
}