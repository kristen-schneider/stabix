#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "decompress.h"
#include "index.h"
#include "utils.h"

using namespace std;

///*

/*
 * Get chromosome and genomic coordinates from compressed file
 * @param compressed_file: string path to compressed file
 * @param delimiter: char delimiter
 * @param header_length: int length of header
 * @param chrm_col: int column index of chromosome
 * @param bp_col: int column index of genomic coordinates
 * @param block_header_end_bytes: vector<string> list of byte offsets for end of
 * block headers
 * @param block_end_bytes: vector<string> list of byte offsets for end of blocks
 * @return vector<tuple<int, int, int>> chrm_bp_byte chromosome, genomic
 * coordinates, byte offset
 */
vector<tuple<int, int, int>>
get_chrm_bp_byte(string compressed_file, char delimiter, int header_length,
                 int chrm_col, int bp_col,
                 vector<string> block_header_end_bytes,
                 vector<string> block_end_bytes, vector<string> codecs_list,
                 vector<string> block_sizes_list) {
    vector<tuple<int, int, int>> chrm_bp_byte;

    // get codecs for each column
    string chrm_codec = codecs_list[chrm_col];
    string bp_codec = codecs_list[bp_col];

    // open gwas file
    ifstream file(compressed_file);

    // count how many bytes in the whole file
    file.seekg(0, ios::end);
    int file_size = file.tellg();

    // start reading at beginning of file
    file.seekg(0, ios::beg);

    // iterate through all blocks and get chromosome and genomic coordinates
    int buffer_bytes = 4;
    int header_bytes = buffer_bytes + header_length;
    int curr_byte_offset;
    int block_header_size;
    int block_start_byte;
    // iterate through block_header_end_bytes and block_end_bytes
    // to get chromosome and genomic coordinates
    for (int block_idx = 0; block_idx < block_header_end_bytes.size();
         block_idx++) {
        // get block_size
        size_t block_size;
        // all blocks are the same size except the last one
        if (block_idx == block_header_end_bytes.size() - 1) {
            block_size = stoi(block_sizes_list[1]);
        } else {
            block_size = stoi(block_sizes_list[0]);
        }

        // if first block, start after header
        if (block_idx == 0) {
            curr_byte_offset = header_bytes;
            block_header_size = stoi(block_header_end_bytes[block_idx]);
            block_start_byte = header_bytes;
        }
        // else start after previous block
        else {
            curr_byte_offset =
                header_bytes + stoi(block_end_bytes[block_idx - 1]);
            block_header_size = stoi(block_header_end_bytes[block_idx]) -
                                stoi(block_end_bytes[block_idx - 1]);
            block_start_byte =
                header_bytes + stoi(block_end_bytes[block_idx - 1]);
        }
        // jump to start of block header
        file.seekg(curr_byte_offset, ios::beg);
        // read in block header
        char block_header_bytes[block_header_size];
        file.read(block_header_bytes, block_header_size);
        // decompress block header
        string block_header_bitstring =
            string(block_header_bytes, block_header_size);
        string decompressed_block_string =
            zlib_decompress(block_header_bitstring);
        // split block header
        vector<string> curr_block_header_list =
            split_string(decompressed_block_string, ',');
        int chrm_column_byte_start = stoi(block_header_end_bytes[block_idx]) +
                                     stoi(curr_block_header_list[chrm_col - 1]);
        int chrm_column_length = stoi(curr_block_header_list[chrm_col]) -
                                 stoi(curr_block_header_list[chrm_col - 1]);

        int bp_column_byte_start = stoi(block_header_end_bytes[block_idx]) +
                                   stoi(curr_block_header_list[bp_col - 1]);
        int bp_column_length = stoi(curr_block_header_list[bp_col]) -
                               stoi(curr_block_header_list[bp_col - 1]);

        // jump to start of chromosome column

        curr_byte_offset = header_bytes + chrm_column_byte_start;
        file.seekg(curr_byte_offset, ios::beg);
        // read in chromosome column
        char chrm_bytes[chrm_column_length];
        file.read(chrm_bytes, chrm_column_length);
        // convert chromosome bytes to string
        string chrm_string = string(chrm_bytes, chrm_column_length);
        // decompress chromosome string
        string decompressed_chrm_column = decompress_column(
            chrm_string, chrm_codec, chrm_column_length, block_size);
        vector<string> chrm_list =
            split_string(decompressed_chrm_column, delimiter);
        int block_chrm_start = stoi(chrm_list[0]);

        // jump to start of bp column
        curr_byte_offset = header_bytes + bp_column_byte_start;
        file.seekg(curr_byte_offset, ios::beg);
        // read in bp column
        char bp_bytes[bp_column_length];
        file.read(bp_bytes, bp_column_length);
        // convert bp bytes to string
        string bp_string = string(bp_bytes, bp_column_length);
        // decompress bp string
        string decompressed_bp_column = decompress_column(
            bp_string, bp_codec, bp_column_length, block_size);
        vector<string> bp_list =
            split_string(decompressed_bp_column, delimiter);
        int block_bp_start = stoi(bp_list[0]);

        // add chromosome and genomic coordinates to chrm_bp_byte
        chrm_bp_byte.push_back(
            make_tuple(block_chrm_start, block_bp_start, block_start_byte));
        //        cout << " finished block: " << block_idx << endl;
    }

    return chrm_bp_byte;
}

