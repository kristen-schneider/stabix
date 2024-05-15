#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <sstream>
#include <stdexcept>

#include "utils.h"

using namespace std;

string zlib_decompress(string compressed_string);
vector<uint32_t> fastpfor_vb_decompress(uint32_t* in_data, size_t uncompressedSize, size_t block_size);
vector<uint32_t> fastpfor_vb_delta_decompress(const std::vector<uint32_t>& encoded_deltas);

/*
 * Decompress a column using the specified codec
 * @param compressed_column: string, the compressed column
 * @param codec: string, the codec used to compress the column
 * @return: string, the decompressed column
 */
string decompress_column(string compressed_column,
                         string codec,
                         size_t compressedSize,
                         size_t block_size){
    string decompressed_column_str;
    if (codec == "zlib"){
        decompressed_column_str = zlib_decompress(compressed_column);
        return decompressed_column_str;
    }
    else if (codec == "fpfVB"){
        vector<uint32_t> decompressed_column;
        decompressed_column = fastpfor_vb_decompress((uint32_t*)compressed_column.c_str(),
                                                     compressedSize,
                                                     block_size);
        decompressed_column_str = convert_vector_int_to_string(decompressed_column);
        return decompressed_column_str;
    }
    else {
        cout << "ERROR: Codec not recognized: " << codec << endl;
        exit(1);
    }
}

/*
 * Decompress a string using zlib
 * @param in_data: string, the zlib compressed string
 * @return: string, the decompressed string
 */
string zlib_decompress(string in_data){
    // https://gist.github.com/gomons/9d446024fbb7ccb6536ab984e29e154a

    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
        throw(std::runtime_error("inflateInit failed while decompressing."));

    zs.next_in = (Bytef*)in_data.data();
    zs.avail_in = in_data.size();           // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
        << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

#include "FastPFor/headers/variablebyte.h"
#include "FastPFor/headers/pfor.h"
#include "FastPFor/headers/fastpfor.h"


using namespace FastPForLib;

/*
 * use fastpfor library to decompress a string
 * @param in_data: uint32_t*, the compressed data
 * @param compressed: size_t, the size of the compressed data

 */
vector<uint32_t> fastpfor_vb_decompress(uint32_t* in_data,
                                        size_t compressedSize,
                                        size_t block_size) {
    FastPForLib::VariableByte vb;
//    size_t uncompressedSize = compressedSize * 2;
    vector<uint32_t> decompressed(block_size);
    vb.decodeArray(in_data, compressedSize, decompressed.data(), block_size);

    // Resize the decompressed vector to fit the actual decompressed data
    decompressed.resize(block_size);

    return decompressed;

}

vector<uint32_t> fastpfor_vb_delta_decompress(const std::vector<uint32_t>& encoded_deltas) {
    FastPForLib::VariableByte vb;

    // Decode deltas using FastPFor's variable byte decoding
    vector<uint32_t> decoded_deltas(encoded_deltas.size() * 2);
    size_t decoded_size;
    vb.decodeArray(encoded_deltas.data(), encoded_deltas.size(), decoded_deltas.data(), decoded_size);
    decoded_deltas.resize(decoded_size);

    // Reconstruct original sequence by adding deltas to previous values
    vector<uint32_t> original_sequence;
    original_sequence.reserve(decoded_deltas.size() + 1);
    uint32_t prev_value = 0; // Assuming the first value is 0, adjust if needed
    for (const auto& delta : decoded_deltas) {
        prev_value += delta;
        original_sequence.push_back(prev_value);
    }

    return original_sequence;
}
