#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

using namespace std;
namespace fs = std::filesystem;

char *int_to_bytes(int value);

vector<string> split_string(string str, char delimiter);

/*
 * Reads and parses options in configuration file
 * returns a map of options
 */
map<string, string> read_config_file(string config_file) {
    map<string, string> config_options;

    // check if file exists
    ifstream config_stream(config_file);
    if (!config_stream.good()) {
        cout << "ERROR: Config file does not exist: " << config_file << endl;
        exit(1);
    }
    // read and parse config file
    string config_option = "";
    string config_value = "";
    string line;
    while (getline(config_stream, line)) {
        // remove white space from lines
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        // skip empty lines
        if (line.empty()) {
            continue;
        }
        // if line contains ":" it is a config option
        else if(line.find(":") != string::npos) {
            // remove ":" from line
            line.erase(remove(line.begin(), line.end(), ':'),
                       line.end());
            config_option = line;
        }
        // if line does not contain ":", and is not blank, it is a config value
        else {
            // remove newline character from line
            line.erase(remove(line.begin(), line.end(), '\r'), line.end());
            config_value = line;
            config_options[config_option] = config_value;
            config_option = "";
        }
    }
    config_stream.close();
    return config_options;
}

void add_default_config_options(map<string, string> &config_options) {
    // add default config options if option is empty
    if (config_options["block_size"].empty()) {
        config_options["block_size"] = "20";
    }
    if (config_options["genomic"].empty()) {
        // exit program with error message
        cout << "ERROR: genomic query not specified in config file." << endl;
        exit(1);
    }
    // TODO: determine default data type codecs
    if (config_options["int"].empty()){
        config_options["int"] = "zlib";
    }
    if (config_options["float"].empty()){
        config_options["float"] = "zlib";
    }
    if (config_options["string"].empty()){
        config_options["string"] = "zlib";
    }
}

/*
 * return a list of codecs according to the data types in the file
 */
vector<string> get_codecs_by_data_type(vector<string> data_types,
                                       map<string, string> codec_types) {
    vector<string> codecs;
    for (int i = 0; i < data_types.size(); i++) {
        string codec = codec_types[data_types[i]];
        codecs.push_back(codec);
    }
    return codecs;
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

vector<string> read_bed_file(string bed_file){
    vector<string> bed_data;
    // check if file exists
    ifstream bed_stream(bed_file);
    if (!bed_stream.good()) {
        cout << "ERROR: Bed file does not exist: " << bed_file << endl;
        exit(1);
    }
    // read bed file, splitting by delimiter
    // store as "chrm:bp_start-bp_end"
    string line;
    while (getline(bed_stream, line)) {
        // skip empty lines
        if (line.empty()) {
            continue;
        }
        // split line by tab
        istringstream iss(line);
        string token;
        vector<string> tokens;
        while(std::getline(iss, token, ' '))
            tokens.push_back(token);
        string chrm = tokens[0];
        string bp_start = tokens[1];
        string bp_end = tokens[2];
        string bed_line = chrm + ":" + bp_start + "-" + bp_end;
        bed_data.push_back(bed_line);
    }

    return bed_data;
}

vector<string> get_data_types(string line, char delimiter) {

    vector<string> column_data_types;
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
            column_data_types.push_back("float");
        } else {
            try {
                stoi(item);
                column_data_types.push_back("int");
            } catch (invalid_argument &e) {
                column_data_types.push_back("string");
            }
        }
    }
    return column_data_types;
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

map<int, vector<uint32_t>> read_cm_map_file(string map_file) {
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

vector<string> gwas_column_names(string gwasPathString) {
    auto gwasPath = fs::path(gwasPathString);
    // TODO: block_size via map file not implemented
    cout << "...Success" << endl;

    cout << "Preparring indices..." << endl;

    string gwasColumnLine;
    ifstream gwasFile(gwasPath);
    if (!gwasFile.is_open() || !getline(gwasFile, gwasColumnLine)) {
        throw runtime_error("Error opening GWAS file");
    }

    auto gwasColumns = split_string(gwasColumnLine, '\t');
    gwasFile.close();
    return gwasColumns;
}

vector<string> index_paths_of(string output_dir, vector<string> gwasColumns) {
    auto gwasPath = fs::path(output_dir);
    auto outDir =
        gwasPath.parent_path() / (gwasPath.stem().string());
    //    fs::create_directories(outDir);
    auto outPaths = vector<string>();

    for (int i = 0; i < gwasColumns.size(); i++) {
        string columnName = gwasColumns[i];
        auto outPath = outDir / (columnName + ".idx");
        outPaths.push_back(outPath.string());
    }

    return outPaths;
}

map<int, map<int, tuple<int, int, int>>>
read_genomic_index_file(string index_file) {

    map<int, map<int, tuple<int, int, int>>> genomic_index_file_map;

    // check if file exists
    ifstream index_stream(index_file);
    if (!index_stream.good()) {
        cout << "ERROR: Index file does not exist: " << index_file << endl;
        exit(1);
    }

    // read index file
    // format of index file
    // header
    // block_idx,chrm_start,bp_start,line_number,byte_start

    string line;
    while (getline(index_stream, line)) {
        // skip header
        if (line.find("block_idx") != string::npos) {
            continue;
        }
        // split line by comma
        vector<string> vec = split_string(line, ',');
        int block_idx = stoi(vec[0]);
        int chrm_start = stoi(vec[1]);
        int bp_start = stoi(vec[2]);
        int line_number = stoi(vec[3]);
        int byte_start = stoi(vec[4]);
        genomic_index_file_map[block_idx][chrm_start] =
            make_tuple(bp_start, line_number, byte_start);
    }
    index_stream.close();
    return genomic_index_file_map;
}

map<int, int> make_lineID_blockID_map(string index_file) {
    map<int, int> lineID_blockID_map;

    // check if file exists
    ifstream index_stream(index_file);
    if (!index_stream.good()) {
        cout << "ERROR: Index file does not exist: " << index_file << endl;
        exit(1);
    }

    // read index file
    // format of index file
    // header
    // block_idx,chrm_start,bp_start,line_number,byte_start

    string line;
    while (getline(index_stream, line)) {
        // skip header
        if (line.find("block_idx") != string::npos) {
            continue;
        }
        // split line by comma
        vector<string> vec = split_string(line, ',');
        int block_idx = stoi(vec[0]);
        int line_number = stoi(vec[3]);
        lineID_blockID_map[line_number] = block_idx;
    }
    index_stream.close();
    return lineID_blockID_map;
}

BlockLineMap::BlockLineMap(map<int, int> map) {
    this->lineID_blockID_map = map;
}

BlockLineMap::BlockLineMap(string index_file)
    : BlockLineMap(make_lineID_blockID_map(index_file)) {}

int BlockLineMap::line_to_block(int line_number) {
    // TODO: line numbers start on 1
    map<int, int> lineID_blockID_map = this->lineID_blockID_map;

    // try and return block ID
    try {
        return lineID_blockID_map.at(line_number);
    } catch (const out_of_range &e) {
        // if line number is not a key; find where the line number would fall
        // between the ordered keys
        auto it = lineID_blockID_map.upper_bound(line_number);
        if (it == lineID_blockID_map.begin()) {
            cout << "ERROR: Line number not found in index file." << endl;
            exit(1);
        }
        it--;
        return it->second;
    }
}