/*
 * Make index block map
 * @param index_file: string
 * @return map<int, int> block_idx, byte_offset
 */
map<int, int> make_index_block_map(string index_file) {
    map<int, int> index_map;
    ifstream file(index_file);
    string line;
    vector<string> header;
    int line_count = 0;
    while (getline(file, line)) {
        if (line_count == 0) {
            header = split_string(line, ',');
        } else {
            vector<string> line_list = split_string(line, ',');
            int block_idx = stoi(line_list[0]);
            string chrm = line_list[1];
            string bp = line_list[2];
            int line_start = stoi(line_list[3]);
            int byte_offset = stoi(line_list[4]);
            index_map[block_idx] = byte_offset;
        }
        line_count++;
    }
    return index_map;
}

/*
 * Read index file into map
 * @param index_file: string
 * @return index_map: map<string, map<string, string>> chrm, bp, byte_offset,
 * block_idx chrm, bp, byte_offset
 */
map<int, map<int, tuple<int, int>>> read_index_file(string index_file) {
    map<int, map<int, tuple<int, int>>> index_map;
    ifstream file(index_file);
    string line;
    vector<string> header;
    int line_count = 0;
    while (getline(file, line)) {
        if (line_count == 0) {
            header = split_string(line, ',');
        } else {
            vector<string> line_list = split_string(line, ',');
            map<int, tuple<int, int>> line_map;
            int block_idx = stoi(line_list[0]);
            int chrm = stoi(line_list[1]);
            int bp = stoi(line_list[2]);
            int byte_offset = stoi(line_list[3]);

            line_map[bp] = make_tuple(byte_offset, block_idx);
            // if chromosome not in index_map, add it
            if (index_map.find(chrm) == index_map.end()) {
                index_map[chrm] = line_map;
            }
            // else add to existing chromosome
            else {
                index_map[chrm][bp] = make_tuple(byte_offset, block_idx);
            }
        }
        line_count++;
    }
    return index_map;
}

/*
 * read genomic index file and create a data structure which can be searched by genomic location:( chrm and bp )
 * return genomic_index_info_by_location: chrm -> bp -> block_idx, line_start, byte_offset
 */
