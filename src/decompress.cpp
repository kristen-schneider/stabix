#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <sstream>
#include <stdexcept>

#include "utils.h"

using namespace std;

string zlib_decompress(string compressed_string);
vector<uint32_t> fastpfor_vb_decompress(uint32_t* in_data);

/*
 * Decompress a column using the specified codec
 * @param compressed_column: string, the compressed column
 * @param codec: string, the codec used to compress the column
 * @return: string, the decompressed column
 */
string decompress_column(string compressed_column, string codec){
    string decompressed_column_str;
    if (codec == "zlib"){
        decompressed_column_str = zlib_decompress(compressed_column);
        return decompressed_column_str;
    }
    else if (codec == "fpfVB"){
        vector<uint32_t> decompressed_column;
        decompressed_column = fastpfor_vb_decompress((uint32_t*)compressed_column.c_str());
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
 */
vector<uint32_t> fastpfor_vb_decompress(uint32_t* in_data) {
    FastPForLib::VariableByte vb;

    size_t compressedSize = 1000;
    vector<uint32_t> decompressed(compressedSize);
    size_t decompressedSize; // variable to store the number of decompressed values
    vb.decodeArray(in_data, compressedSize, decompressed.data(), decompressedSize);

    // Resize the decompressed vector to fit the actual decompressed data
    decompressed.resize(decompressedSize);

    return decompressed;

}