#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <sstream>
#include <stdexcept>

#include "utils.h"
#include "decompress.h"

using namespace std;

//string zlib_compress(string in_data);
//uint32_t* fastpfor_vb_compress(vector<uint32_t> in_data);

///*
// * Function to compress a block
// * @param block: vector<vector<string>> - block of data
// * @param codecs_list: vector<string> - list of codecs to use for each column
// * @return compressed_block: vector<string> - compressed block of data
// */
//vector<string> compress_block(vector<vector<string>> block,
//                              vector<string> codecs_list){
//    vector<string> compressed_block;
//
//    for (int col_i = 0; col_i < block.size(); col_i++){
//        if (codecs_list[col_i] == "zlib"){
//            // convert column from vector of strings to one string
//            string column_string = convert_vector_str_to_string(block[col_i]);
//            // compress string with zlib
//            string compressed_string = zlib_compress(column_string);
//            compressed_block.push_back(compressed_string);
//        }
//        else if(codecs_list[col_i] == "fpfVB"){
//            // convert column from vector of strings to vector of integers
//            vector<uint32_t> column_ints = convert_vector_to_int(block[col_i]);
//            // compress vector of integers with fastpfor
//            uint32_t* compressed_ints = fastpfor_vb_compress(column_ints);
//        }
//        else {
//            cout << "ERROR: Codec not recognized: " << codecs_list[col_i] << endl;
//            exit(1);
//        }
//    }
////    // compress with a different codec for each column
////    for (int i = 0; i < codecs_list.size(); i++){
////        if (codecs_list[i] == "zlib"){
////            // convert each column in a block to a string
////            vector<string> column_strings;
////            for (int i = 0; i < block.size(); i++){
////                column_strings.push_back(convert_vector_str_to_string(block[i]));
////            }
////
////            // compress each column in a block
////            for (int i = 0; i < column_strings.size(); i++){
////                compressed_block.push_back(zlib_compress(column_strings[i]));
////            }
////        }
////        else {
////            cout << "ERROR: Codec not recognized: " << codecs_list[i] << endl;
////            exit(1);
////        }
////    }
//
//    return compressed_block;
//}

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


#include "FastPFor/headers/variablebyte.h"
#include "FastPFor/headers/pfor.h"
#include "FastPFor/headers/simdvariablebyte.h"
#include "FastPFor/headers/optpfor.h"


using namespace FastPForLib;

/*
 * use fastpfor variable byte to compress a vector of integers
 */
uint32_t* fastpfor_vb_compress(vector<uint32_t> in_data, size_t& compressedSize){
    FastPForLib::VariableByte vb;

    // Compress the integer array
    vector<uint32_t> compressed(in_data.size() * 2); // Allocate space for compressed data
//    size_t compressedSize; // variable to store the number of compressed values
    vb.encodeArray(in_data.data(), in_data.size(), compressed.data(), compressedSize);

    // Resize the compressed vector to fit the actual compressed data
//    compressed.resize(compressedSize);

    // allocate memory for compressed data
    uint32_t* compressed_data = new uint32_t[compressed.size()];
    copy(compressed.begin(), compressed.end(), compressed_data);

//    // decompress
//    vector<uint32_t> decompressed(in_data.size());
//    size_t decompressedSize; // variable to store the number of decompressed values
//    vb.decodeArray(compressed.data(), compressed.size(), decompressed.data(), decompressedSize);
//    cout << "Decompressed integers: ";
//    for (int i : decompressed) {
//        cout << i << " ";
//    }
//    cout << std::endl;

    return compressed_data;
}

/*
 * use fastpfor variable byte delta to compress a vector of integers
 */
vector<uint32_t> fastpfor_vb_delta_compress(vector<uint32_t> in_data, size_t& compressedSize){
    FastPForLib::VariableByte vb;
    vector<uint32_t> deltas;
    deltas.reserve(in_data.size());

    // Compute deltas between consecutive values
    for (size_t i = 1; i < in_data.size(); ++i) {
        deltas.push_back(in_data[i] - in_data[i - 1]);
    }

    // Encode deltas using FastPFor's variable byte encoding
    vector<uint32_t> encoded_deltas(in_data.size() * 2);
    size_t encoded_size;
    vb.encodeArray(deltas.data(), deltas.size(), encoded_deltas.data(), encoded_size);
    encoded_deltas.resize(encoded_size);

    return encoded_deltas;
}


///*
// * use fastpfor library to decompress a string
// */
//vector<uint32_t> fastpfor_simbd_vb_compress(const std::vector<uint32_t>& input_data) {
//    FastPForLib::SIMDVariableByte vb;
//
//    // Compress the integer array
//    vector<uint32_t> compressed(input_data.size() * 2); // Allocate space for compressed data
//    size_t compressedSize; // variable to store the number of compressed values
//    vb.encodeArray(input_data.data(), input_data.size(), compressed.data(), compressedSize);
//
//    // Resize the compressed vector to fit the actual compressed data
//    compressed.resize(compressedSize);
//
//    return compressed;
//}
//
///*
// * use fastpfor optpfor to compress a vector of integers
// */
//vector<uint32_t> optfor_compress(const std::vector<uint32_t>& input_data) {
//    FastPForLib::OPTPFor optpfor;
//    vector<uint32_t> compressed_data(input_data.size() * 2); // Allocate space for compressed data
//    size_t compressed_size;
//    optpfor.encodeArray(input_data.data(), input_data.size(), compressed_data.data(), compressed_size);
//    compressed_data.resize(compressed_size); // Resize to actual compressed size
//    return compressed_data;
//}
//
//vector<uint32_t> bp_compress(const std::vector<uint32_t>& input_data) {
//    BP32<true> bp32; // Use BP32 for 32-bit data, BP64 for 64-bit data
//    std::vector<uint32_t> compressed_data(input_data.size() * 2); // Allocate space for compressed data
//    size_t compressed_size;
//    bp32.encodeArray(input_data.data(), input_data.size(), compressed_data.data(), compressed_size);
//    compressed_data.resize(compressed_size); // Resize to actual compressed size
//    return compressed_data;
//}


