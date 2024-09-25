#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "decompress.h"
#include "header.h"
#include "index.h"
#include "indexers.h"

using namespace std;
namespace fs = std::filesystem;

unordered_set<int> query_genomic_idx(vector<string> query_list,
                                     map<int, map<int, vector<int>>> genomic_index_info_by_location,
                                     map<int, vector<int>> genomic_index_info_by_block) {

    vector<tuple<int, int>> all_query_block_indexes = get_start_end_block_idx(
        query_list,
        genomic_index_info_by_location);
    unordered_set<int> blocks;

    for (int q_idx = 0; q_idx < all_query_block_indexes.size(); q_idx++) {
        // cout << "...decompressing query " << q_idx + 1 << ": "
        //      << query_list[q_idx] << endl;
        //
        // output_file << "Query: " << query_list[q_idx] << endl;

        tuple<int, int> query_start_end_byte = all_query_block_indexes[q_idx];
        int start_block_idx = get<0>(query_start_end_byte);
        int end_block_idx = get<1>(query_start_end_byte);

        // if start and end block indexes are -1, query not found in index
        if (start_block_idx == -1 && end_block_idx == -1) {
//            cout << "Query " << q_idx << " NOT found in genomic index." << endl;
            continue;
        }
        else{
            // if start block is -1; it means that the query starts before the first block
            // make start block idx 0
            if (start_block_idx == -1) {
                start_block_idx = 0;
            }
            for (int block_idx = start_block_idx; block_idx <= end_block_idx; block_idx++) {
                blocks.insert(block_idx);
            }
        }
    }

    return blocks;
}

/*
 * make a map whose key is a block idx and whose value is a list of genes in that block
 *
 */
map<int, vector<string>> make_block_to_gene_map(string bed_file,
                                                map<int, vector<int>> genomic_index_info_by_block,
                                                map<int, map<int, vector<int>>> genomic_index_info_by_location) {

    map<int, vector<string>> block_to_gene_map;

    // check if file exists
    ifstream bed_stream(bed_file);
    if (!bed_stream.good()) {
        cout << "ERROR: Bed file does not exist: " << bed_file << endl;
        exit(1);
    }
    // read bed file, splitting by delimiter
    // store as "chrm:bp_start-bp_end"
    string line;
    unordered_set<int> single_gene_blocks;
    while (getline(bed_stream, line)) {
        // skip empty lines
        if (line.empty()) {
            continue;
        }
        // split line by tab
        vector<string> single_gene;

        istringstream iss(line);
        string token;
        vector<string> tokens;
        while (std::getline(iss, token, ' '))
            tokens.push_back(token);
        string chrm = tokens[0];
        string bp_start = tokens[1];
        string bp_end = tokens[2];
        string gene = tokens[3];
        single_gene.push_back(chrm + ":" + bp_start + "-" + bp_end);
        single_gene_blocks = query_genomic_idx(single_gene,
                                               genomic_index_info_by_location,
                                               genomic_index_info_by_block);
        // assign gene to each block
        // for each block in single_gene_blocks
        // add block to block_to_gene_map and add gene to block_to_gene_map[block]

        for (int block : single_gene_blocks) {
            block_to_gene_map[block].push_back(gene);
        }

        // clear single_gene_blocks
        single_gene_blocks.clear();
    }
    return block_to_gene_map;
}

unordered_set<int> query_abs_idx(string path,
                                 vector<string> config_bins,
                                 string config_query,
                                 BlockLineMap block_line_map) {

    // parse config_bins
    vector<float> bins;

    for (string bin : config_bins) {
        try {
            bins.push_back(stof(bin));
        } catch (const invalid_argument &e) {
            throw invalid_argument("Bin must be a float: " + bin);
        }
    }

    // parse config_query
    float val;
    ComparisonType op;

    regex re("(>|<)(=?)\\s*(\\d*\\.?.*)");
    smatch matches;

    if (regex_search(config_query, matches, re)) {
        if (matches[1].str() == "<") {
            op = matches[2].str() == "=" ? ComparisonType::LessThanOrEqual
                                         : ComparisonType::LessThan;
        } else {
            op = matches[2].str() == "=" ? ComparisonType::GreaterThanOrEqual
                                         : ComparisonType::GreaterThan;
        }

        string val_exp = matches[3].str();

        try {
            val = stof(val_exp);
        } catch (const invalid_argument &e) {
            throw invalid_argument("Value must be a float: " + val_exp);
        }
    } else {
        throw invalid_argument("Could not parse \"" + config_query + "\". Expected something like \"<= 0.3\".");
    }

    auto index = PValIndexer(path, &block_line_map, bins);
    return index.compare_query(val, op);
}

