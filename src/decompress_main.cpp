#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

#include "decompress.h"
#include "decompress_main.h"
#include "header.h"
#include "index.h"
#include "indexers.h"
#include "stabix_except.h"

using namespace std;
namespace fs = std::filesystem;

vector<int> query_genomic_idx_gene(
    int gene_chrm, int gene_bp_start, int gene_bp_end,
    map<int, map<int, vector<unsigned int>>> genomic_index_info_by_location) {

    tuple<int, unsigned int> gene_blocks = get_start_end_block_idx_single(
        gene_chrm, gene_bp_start, gene_bp_end, genomic_index_info_by_location);

    // if start or end is -1, return empty set
    if (get<0>(gene_blocks) == -1 || get<1>(gene_blocks) == -1) {
        return {};
    }

    vector<int> blocks;

    // fill blocks with all blocks between start and end block indexes
    for (int block_idx = get<0>(gene_blocks); block_idx <= get<1>(gene_blocks);
         block_idx++) {
        blocks.push_back(block_idx);
    }
    return blocks;
}

unordered_set<int> query_abs_idx(string path, string config_query,
                                 BlockLineMap block_line_map) {
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
        throw invalid_argument("Could not parse \"" + config_query +
                               "\". Expected something like \"<= 0.3\".");
    }

    auto index = PValIndexer(path, &block_line_map);
    index.read_index();
    return index.compare_query(val, op);
}

int decompress_main(string config_path) {
    cout << "Reading config options from: " << config_path << endl;
    map<string, string> config_options = read_config_file(config_path);
    add_default_config_options(config_options);
    return decompress_main_by_map(config_options);
}

