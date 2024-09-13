#include <bzlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>
#include <zlib.h>

#include "variablebyte.h"
// #include "zipint.h"
#include "bxzstr.hpp"
#include "compression_types.hpp"

#include "decompress.h"
#include "header.h"
#include "utils.h"

using namespace FastPForLib;
using namespace std;

string ZLIB_HEADER_C = "x\xda";
/*
 * Function to compress a string using zlib
 * @param in_data: string - input data
 * @return outstring: string - compressed data using zlib
 */
string zlib_compress(string in_data) {
    // TODO: replace with bxzstr

    // https://gist.github.com/gomons/9d446024fbb7ccb6536ab984e29e154a
    z_stream zs; // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));

    zs.next_in = (Bytef *)in_data.data();
    zs.avail_in = in_data.size(); // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) { // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    // remove zlib header
    string outstring_noheader = remove_zlib_header(outstring, ZLIB_HEADER_C);
    return outstring_noheader;
    //    return outstring;
}


/*
 * Decompress a string using fastpfor variable byte decoding
 * @param in_data: uint32_t*, the compressed data
 * @param compressedSize: size_t, the size of the compressed data
 * @return: vector<uint32_t>, the decompressed data
 */
vector<uint32_t> fastpfor_vb_compress(vector<uint32_t> in_data,
                                      size_t &compressedSize) {
    FastPForLib::VariableByte vb;

    // Compress the integer array
    vector<uint32_t> compressed(in_data.size() *
                                2); // Allocate space for compressed data
    vb.encodeArray(in_data.data(), in_data.size(), compressed.data(),
                   compressedSize);
    // resize compressed data
    compressed.resize(compressedSize);

    return compressed;
}


/*
 * Compress a string using bxz variable byte decoding
 * @param in_data: string, the compressed data
 * @return: string, the decompressed data
 */
string bxzstr_compress(string input, bxz::Compression codec) {
    std::stringstream out;
    bxz::ostream to(out, codec, 9);
    to << input;
    to.flush();
    return out.str().substr(magicNumberCullSize(codec));
}


/*
 * Compress a string using zlib deflate
 * @param inputData: string, the input data
 * @return: string, the compressed data
 */
string deflate_compress(string inputData) {
    return bxzstr_compress(inputData, bxz::z);
}


/*
 * Compress a string using bzip2
 * @param inputData: string, the input data
 * @return: string, the compressed data
 */
string bz2_compress(string inputData) {
    return bxzstr_compress(inputData, bxz::bz2);
}


/*
 * Compress a string using xz
 * @param inputData: string, the input data
 * @return: string, the compressed data
 */
string xz_compress(string inputData) {
    return bxzstr_compress(inputData, bxz::lzma);
}


/*
 * Compress a string using zstd
 * @param inputData: string, the input data
 * @return: string, the compressed data
 */
string zstd_compress(string inputData) {
    return bxzstr_compress(inputData, bxz::zstd);
}