int main(int argc, char *argv[]) {
    // DECOMPRESSION STEPS

//    // set bins
//    auto pvalue_bins = vector<string>{"5e-1", "5e-5", "5.1e-8"};

    // 0. read config options
    // open file, exit
    if (argc != 2) {
        // prevent seg faults
        cout << "1 argument required: config_path" << endl;
        return -1;
    }
    string config_file = argv[1];
    cout << "Reading config options from: " << config_file << endl;
    map<string, string> config_options = read_config_file(config_file);
    add_default_config_options(config_options);

    // - input gwas file
    string gwas_file = config_options["gwas_file"];

    // - queries
    vector<string> index_types = {"genomic"};
    string query_genomic = config_options["genomic"];
    vector<string> genomic_query_list = read_bed_file(query_genomic);


    // TODO: get query types for other optional queries
    string extra_indices = config_options["extra_indices"];
    int second_index_col_idx;
    auto second_index_bins = std::vector<string> {};
    string second_index_threshold = "";
    // add extra indices to index_types
    if (extra_indices != "None") {
        vector<string> extra_indices_list = split_string(extra_indices, ',');
        index_types.insert(index_types.end(), extra_indices_list.begin(),
                           extra_indices_list.end());

        // get second index col idx from config
        second_index_col_idx = stoi(config_options["col_idx"]);
        // get bins for second index
        string second_index_bins_string = config_options["bins"];
        vector<string> bin_string = split_string(second_index_bins_string, ',');
        for (auto &bin : bin_string) {
            second_index_bins.push_back(bin);
        }
        // get threshold for second index
        second_index_threshold = config_options["threshold"];
    }

    int block_size = -1;
    try {
        block_size = stoi(config_options["block_size"]);
    } catch (invalid_argument &e) {
        block_size = -1;
    }

    // - codecs (by data type)
    string codec_int = config_options["int"];
    string codec_float = config_options["float"];
    string codec_str = config_options["string"];
    map<string, string> data_type_codecs = {
            {"int", codec_int},
            {"float", codec_float},
            {"string", codec_str}};

    // -out
    auto gwas_path = fs::path(config_options["gwas_file"]);
    // out dir naming scheme = "gwasfilename_blocksize_out"

    auto out_dir_path = fs::path();
    string compressed_file;
    if (block_size == -1) {
        out_dir_path = gwas_path.parent_path() /
                       (gwas_path.stem().string() +
                        "_map" +
                        "_" + config_options["out_name"]);

        compressed_file = out_dir_path / (gwas_path.stem().string() +
                                          "_map" +
                                          "_" + config_options["out_name"] +
                                          ".grlz");
    } else {
        out_dir_path = gwas_path.parent_path() /
                       (gwas_path.stem().string() +
                        "_" + config_options["block_size"] +
                        "_" + config_options["out_name"]);

        compressed_file = out_dir_path / (gwas_path.stem().string() +
                                          "_" + config_options["block_size"] +
                                          "_" + config_options["out_name"] +
                                          ".grlz");
    }

    ofstream query_output_stream;
    string query_output_file_name =
            out_dir_path / (gwas_path.stem().string() + ".query");
    query_output_stream.open(query_output_file_name, ios::trunc);
    if (!query_output_stream.is_open()) {
        cout << "Error: could not open output file." << endl;
        return 1;
    }
    cout << "Done." << endl << endl;

    // TODO: time decompression steps
    // outfile for decompression times
    fs::create_directories(out_dir_path.parent_path() / "decompression_times");
    fs::path compression_times_file;
    fs::path col_times_file;
    if (block_size == -1) {
        compression_times_file = out_dir_path.parent_path() / "decompression_times" /
                                 (gwas_path.stem().string() +
                                 "_map" +
                                 "_" + config_options["out_name"] + "_decompression.csv");

        col_times_file = out_dir_path.parent_path() / "decompression_times" /
                            (gwas_path.stem().string() +
                            "_map" +
                            "_" + config_options["out_name"] + "_column_decompression.csv");
    } else {
        compression_times_file = out_dir_path.parent_path() / "decompression_times" /
                                 (gwas_path.stem().string() +
                                 "_" + config_options["block_size"] +
                                 "_" + config_options["out_name"] + "_decompression.csv");

        col_times_file = out_dir_path.parent_path() / "decompression_times" /
                         (gwas_path.stem().string() +
                          "_" + config_options["block_size"] +
                          "_" + config_options["out_name"] + "_column_decompression.csv");
    }

    // outfile for column decompression times

    ofstream col_times;
    col_times.open(col_times_file, ios::trunc);
    // write header
    col_times << "block_idx,col_idx,comp_time,col_size,codec" << endl;
    col_times.close();

    // clear contents of output file and close
    query_output_stream.close();
    query_output_stream.open(query_output_file_name, ios::trunc);
    cout << "Done." << endl << endl;

    // 1. open compressed file and read header
    cout << "Opening compressed file and reading header..." << endl;
    // TODO: There needs to be a system to vet quality inputs (such as config)
    ifstream file(compressed_file);

    // start at beginning and read 4 bytes
    file.seekg(0, ios::beg);
    char header_length_bytes[4];
    file.read(header_length_bytes, 4);
    // convert 4 bytes to int
    int header_length = bytes_to_int(header_length_bytes);

    // read header_length bytes starting at byte 4
    file.seekg(4, ios::beg);
    char header_bytes[header_length];
    file.read(header_bytes, header_length);
    // convert header bytes to string
    string header_string = string(header_bytes, header_length);
    // decompress header
    string header = zlib_decompress(header_string);
    vector<string> header_list = split_string(header, ',');

    // parse header
    string num_columns = parse_header_list(header_list, "num columns")[0];
    string num_blocks = parse_header_list(header_list, "num blocks")[0];
    vector<string> column_names_list =
        parse_header_list(header_list, "column names");
    vector<string> codecs_list = parse_header_list(header_list, "codecs");
    vector<string> block_header_end_bytes_list =
        parse_header_list(header_list, "block header end bytes");
    vector<string> block_end_bytes_list =
        parse_header_list(header_list, "block end bytes");
    vector<string> block_sizes_list =
        parse_header_list(header_list, "block sizes");

    // 3. opening index file
    vector<string> index_names = {"genomic", "pval"};
    auto index_paths = index_paths_of(out_dir_path, index_names);
    auto genomic_index_path = index_paths[0];
    auto pval_index_path = index_paths[1];

    // read genomic index
    cout << "Opening genomic index file..." << endl;
    cout << "\t..." << genomic_index_path << endl;

    auto total_blocks_to_decompress = vector<int>();

    // time reading genomic index
    auto read_genomic_index_start = chrono::high_resolution_clock::now();
    ifstream genomic_index_file(
        genomic_index_path); // TODO: remove: these next fns
                             // should take path, not stream
    map<int, map<int, vector<int>>> genomic_index_info_by_location =
            read_genomic_index_by_location(
                    genomic_index_path);

    map<int, vector<int>> genomic_index_info_by_block =
            read_genomic_index_by_block(
                    genomic_index_path);

    auto block_line_map = BlockLineMap(genomic_index_path);
    genomic_index_file.close();
    cout << "Done." << endl << endl;
    auto read_genomic_index_end = chrono::high_resolution_clock::now();

    cout << "Getting genomic blocks..." << endl;
    // 4. get and aggregate blocks associated with each query
    // time query genomic index
    auto query_genomic_index_start = chrono::high_resolution_clock::now();

    // get genomic blocks
    auto genom_blocks = query_genomic_idx(
            genomic_query_list,
            genomic_index_info_by_location,
            genomic_index_info_by_block);

//    // TODO: get gene associated with each block idx
//    map<int, vector<string>> block_to_gene_map = make_block_to_gene_map(query_genomic,
//                                                                        genomic_index_info_by_block,
//                                                                        genomic_index_info_by_location);

    auto query_genomic_index_end = chrono::high_resolution_clock::now();
    cout << "Done." << endl;

    // if there are no genomic blocks, return early. nothing found.
    if (genom_blocks.empty()) {
        cout << endl << "No blocks found for given query." << endl;
        return 0;
        // if there is no p-value query, set the query to all genomic blocks
    }else if( extra_indices == "None"){
        cout << "No extra queries." << endl;
        // set total_blocks_to_decompress to genome blocks
        total_blocks_to_decompress = vector<int>(genom_blocks.begin(), genom_blocks.end());
        // sort blocks
        sort(total_blocks_to_decompress.begin(), total_blocks_to_decompress.end());
        // get blocks from pvalue query
    }else{
        cout << "Getting p-value blocks..." << endl;
        auto query_pval_index_start = chrono::high_resolution_clock::now();
        auto pval_blocks = query_abs_idx(pval_index_path,
                                     second_index_bins,
                                     second_index_threshold,
                                     block_line_map);

        auto query_pval_index_end = chrono::high_resolution_clock::now();
        cout << "Done." << endl;

        // TODO: delete this after debug
        cout << "genome blocks: " << genom_blocks.size() << endl;
        cout << "pval blocks: " << pval_blocks.size() << endl;
//        for (int block : pval_blocks) {
//            cout << block << endl;
//        }

        // get blocks that are in both genomic and p-value queries
        for (int block : genom_blocks) {
            if (pval_blocks.contains(block)) {
                total_blocks_to_decompress.push_back(block);
            }
        }

        // sort blocks
        sort(total_blocks_to_decompress.begin(), total_blocks_to_decompress.end());
    }

    // 5. decompress all blocks for each query
    // open col times file
    col_times.open(col_times_file, ios::app);
    std::cout << "Decompressing " << total_blocks_to_decompress.size()
              << " blocks" << std::endl;
    vector<float> decompress_block_times;
    for (int block_idx : total_blocks_to_decompress) {
        // time decompression
        auto decompress_block_start = chrono::high_resolution_clock::now();
        size_t block_size = -1;
        // if there are only two block sizes, block size is fixed except
        // for last block
        if (block_sizes_list.size() == 2) {
            if (block_idx < stoi(num_blocks) - 1) {
                block_size = stoi(block_sizes_list[0]);
            } else if (block_idx == stoi(num_blocks) - 1) {
                block_size = stoi(block_sizes_list[1]);
            }
        } else {
            // if there are more than two block sizes,
            // block size is variable
            block_size = stoi(block_sizes_list[block_idx]);
        }
        vector<string> decompressed_block;
        int block_header_length;
        int block_length;
        if (block_idx == 0) {
            block_header_length = stoi(block_header_end_bytes_list[block_idx]);
            block_length =
                stoi(block_end_bytes_list[block_idx]) - block_header_length;
        } else {
            block_header_length = stoi(block_header_end_bytes_list[block_idx]) -
                                  stoi(block_end_bytes_list[block_idx - 1]);
            block_length = stoi(block_end_bytes_list[block_idx]) -
                           stoi(block_end_bytes_list[block_idx - 1]) -
                           block_header_length;
        }
        int start_byte = get_start_byte(block_idx, genomic_index_info_by_block);
        cout << "\t...decompressing block " << block_idx
             << ", size: " << block_size << endl;

        file.seekg(start_byte, ios::beg);
        char block_header_bytes[block_header_length];
        file.read(block_header_bytes, block_header_length);
        // decompress block header
        string block_header_bitstring =
            string(block_header_bytes, block_header_length);
        string block_header = zlib_decompress(block_header_bitstring);
        vector<string> block_header_list = split_string(block_header, ',');
        // read in compressed block
        char block_bytes[block_length];
        file.read(block_bytes, block_length);
        // convert to string
        string block_bitstring = string(block_bytes, block_length);
        // decompress block by column
        int curr_block_byte = 0;
        int col_bytes;
        for (int col_idx = 0; col_idx < stoi(num_columns); col_idx++) {
            string col_codec = codecs_list[col_idx];
            if (col_idx == 0) {
                col_bytes = stoi(block_header_list[col_idx]);
            } else if (col_idx == stoi(num_columns) - 1) {
                col_bytes = block_length - stoi(block_header_list[col_idx - 1]);
            } else {
                col_bytes = stoi(block_header_list[col_idx]) -
                            stoi(block_header_list[col_idx - 1]);
            }
            string col_bitstring =
                block_bitstring.substr(curr_block_byte, col_bytes);
            curr_block_byte += col_bytes;
            size_t compressed_size = col_bitstring.size();

            // time decompressing column
            auto col_decompress_start = chrono::high_resolution_clock::now();
            string col_decompressed = decompress_column(
                    col_bitstring,
                    col_codec,
                    compressed_size,
                    block_size);
            auto col_decompress_end = chrono::high_resolution_clock::now();
            col_times << block_idx << "," << col_idx << ","
                      << chrono::duration_cast<chrono::microseconds>(
                             col_decompress_end - col_decompress_start)
                             .count()
                      << "μs," << col_bytes << "," << col_codec << endl;
            decompressed_block.push_back(col_decompressed);
        }

        auto decompress_block_end = chrono::high_resolution_clock::now();
        decompress_block_times.push_back(
            chrono::duration_cast<chrono::microseconds>(
                decompress_block_end - decompress_block_start)
                .count());

        // time writing decompressed block to output file
        auto write_block_start = chrono::high_resolution_clock::now();

        // write decompressed block to output file
        cout << "Writing block " << block_idx << " to output file..." << endl;
        int column_count = stoi(num_columns);


        // TODO: filter the block based on the query
        // only return rows that match the query
        vector<int> hits = {};
        bool compare_result = false;
        string query_col = decompressed_block[second_index_col_idx];
        vector<string> split_query_col_string = split_string(query_col, ',');
        vector<float> split_query_col_float = {};
        for (string val : split_query_col_string) {
            try{
                split_query_col_float.push_back(stof(val));
            }
            catch (const invalid_argument &e) {
                // ignore invalid values
                split_query_col_float.push_back(-INFINITY);
            }
        }

        for (int i = 0; i < split_query_col_float.size(); i++) {
            compare_result = compare_values(second_index_threshold,
                                            split_query_col_float[i]);
            if (compare_result) {
                hits.push_back(i);
            }
        }

        // split columns of block into vectors
        vector<string> split_columns[column_count];
        for (int i = 0; i < column_count; i++) {
            split_columns[i] = split_string(decompressed_block[i], ',');
        }
        // allocate new space for filtered block
        // fill in filtered block with only the row indexes that match the query
        vector<string> filtered_block[column_count];
        for (int i = 0; i < column_count; i++) {
            for (int hit : hits) {
                filtered_block[i].push_back(split_columns[i][hit]);
            }
        }

        // write filtered block to output file

        for (int record_i = 0; record_i <= hits.size() - 1; record_i++) {
            for (int col_i = 0; col_i <= column_count - 1; col_i++) {
                auto block_list = filtered_block[col_i];
                string record = block_list[record_i];
                query_output_stream << record << ',';
            }
            query_output_stream << endl;
        }

//        for (int record_i = 0; record_i <= block_size - 1; record_i++) {
//            for (int col_i = 0; col_i <= column_count - 1; col_i++) {
//                auto block_list = split_columns[col_i];
//                string record = block_list[record_i];
//                query_output_stream << record << ',';
//            }
//            query_output_stream << endl;
//        }

        auto write_block_end = chrono::high_resolution_clock::now();
    }
    query_output_stream.close();
    col_times.close();

    // write decompression times to file
    ofstream decompression_times_stream;
    decompression_times_stream.open(compression_times_file, ios::trunc);

    decompression_times_stream << "GWAS file: " << gwas_file << endl;
    decompression_times_stream << "genomic_index_read,"
                               << chrono::duration_cast<chrono::microseconds>(
                                      read_genomic_index_end - read_genomic_index_start)
                                          .count() << "μs" << endl;
    decompression_times_stream << "genomic_index_query,"
                                 << chrono::duration_cast<chrono::microseconds >(
                                          query_genomic_index_end - query_genomic_index_start)
                                            .count() << "μs" << endl;
    // write decompress_block_times
    for (int i = 0; i < decompress_block_times.size(); i++) {
        decompression_times_stream << "block_" << i << "_decompression,"
                                   << decompress_block_times[i] << "μs" << endl;
    }

    decompression_times_stream.close();



    cout << endl << "---Decompression Complete---" << endl;
    return 0;
}