int decompress_main_by_map(map<string, string> config_options) {
    // DECOMPRESSION STEPS

    // - queries
    vector<string> index_types = {"genomic"};
    string query_genomic = config_options["genomic"];
    map<string, vector<vector<string>>> genomic_query_list =
        read_bed_file(query_genomic);

    // TODO: get query types for other optional queries
    string extra_index = config_options["extra_index"];
    int second_index_col_idx;
    string second_index_threshold = "";
    // add extra indices to index_types
    if (extra_index != "None") {
        vector<string> extra_indices_list = split_string(extra_index, ',');
        index_types.insert(index_types.end(), extra_indices_list.begin(),
                           extra_indices_list.end());

        // get second index col idx from config
        second_index_col_idx = stoi(config_options["col_idx"]);
        // get threshold for second index
        second_index_threshold = config_options["threshold"];
    }

    // -out
    auto out_dir_path = fs::path(config_options["index_dir"]);
    auto compressed_file = out_dir_path / "rows.grlz";

    string query_output_file_name = fs::path(config_options["out_path"]);
    ofstream query_output_stream;
    query_output_stream.open(query_output_file_name, ios::trunc);
    if (!query_output_stream.is_open()) {
        throw StabixExcept("Error: could not open output file.");
    }
    query_output_stream.close();

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

    cout << "...Done." << endl;

    // 3. opening index file
    vector<string> index_names = {"genomic"};
    if (extra_index != "None") {
        index_names.push_back(extra_index);
    }
    auto index_paths = index_paths_of(out_dir_path, index_names);
    auto genomic_index_path = index_paths[0];
    auto extra_index_path = index_paths[1];

    // reading genomic index
    cout << "Reading genomic index..." << endl;
    ifstream genomic_index_file(
        genomic_index_path); // TODO: remove: these next fns
                             // should take path, not stream
    map<int, map<int, vector<unsigned int>>> genomic_index_info_by_location =
        read_genomic_index_by_location(genomic_index_path);

    map<int, vector<unsigned int>> genomic_index_info_by_block =
        read_genomic_index_by_block(genomic_index_path);

    auto block_line_map = BlockLineMap(genomic_index_path);
    genomic_index_file.close();
    cout << "...Done." << endl;

    // TODO: perform a query by gene, not all blocks and all blocks
    // 4. get and aggregate blocks associated with each query

    // get all statistic-based blocks first
    auto statistic_blocks = unordered_set<int>();
    vector<int> sorted_statistic_blocks;
    if (extra_index == "None") {
        cout << "Not extra indexes being queried." << endl;
    } else {
        cout << "Getting blocks for " << extra_index << "..." << endl;
        statistic_blocks = query_abs_idx(
            extra_index_path, second_index_threshold, block_line_map);

        // sort statistical blocks for faster intersection
        sorted_statistic_blocks =
            vector<int>(statistic_blocks.begin(), statistic_blocks.end());
        sort(sorted_statistic_blocks.begin(), sorted_statistic_blocks.end());
        cout << "...Done." << endl;
    }

    // if there are no statistic-based blocks, return
    if (sorted_statistic_blocks.empty()) {
        cout << "No blocks found for statistic query." << endl;
        return 0;
    }

    query_output_stream.open(query_output_file_name, ios::app);

    cout << "Querying data by gene..." << endl;
    // for each gene in genomic_query_list get genomic blocks
    // there can be multiple chmr,bp locations
    for (const auto &gene : genomic_query_list) {
        // get gene
        string geneName = gene.first;

        //        // debug
        //        cout << geneName << ", ";
        //        if (geneName == "ACO2") {
        //            int bug;
        //        }
        // debug

        int geneStatHits = 0;
        const auto &geneLocations = gene.second;

        // iterate through each gene location
        for (const auto &geneLocation : geneLocations) {
            int gene_chrm;
            int gene_bp_start;
            int gene_bp_end;

            try {
                gene_chrm = stoi(geneLocation[0]);
            } catch (const invalid_argument &e) {
                if (geneLocation[0] == "X") {
                    gene_chrm = 23;
                } else if (geneLocation[0] == "Y") {
                    gene_chrm = 24;
                } else if (geneLocation[0] == "MT") {
                    gene_chrm = 25;
                } else {
                    // ignore invalid values
                    gene_chrm = -1;
                }
            }

            gene_bp_start = stoi(geneLocation[1]);
            gene_bp_end = stoi(geneLocation[2]);
            // get genomic blocks
            vector<int> curr_genome_blocks =
                query_genomic_idx_gene(gene_chrm, gene_bp_start, gene_bp_end,
                                       genomic_index_info_by_location);

            // if there are no genomic blocks to decompress, go to
            // next geneLocation
            if (curr_genome_blocks.empty()) {
                continue;
            }

            // get intersection of genomic blocks and statistic blocks
            auto gene_statistic_intersection_blocks = vector<int>();

            set_intersection(
                curr_genome_blocks.begin(), curr_genome_blocks.end(),
                sorted_statistic_blocks.begin(), sorted_statistic_blocks.end(),
                back_inserter(gene_statistic_intersection_blocks));

            // if there are no blocks to decompress, go to next
            // geneLocation
            if (gene_statistic_intersection_blocks.empty()) {
                continue;
            }

            // decompress blocks
            for (int block_to_decompress : gene_statistic_intersection_blocks) {
                size_t block_size_decomp = -1;
                // if there are only two block sizes, block size is fixed except
                // for last block
                if (block_sizes_list.size() == 2) {
                    if (block_to_decompress < stoi(num_blocks) - 1) {
                        block_size_decomp = stoi(block_sizes_list[0]);
                    } else if (block_to_decompress == stoi(num_blocks) - 1) {
                        block_size_decomp = stoi(block_sizes_list[1]);
                    }
                } else {
                    // if there are more than two block sizes,
                    // block size is variable
                    block_size_decomp =
                        stoi(block_sizes_list[block_to_decompress]);
                }
                vector<string> decompressed_block;
                unsigned int block_header_length;
                unsigned int block_length;
                if (block_to_decompress == 0) {
                    block_header_length =
                        stoi(block_header_end_bytes_list[block_to_decompress]);
                    block_length =
                        stoi(block_end_bytes_list[block_to_decompress]) -
                        block_header_length;
                } else {
                    block_header_length =
                        stoul(
                            block_header_end_bytes_list[block_to_decompress]) -
                        stoul(block_end_bytes_list[block_to_decompress - 1]);
                    block_length =
                        stoul(block_end_bytes_list[block_to_decompress]) -
                        stoul(block_end_bytes_list[block_to_decompress - 1]) -
                        block_header_length;
                }
                unsigned int start_byte = get_start_byte(
                    block_to_decompress, genomic_index_info_by_block);

                file.seekg(start_byte, ios::beg);
                char block_header_bytes[block_header_length];
                file.read(block_header_bytes, block_header_length);
                // decompress block header
                string block_header_bitstring =
                    string(block_header_bytes, block_header_length);
                string block_header = zlib_decompress(block_header_bitstring);
                vector<string> block_header_list =
                    split_string(block_header, ',');
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
                        col_bytes =
                            block_length - stoi(block_header_list[col_idx - 1]);
                    } else {
                        col_bytes = stoi(block_header_list[col_idx]) -
                                    stoi(block_header_list[col_idx - 1]);
                    }
                    string col_bitstring =
                        block_bitstring.substr(curr_block_byte, col_bytes);
                    curr_block_byte += col_bytes;
                    size_t compressed_size = col_bitstring.size();

                    string col_decompressed =
                        decompress_column(col_bitstring, col_codec,
                                          compressed_size, block_size_decomp);
                    decompressed_block.push_back(col_decompressed);
                }

                // filter block based on statistical query
                vector<int> statistical_hits = {};
                // TODO: col_idx for pVal idx should be stored in index
                string statistic_col = decompressed_block[second_index_col_idx];
                vector<string> split_statistic_col =
                    split_string(statistic_col, ',');
                vector<float> split_statistic_float = {};

                string chrm_col = decompressed_block[0];
                string bp_col = decompressed_block[1];
                vector<string> split_chrm_col = split_string(chrm_col, ',');
                vector<string> split_bp_col = split_string(bp_col, ',');

                for (string val : split_statistic_col) {
                    try {
                        split_statistic_float.push_back(stof(val));
                    } catch (const invalid_argument &e) {
                        // ignore invalid values
                        split_statistic_float.push_back(-INFINITY);
                    }
                }

                // compare each value in the statistic column to the threshold
                for (int v = 0; v < split_statistic_float.size(); v++) {
                    if (compare_values(second_index_threshold,
                                       split_statistic_float[v])) {
                        geneStatHits += 1;
                        //
                        // if statistical hit is correct, check chrm and bp
                        int hit_chrm;
                        try {
                            hit_chrm = stoi(split_chrm_col[v]);
                        } catch (const invalid_argument &e) {
                            if (split_chrm_col[v] == "X") {
                                hit_chrm = 23;
                            } else if (split_chrm_col[v] == "Y") {
                                hit_chrm = 24;
                            } else if (split_chrm_col[v] == "MT") {
                                hit_chrm = 25;
                            } else {
                                // ignore invalid values
                                hit_chrm = -1;
                            }
                        }
                        int hit_bp = stoi(split_bp_col[v]);
                        if (hit_chrm == gene_chrm && hit_bp >= gene_bp_start &&
                            hit_bp <= gene_bp_end) {
                            statistical_hits.push_back(v);
                        }
                    }
                }

                // if there are no statistical hits, skip to next gene
                if (statistical_hits.empty()) {
                    continue;
                }

                // TODO: include chrm and bp col in config

                // if there are no hits, skip to next gene
                if (statistical_hits.empty()) {
                    continue;
                }
                // get final block to write to output
                vector<string> final_decompressed_block[stoi(num_columns)];
                for (int col_idx = 0; col_idx < stoi(num_columns); col_idx++) {
                    string col = decompressed_block[col_idx];
                    vector<string> split_col = split_string(col, ',');
                    for (int hit : statistical_hits) {
                        final_decompressed_block[col_idx].push_back(
                            split_col[hit]);
                    }
                }
                // transpose decompressed block with just the final hits
                for (int row = 0; row < final_decompressed_block[0].size();
                     row++) {
                    for (int col = 0; col < stoi(num_columns); col++) {
                        query_output_stream
                            << final_decompressed_block[col][row];
                        if (col < stoi(num_columns) - 1) {
                            query_output_stream << "\t";
                        }
                    }
                    query_output_stream << endl;
                }
            }
        }
    }

    cout << "...Done." << endl;

    // close output file
    query_output_stream.close();
    cout << endl << "---Decompression Complete---" << endl;

    return 0;
}