map<int, map<int, vector<int>>> read_genomic_index_by_location(
        string genomic_index_file) {

    // chrm -> bp -> block_idx, line_start, byte_offset
    map<int, map<int, vector<int>>> genomic_index_info_by_location;

    ifstream file(genomic_index_file);
    string line;

    vector<string> header;

    int line_count = 0;

    while (getline(file, line)) {
        if (line_count == 0) {
            header = split_string(line, ',');
        } else {
            vector<string> line_list = split_string(line, ',');

            map<int, vector<int>> bp_line_byte;
            int block_idx = stoi(line_list[0]);
            int chrm = stoi(line_list[1]);
            int bp_start = stoi(line_list[2]);
            int line_start = stoi(line_list[3]);
            int byte_offset = stoi(line_list[4]);

            bp_line_byte[bp_start] = {block_idx, line_start, byte_offset};
            // if chromosome not in index_map, add it
            if (genomic_index_info_by_location.find(chrm) == genomic_index_info_by_location.end()) {
                genomic_index_info_by_location[chrm] = bp_line_byte;

            }
            // else add to existing chromosome
            else {
                genomic_index_info_by_location[chrm][bp_start] = {block_idx, line_start, byte_offset};
            }
        }
        line_count++;
    }
    return genomic_index_info_by_location;
}

/*
 * read genomic index file and create a data structure which can be searched by block idx
 * return genomic_index_info_by_block: block_idx -> line_start, byte_offset
 */map<int, vector<int>> read_genomic_index_by_block(
        string genomic_index_file)
 {

    // block_idx -> line_start, byte_offset
    map<int, vector<int>> genomic_index_info_by_block;

    ifstream file(genomic_index_file);
    string line;

    vector<string> header;

    int line_count = 0;

    while (getline(file, line)) {
        if (line_count == 0) {
            header = split_string(line, ',');
        } else {
            vector<string> line_list = split_string(line, ',');

            map<int, vector<int>> bp_line_byte;
            int block_idx = stoi(line_list[0]);
            int chrm = stoi(line_list[1]);
            int bp_start = stoi(line_list[2]);
            int line_start = stoi(line_list[3]);
            int byte_offset = stoi(line_list[4]);

            genomic_index_info_by_block[block_idx] = {line_start, byte_offset};
        }
        line_count++;
    }
    return genomic_index_info_by_block;
}

/*
 * Get block index for query
 * @param q_chrm: int of query chromosome
 * @param q_bp: int of query genomic position
 * @param index_file_map: map<int, map<int, vector<int>>> genomic_index_info
 * @return int start_block_idx
 */
int get_block_idx(int q_chrm,
                  int q_bp,
                  map<int, map<int, vector<int>>> genomic_index_info_by_location) {
    int start_block_idx = -1;

    auto chrm = genomic_index_info_by_location.find(q_chrm);
    if (chrm != genomic_index_info_by_location.end()) {
        for (auto const &bp : chrm->second) {
            // this is asking for "give me the biggest bp.first"
            if (q_bp >= bp.first) {
                start_block_idx = bp.second[0];
            }
        }
    }

    return start_block_idx;
}

/*
 * Get start byte for block
 * @param block_idx: int
 * @param index_block_map: map<int, int> index block map
 * @return int start_byte
 */
int get_start_byte(int block_idx, map<int, vector<int>> index_block_map) {
    return index_block_map[block_idx][1];
}

/*
 * Get start and end block index for query
 * @param query_list: vector<string> of queries
 * @param index_file_map: map<int, map<int, tuple<int, int>>> index file map
 * @param index_block_map: map<int, int> index block map
 * @return vector<tuple<int, int>> all_query_info
 */
vector<tuple<int, int>> get_start_end_block_idx(
        vector<string> query_list,
        map<int, map<int, vector<int>>> genomic_index_info_by_location,
        map<int, vector<int>> genomic_index_info_by_block) {

    vector<tuple<int, int>> all_query_info;
    for (int q_idx = 0; q_idx < query_list.size(); q_idx++) {
        // get information about the query
        int q_chrm = stoi(split_string(query_list[q_idx], ':')[0]);
        int q_bp_start = stoi(split_string(query_list[q_idx], ':')[1]);
        int q_bp_end =
            stoi(split_string(split_string(query_list[q_idx], ':')[1], '-')[1]);

        // get block idx for query
        int start_block_idx =
            get_block_idx(q_chrm, q_bp_start, genomic_index_info_by_location);
        int end_block_idx = get_block_idx(q_chrm, q_bp_end, genomic_index_info_by_location);
        all_query_info.push_back(make_tuple(start_block_idx, end_block_idx));
    }
    return all_query_info;
}
