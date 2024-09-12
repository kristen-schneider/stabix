#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "blocks.h"
#include "compress.h"
#include "header.h"
#include "index.h"
#include "utils.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    // COMPRESSION STEPS

    // 0. read config options
    if (argc != 2) {
        // prevent seg faults
        cout << "1 argument required: config_path" << endl;
        return -1;
    }
    string config_file = argv[1];
    cout << "Reading config options from: " << config_file << endl;
    map<string, string> config_options = read_config_file(config_file);
    add_default_config_options(config_options);

    string gwas_file = config_options["gwas_file"];
    // block size
    int block_size = -1;
    // if block_size cannot be converted to an int, it is a map file
    try {
        block_size = stoi(config_options["block_size"]);
    } catch (invalid_argument &e) {
        block_size = -1;
    }
    // queries
    vector<string> index_types = {"genomic"};
    string query_genomic = config_options["query_genomic"];
    vector<string> genomic_queries = read_bed_file(query_genomic);
    // TODO: get query types for other optional queries
    string extra_indices = config_options["extra_indices"];

    // codecs by data type
    string codec_int = config_options["codecs_int"];
    string codec_float = config_options["float"];
    string codec_str = config_options["string"];
    map<string, string> data_type_codecs = {
        {"int", codec_int},
        {"float", codec_float},
        {"string", codec_str}};



    // creating output directory for generated files
    auto gwas_path = fs::path(config_options["gwas_file"]);
    auto out_dir = gwas_path.parent_path() / (gwas_path.stem().string() + "_output");
    fs::create_directories(out_dir);
    string compressed_file = out_dir / (gwas_path.stem().string() + ".grlz");

    cout << "\t...gwas_file: " << gwas_file << endl;
    cout << "\t...block_size: " << block_size << endl;
    cout << "\t...indexes: " << convert_vector_str_to_string(index_types) << endl;
    cout << "\t...compressed_file: " << compressed_file << endl;

    cout << "Done." << endl << endl;


    // 1. get file information to store in first half of compressed file header
    /* HEADER FIRST HALF
     * num_columns = None
     * num_blocks = None
     * column_names_list = None
     * codecs_list = None
     * codec_headers_list = None
     * block_sizes_tuple = None
     * */
    cout << "Reading file header..." << endl;
    // open gwas file and get first two lines
    ifstream file(gwas_file);
    if (!file.is_open()) {
        cout << "Unable to open gwas_file: " << gwas_file << endl;
        return -1;
    }
    string line_1;
    string line_2;
    getline(file, line_1);
    getline(file, line_2);
    // close file
    file.close();

    // get delimiter
    char delimiter = get_delimiter(line_1);
    // get column names
    vector<string> column_names_list = split_string(line_1, delimiter);
    string column_names_str = convert_vector_str_to_string(column_names_list);
    cout << "\t...column names: " << column_names_str << endl;
    // get number of columns
    int num_columns = column_names_list.size();
    cout << "\t...num columns: " << num_columns << endl;
    cout << "Done." << endl << endl;
    // get column data types
    vector<string> col_data_types = get_data_types(line_2, delimiter);
    // get codecs by data type
    vector<string> codecs_list = get_codecs_by_data_type(col_data_types, data_type_codecs);

    // setting up variables for blocks
    vector<vector<vector<string>>> all_blocks;
    vector<vector<string>> compressed_blocks;
    vector<vector<int>> genomic_index;
    int num_blocks;

    // 2. create blocks
    cout << "Making blocks..." << endl;
    // if block_size == "map", make block sizes by the cm map file
    if (block_size == -1) {
        string map_file = config_options["block_size"];
        map<int, vector<uint32_t>> chrm_block_bp_ends =
                read_cm_map_file(map_file);
        all_blocks = make_blocks_map(gwas_file,
                                     num_columns,
                                     chrm_block_bp_ends,
                                     delimiter,
                                     genomic_index);
    } else {
        all_blocks =
            make_blocks(gwas_file,
                        num_columns,
                        block_size,
                        delimiter,
                        9,
                        genomic_index);
    }
    num_blocks = all_blocks.size();
    if (block_size == -1) {
        cout << "\t...made " << num_blocks << ", 1cM in length." << endl;
    } else {
        cout << "\t...made " << num_blocks << " blocks of size "
             << block_size << " or less." << endl;
    }
    cout << "Done." << endl << endl;


    // 4. compress blocks AND get second half of header
    cout << "Compressing blocks..." << endl;
    // compress each block and add to compressed_blocks
    for (auto const &block : all_blocks) {
        vector<string> compressed_block = compress_block(block, codecs_list);
        compressed_blocks.push_back(compressed_block);
    }

    // write compressed block sizes to file (by column)
    fs::path block_sizes_file;
    if (block_size == -1) {
        block_sizes_file = out_dir / (gwas_path.stem().string() + "_cm_col-sizes.csv");
    }else{
        block_sizes_file = out_dir / (gwas_path.stem().string() +
                                             "_" + to_string(block_size) + "_col-sizes.csv");
    }


