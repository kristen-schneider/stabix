#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <sstream>
#include <stdexcept>

#include "utils.h"
// include headers from FastPFor
#include "headers/codecfactory.h"
#include "headers/deltautil.h"


using namespace std;
using namespace FastPForLib;


string zlib_compress(string in_data);

/*
 * Function to compress a block
 * @param block: vector<vector<string>> - block of data
 * @param codecs_list: vector<string> - list of codecs to use for each column
 * @return compressed_block: vector<string> - compressed block of data
 */
vector<string> compress_block(vector<vector<string>> block,
                              vector<string> codecs_list){
    vector<string> compressed_block;

    for (int col_i = 0; col_i < block.size(); col_i++){
        if (codecs_list[col_i] == "zlib"){
            // convert each column in a block to a string
            string column_string = convert_vector_to_string(block[col_i]);
            // compress each column in a block
            string compressed_string = zlib_compress(column_string);
            compressed_block.push_back(compressed_string);
        }
        else {
            cout << "ERROR: Codec not recognized: " << codecs_list[col_i] << endl;
            exit(1);
        }
    }
//    // compress with a different codec for each column
//    for (int i = 0; i < codecs_list.size(); i++){
//        if (codecs_list[i] == "zlib"){
//            // convert each column in a block to a string
//            vector<string> column_strings;
//            for (int i = 0; i < block.size(); i++){
//                column_strings.push_back(convert_vector_to_string(block[i]));
//            }
//
//            // compress each column in a block
//            for (int i = 0; i < column_strings.size(); i++){
//                compressed_block.push_back(zlib_compress(column_strings[i]));
//            }
//        }
//        else {
//            cout << "ERROR: Codec not recognized: " << codecs_list[i] << endl;
//            exit(1);
//        }
//    }

    return compressed_block;
}

/*
 * Function to compress a string
 * @param in_data: string - input data
 * @return outstring: string - compressed data using zlib
 */
string zlib_compress(string in_data){
    // https://gist.github.com/gomons/9d446024fbb7ccb6536ab984e29e154a
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));

    zs.next_in = (Bytef*)in_data.data();
    zs.avail_in = in_data.size();           // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

/*
 * use fastpfor library to compress a string
 */
string fastpfor_compress(string in_data){
    //
    std::vector<uint32_t> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<uint32_t> compressedData;

    FastPForLib::VariableByte vb;
    vb.encodeArray(data.data(), data.size(), compressedData);

    // Handle compressedData as needed

    std::cout << "Original data size: " << sizeof(data[0]) * data.size() << " bytes\n";
    std::cout << "Compressed data size: " << compressedData.size() * sizeof(compressedData[0]) << " bytes\n";

    return 0;
}
