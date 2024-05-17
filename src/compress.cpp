#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <sstream>
#include <stdexcept>

#include "FastPFor/headers/variablebyte.h"

#include "utils.h"
#include "decompress.h"

using namespace FastPForLib;
using namespace std;

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
 * Decompress a string using fastpfor variable byte decoding
 * @param in_data: uint32_t*, the compressed data
 * @param compressedSize: size_t, the size of the compressed data
 * @return: vector<uint32_t>, the decompressed data
 */
vector <uint32_t> fastpfor_vb_compress(vector<uint32_t> in_data,
                                       size_t& compressedSize){
    FastPForLib::VariableByte vb;

    // Compress the integer array
    vector<uint32_t> compressed(in_data.size() * 2); // Allocate space for compressed data
    vb.encodeArray(in_data.data(), in_data.size(), compressed.data(), compressedSize);
    // resize compressed data
    compressed.resize(compressedSize);

    return compressed;
}

///*
// * use fastpfor variable byte delta to compress a vector of integers
// * @param in_data: vector<uint32_t> - input data
// * @param compressedSize: size_t - size of compressed data
// * @return encoded_deltas: vector<uint32_t> - compressed data
// */
//vector<uint32_t> fastpfor_vb_delta_compress(vector<uint32_t> in_data, size_t& compressedSize){
//    FastPForLib::VariableByte vb;
//    vector<uint32_t> deltas;
//    deltas.reserve(in_data.size());
//
//    // Compute deltas between consecutive values
//    for (size_t i = 1; i < in_data.size(); ++i) {
//        deltas.push_back(in_data[i] - in_data[i - 1]);
//    }
//
//    // Encode deltas using FastPFor's variable byte encoding
//    vector<uint32_t> encoded_deltas(in_data.size() * 2);
//    size_t encoded_size;
//    vb.encodeArray(deltas.data(), deltas.size(), encoded_deltas.data(), encoded_size);
//    encoded_deltas.resize(encoded_size);
//
//    return encoded_deltas;
//}