//    string block_sizes_file =
//        compressed_file + "_" + to_string(block_size) + "_sizes.csv";
    ofstream block_sizes_out;
    block_sizes_out.open(block_sizes_file);
    // write gwas file name
    block_sizes_out << gwas_file << endl;
    // write header
    block_sizes_out << "block_idx,col_idx,column_size" << endl;
    int block_i = 0;
    for (auto const &block : compressed_blocks) {
        int col_i = 0;
        for (auto const &column : block) {
            // write block_idx, col_idx, column size
            block_sizes_out << block_i << "," << col_i << "," << column.size()
                            << endl;
            col_i++;
        }
        block_i++;
    }
    block_sizes_out.close();
    cout << "Wrote compressed block sizes to: " << block_sizes_file << endl;

    // get block headers
    vector<vector<string>> block_headers;
    for (auto const &block : compressed_blocks) {
        vector<string> block_header = get_block_header(block);
        block_headers.push_back(block_header);
    }

    // compress block headers
    vector<string> compressed_block_headers;
    for (auto const &block_header : block_headers) {
        string string_block_header = convert_vector_str_to_string(block_header);
        string compressed_block_header = zlib_compress(string_block_header);
        compressed_block_headers.push_back(compressed_block_header);
    }
    cout << "Done." << endl << endl;

    // 5. get second half of header
    /* HEADER SECOND HALF
     * compressed block header ends
     * compressed block ends
     * block sizes (tuple)
     */
    // get ending index of each compressed block header and compressed block
    vector<string> block_header_end_bytes;
    vector<string> block_end_bytes;
    int curr_byte_idx = 0;
    for (int curr_block_idx = 0; curr_block_idx < num_blocks;
         curr_block_idx++) {
        int curr_block_header_length =
            compressed_block_headers[curr_block_idx].length();
        curr_byte_idx += curr_block_header_length;
        block_header_end_bytes.push_back(to_string(curr_byte_idx));
        int curr_block_length =
            get_block_length(compressed_blocks[curr_block_idx]);
        curr_byte_idx += curr_block_length;
        block_end_bytes.push_back(to_string(curr_byte_idx));
    }

    string block_sizes = "";
    // if block_size == "map", block sizes are variable
    vector<int> block_sizes_list;
    if (block_size == -1) {
        block_sizes_list = get_block_sizes(all_blocks);
        block_sizes = convert_vector_int_to_string(block_sizes_list);
    }
    // if block_size is fixed, block sizes are the same, except the last block
    // may be
    else {
        int first_block_size = all_blocks[0][0].size();
        int last_block_size = all_blocks[all_blocks.size() - 1][0].size();
        block_sizes =
            to_string(first_block_size) + "," + to_string(last_block_size);
    }

    /*
     * num_columns = None
     * num_blocks = None
     * column_names_list = None
     * codecs_list = None
     * block_header_end_bytes = None
     * block_end_bytes = None
     * block_sizes_tuple = None
     */
    vector<string> header = {
        to_string(num_columns),
        to_string(num_blocks),
        column_names_str,
        convert_vector_str_to_string(codecs_list),
        convert_vector_str_to_string(block_header_end_bytes),
        convert_vector_str_to_string(block_end_bytes),
        block_sizes};

    // 6. compress header
    cout << "Compressing header..." << endl;
    string header_str = convert_vector_str_to_string(header);
    string compressed_header = zlib_compress(header_str);
    cout << "Done." << endl << endl;

    // 7. open file to write compressed header and compressed blocks
    cout << "Writing compressed file to: " << compressed_file << endl;
    ofstream compressed_gwas;
    compressed_gwas.open(compressed_file);
    // clear file contents
    compressed_gwas.clear();

    // 8. write compressed header to file
    // reserve first 4 bytes for size of compressed header
    int compressed_header_size = compressed_header.length();
    char *compressed_header_size_bytes = int_to_bytes(compressed_header_size);
    compressed_gwas.write(compressed_header_size_bytes, 4);
    // write compressed header to file
    compressed_gwas << compressed_header;

    // 9. write compressed blocks to file
    int block_idx = 0;
    for (auto const &block : compressed_blocks) {
        // write compressed block header
        string compressed_block_header = compressed_block_headers[block_idx];
        compressed_gwas << compressed_block_header;

        // write compressed block
        for (auto const &column : block) {
            compressed_gwas << column;
        }
        block_idx++;
    }
    compressed_gwas.close();

    // 10. write genomic index to file
    // get the byte start of each block and add to genomic index
    cout << "Getting genomic index information..." << endl;
    get_byte_start_of_blocks(compressed_header_size,
                             block_header_end_bytes,
                             block_end_bytes,
                             genomic_index);

    // write chrm_bp_byte to (genomic) index file
    vector<string> indexNames = {"genomic"};
    auto indexPaths = index_paths_of(gwas_file, indexNames);
    string genomicIndexPath = indexPaths[0];
    cout << "Writing genomic index file to: " << genomicIndexPath << endl;
    ofstream genomicIndexFile;
    genomicIndexFile.open(genomicIndexPath);

    // write header
    genomicIndexFile << "block_idx,chrm_start,bp_start,line_start,byte_start" << endl;

    // write genomic index to file
    for (int block_idx = 0; block_idx < genomic_index.size(); block_idx++) {
        vector<int> chrm_bp_byte = genomic_index[block_idx];
        genomicIndexFile << block_idx << ","
                            << genomic_index[block_idx][0] << ","
                            << genomic_index[block_idx][1] << ","
                            << genomic_index[block_idx][2] << ","
                            << genomic_index[block_idx][3] << endl;
    }

    genomicIndexFile.close();
    file.close();

    cout << "Done." << endl;

    return 0;
}
