#include <algorithm>
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

/*
 * read genomic index file and create a data structure which can be searched by genomic location:( chrm and bp )
 * return genomic_index_info_by_location: chrm -> bp -> block_idx, line_start, byte_offset
 * @param genomic_index_file: string of genomic index file
 * @return map<int, map<int, vector<int>>> genomic_index_info_by_location
 */
map<int, map<int, vector<unsigned int>>> read_genomic_index_by_location(
        string genomic_index_file) {

    // chrm -> bp -> block_idx, line_start, byte_offset
    map<int, map<int, vector<unsigned int>>> genomic_index_info_by_location;

    ifstream file(genomic_index_file);
    string line;

    vector<string> header;

    int line_count = 0;

    while (getline(file, line)) {
        if (line_count == 0) {
            header = split_string(line, ',');
        } else {
            vector<string> line_list = split_string(line, ',');

            map<int, vector<unsigned int>> bp_line_byte;
            unsigned int block_idx = stoul(line_list[0]);
            int chrm = stoi(line_list[1]);
            int bp_start = stoi(line_list[2]);
            unsigned int line_start = stoul(line_list[3]);
            unsigned int byte_offset = stoul(line_list[4]);

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
 * @param genomic_index_file: string of genomic index file
 * @return map<int, vector<int>> genomic_index_info_by_block
 */
map<int, vector<unsigned int>> read_genomic_index_by_block(
        string genomic_index_file)
 {

    // block_idx -> line_start, byte_offset
    map<int, vector<unsigned int>> genomic_index_info_by_block;

    ifstream file(genomic_index_file);
    string line;

    vector<string> header;

    int line_count = 0;

    while (getline(file, line)) {
        if (line_count == 0) {
            header = split_string(line, ',');
        } else {
            vector<string> line_list = split_string(line, ',');

            map<int, vector<unsigned int>> bp_line_byte;
            int block_idx = stoi(line_list[0]);
            int chrm = stoi(line_list[1]);
            int bp_start = stoi(line_list[2]);
            unsigned int line_start = stoul(line_list[3]);
            unsigned int byte_offset = stoul(line_list[4]);

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
 * chrm -> bp -> block_idx, line_start, byte_offset
 * @return int start_block_idx
 */
int get_block_idx(int q_chrm,
                  int q_bp,
                  map<int, map<int, vector<unsigned int>>> genomic_index_info_by_location) {

//    cout << "Chromosome: " << q_chrm << " BP: " << q_bp << endl;

    // time this function
//    auto start = chrono::high_resolution_clock::now();

    // check if chrm is in index
    if (genomic_index_info_by_location.find(q_chrm) == genomic_index_info_by_location.end()) {
        return -1;
    }
    // make start block -1
    int start_block_idx;
    start_block_idx = -1;

    auto chrm = genomic_index_info_by_location.find(q_chrm);

    // Use std::lower_bound to find the first element greater than q_bp
    // This will give us the first bp in the chromosome that is greater than q_bp
    if (chrm != genomic_index_info_by_location.end()) {
        auto bp = chrm->second.lower_bound(q_bp);
        // print the bp
        // If the bp is the first bp in the chromosome, then return the last block in the previous chromosome
        if (bp == chrm->second.begin()) {
            // if chromsome is not 1, then return the last block in the previous chromosome
            if (q_chrm != 1) {
                auto prev_chrm = genomic_index_info_by_location.find(q_chrm - 1);
                start_block_idx = prev_chrm->second.rbegin()->second[0];
//                auto end = chrono::high_resolution_clock::now();
//                auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
//                cout << "Time end early: "
//                     << duration.count() << " nanoseconds" << endl;
                return start_block_idx;
            }else{
                // if chromosome is 1, then return the first block in the chromosome
                start_block_idx = chrm->second.begin()->second[0];
//                auto end = chrono::high_resolution_clock::now();
//                auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
//                cout << "Time end early: "
//                     << duration.count() << " nanoseconds" << endl;
                return start_block_idx;
            }
        }
        // Otherwise, we need to decrement the iterator to get the previous bp
        bp--;
        start_block_idx = bp->second[0];
    }
//    auto end = chrono::high_resolution_clock::now();
//    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
//    cout << "Time taken by function: "
//         << duration.count() << " nanoseconds" << endl;
    return start_block_idx;
}

/*
 * Get start byte for block
 * @param block_idx: int
 * @param index_block_map: map<int, int> index block map
 * @return int start_byte
 */
unsigned int get_start_byte(
        int block_idx,
        map<int, vector<unsigned int>> index_block_map) {
    return index_block_map[block_idx][1];
}

/*
 * get start end block index for a single query
 */
tuple<int, int> get_start_end_block_idx_single(int gene_chrm,
                                               int gene_bp_start,
                                               int gene_bp_end,
                                               map<int, map<int, vector<unsigned int>>> genomic_index_info_by_location) {
    // get block idx for query
    int start_block_idx = get_block_idx(gene_chrm, gene_bp_start, genomic_index_info_by_location);
    int end_block_idx = get_block_idx(gene_chrm, gene_bp_end, genomic_index_info_by_location);
    return make_tuple(start_block_idx, end_block_idx);
}

/*
 * Get start and end block index for a list of queries
 * @param query_list: vector<string> of queries
 * @param index_file_map: map<int, map<int, tuple<int, int>>> index file map
 * @param index_block_map: map<int, int> index block map
 * @return vector<tuple<int, int>> all_query_info
 */
vector<tuple<int, int>> get_start_end_block_idx(
        vector<string> query_list,
        map<int, map<int, vector<unsigned int>>> genomic_index_info_by_location) {

    vector<tuple<int, int>> all_query_info;
    for (int q_idx = 0; q_idx < query_list.size(); q_idx++) {
        // get information about the query
        int q_chrm;
        try{
            q_chrm = stoi(split_string(query_list[q_idx], ':')[0]);
        }
        // if the chromosome is not an integer
        // X --> 23; Y --> 24; M --> 25
        catch (invalid_argument &e){
            if (split_string(query_list[q_idx], ':')[0] == "X"){
                q_chrm = 23;
            }
            else if (split_string(query_list[q_idx], ':')[0] == "Y"){
                q_chrm = 24;
            }
            else if (split_string(query_list[q_idx], ':')[0] == "MT"){
                q_chrm = 25;
            }
            else{
                cout << "Invalid query chromosome:" << query_list[q_idx] << endl;
                // skip this query
                continue;
            }
        }

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
