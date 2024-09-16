#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

#include "bxzstr.hpp"
#include "compression_types.hpp"
#include "variablebyte.h"

#include "decompress.h"
#include "header.h"
#include "utils.h"

using namespace std;
namespace fs = std::filesystem;
using namespace FastPForLib;

string ZLIB_HEADER_D = "x\xda";

/*
 * Decompress a column using the specified codec
 * @param compressed_column: string, the compressed column
 * @param codec: string, the codec used to compress the column
 * @return: string, the decompressed column
 */
string decompress_column(string compressed_column,
                         string codec,
                         size_t compressedSize,
                         size_t block_size) {
    if (codec == "zlib") {
        return zlib_decompress(compressed_column);
    } else if (codec == "deflate") {
        return deflate_decompress(compressed_column);
    } else if (codec == "bz2") {
        return bz2_decompress(compressed_column);
    } else if (codec == "xz") {
        return xz_decompress(compressed_column);
    } else if (codec == "zstd") {
        return zstd_decompress(compressed_column);
    } else if (codec == "fpfVB") {
        vector<uint32_t> compressed_column_ints =
            convert_string_to_vector_unsignedlong(compressed_column);
        vector<uint32_t> decompressed_column = fastpfor_vb_decompress(
            compressed_column_ints, compressedSize, block_size);
        string decompressed_column_str = convert_vector_uint32_to_string(
            decompressed_column.data(), decompressed_column.size());
        return decompressed_column_str;
    } else {
        throw invalid_argument("ERROR: Codec not recognized: " + codec);
    }
}

/*
 * Decompress a string using zlib
 * @param in_data: string, the zlib compressed string
 * @return: string, the decompressed string
 */
string zlib_decompress(string in_data) {

    // add back the zlib header
    in_data = add_zlib_header(in_data, ZLIB_HEADER_D);

    // https://gist.github.com/gomons/9d446024fbb7ccb6536ab984e29e154a

    z_stream zs; // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
        throw(std::runtime_error("inflateInit failed while decompressing."));

    zs.next_in = (Bytef *)in_data.data();
    zs.avail_in = in_data.size(); // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) { // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

/*
 * Decompress a block using FastPFor's variable byte decoding
 * @param in_data: uint32_t*, the compressed block
 * @param compressedSize: size_t, the size of the compressed block
 * @param block_size: size_t, the size of the block
 * @return: vector<uint32_t>, the decompressed block
 */
vector<uint32_t> fastpfor_vb_decompress(vector<uint32_t> in_data,
                                        size_t compressedSize,
                                        size_t block_size) {
    FastPForLib::VariableByte vb;
    size_t decompressedSize = compressedSize * 2;
    vector<uint32_t> decompressed(block_size);
    vb.decodeArray(in_data.data(), in_data.size(), decompressed.data(),
                   decompressedSize);

    // Resize the decompressed vector to fit the actual decompressed data
    decompressed.resize(block_size);

    return decompressed;
}

// vector<uint32_t> fastpfor_vb_delta_decompress(const std::vector<uint32_t>&
// encoded_deltas) {
//     FastPForLib::VariableByte vb;
//
//     // Decode deltas using FastPFor's variable byte decoding
//     vector<uint32_t> decoded_deltas(encoded_deltas.size() * 2);
//     size_t decoded_size;
//     vb.decodeArray(encoded_deltas.data(), encoded_deltas.size(),
//     decoded_deltas.data(), decoded_size);
//     decoded_deltas.resize(decoded_size);
//
//     // Reconstruct original sequence by adding deltas to previous values
//     vector<uint32_t> original_sequence;
//     original_sequence.reserve(decoded_deltas.size() + 1);
//     uint32_t prev_value = 0; // Assuming the first value is 0, adjust if
//     needed for (const auto& delta : decoded_deltas) {
//         prev_value += delta;
//         original_sequence.push_back(prev_value);
//     }
//
//     return original_sequence;
// }

string bxzstr_decompress(string comp, bxz::Compression codec) {
    std::stringstream in;
    bxz::istream from(in);
    in << magicNumberOf(codec);
    in << comp;
    std::stringstream out;
    out << from.rdbuf();
    return out.str();
}

string deflate_decompress(string inputData) {
    return bxzstr_decompress(inputData, bxz::z);
}

string bz2_decompress(string inputData) {
    return bxzstr_decompress(inputData, bxz::bz2);
}

string xz_decompress(string inputData) {
    return bxzstr_decompress(inputData, bxz::lzma);
}

string zstd_decompress(string inputData) {
    return bxzstr_decompress(inputData, bxz::zstd);
}
