#include <vector>
#include <map>

#include "blocks.h"
#include "gtest/gtest.h"

using namespace std;

TEST(MakeBlocks, test_tsv){
    string gwas_file = "/Users/krsc0813/CLionProjects/gwas_local/gwas_files/test.tsv";
    int num_columns = 10;
    int block_size = 0;
    char delim = '\t';
    vector<vector<int>> genomic_index;

    // block_size = 5
    block_size = 5;

    vector<vector<vector<string>>> blocks_5 = make_blocks(gwas_file,
                                                          num_columns,
                                                          block_size,
                                                          delim,
                                                          genomic_index);
    ASSERT_EQ(blocks_5.size(), 6);
    ASSERT_EQ(blocks_5[0].size(), num_columns);
    ASSERT_EQ(blocks_5[0][0].size(), block_size);
    ASSERT_EQ(blocks_5[1].size(), num_columns);
    ASSERT_EQ(blocks_5[1][0].size(), block_size);
    ASSERT_EQ(blocks_5[2].size(), num_columns);
    ASSERT_EQ(blocks_5[2][0].size(), block_size);

    // block_size = 10
    block_size = 10;
    vector<vector<vector<string>>> blocks_10 = make_blocks(gwas_file,
                                                        num_columns,
                                                        block_size,
                                                        delim,
                                                        genomic_index);
    ASSERT_EQ(blocks_10.size(), 3);
    ASSERT_EQ(blocks_10[0].size(), num_columns);
    ASSERT_EQ(blocks_10[0][0].size(), block_size);
    ASSERT_EQ(blocks_10[1].size(), num_columns);
    ASSERT_EQ(blocks_10[1][0].size(), block_size);
    ASSERT_EQ(blocks_10[2].size(), num_columns);
    ASSERT_EQ(blocks_10[2][0].size(), block_size);

    // block_size = 15
    block_size = 15;
    vector<vector<vector<string>>> blocks_15 = make_blocks(gwas_file,
                                                           num_columns,
                                                           block_size,
                                                           delim,
                                                           genomic_index);
    ASSERT_EQ(blocks_15.size(), 2);
    ASSERT_EQ(blocks_15[0].size(), num_columns);
    ASSERT_EQ(blocks_15[0][0].size(), block_size);
    ASSERT_EQ(blocks_15[1].size(), num_columns);
    ASSERT_EQ(blocks_15[1][0].size(), block_size);

    // block_size = 20
    block_size = 20;

    vector<vector<vector<string>>> blocks_20 = make_blocks(gwas_file,
                                                           num_columns,
                                                           block_size,
                                                           delim,
                                                           genomic_index);
    ASSERT_EQ(blocks_20.size(), 2);
    ASSERT_EQ(blocks_20[0].size(), num_columns);
    ASSERT_EQ(blocks_20[0][0].size(), block_size);
    ASSERT_EQ(blocks_20[1].size(), num_columns);
    ASSERT_EQ(blocks_20[1][0].size(), 10);
    ASSERT_NE(blocks_20[1][0].size(), block_size);
}

TEST(MakeBlocksMap, test_tsv) {
    string gwas_file = "/Users/krsc0813/CLionProjects/gwas_local/gwas_files/test.tsv";
    int num_columns = 10;
    char delim = '\t';
  
    vector<vector<int>> genomic_index;
    map<int, vector<uint32_t>> chrm_block_bp_ends = {
            {1, {5000,8999,12999}},
            {2, {5000,8999,12999}},
            {3, {5000,8999,12999}}};

    vector<vector<vector<string>>> blocks_map = make_blocks_map(gwas_file,
                                                            num_columns,
                                                            chrm_block_bp_ends,
                                                            delim,
                                                            genomic_index);
    ASSERT_EQ(blocks_map.size(), 12);
    ASSERT_EQ(blocks_map[0].size(), num_columns);
    ASSERT_EQ(blocks_map[0][0].size(), 5);
    ASSERT_EQ(blocks_map[1].size(), num_columns);
    ASSERT_EQ(blocks_map[1][0].size(), 1);
    ASSERT_EQ(blocks_map[2].size(), num_columns);
    ASSERT_EQ(blocks_map[2][0].size(), 3);
    ASSERT_EQ(blocks_map[3].size(), num_columns);
    ASSERT_EQ(blocks_map[3][0].size(), 1);
    ASSERT_EQ(blocks_map[4].size(), num_columns);
    ASSERT_EQ(blocks_map[4][0].size(), 5);
    ASSERT_EQ(blocks_map[5].size(), num_columns);
    ASSERT_EQ(blocks_map[5][0].size(), 1);
    ASSERT_EQ(blocks_map[6].size(), num_columns);
    ASSERT_EQ(blocks_map[6][0].size(), 3);
    ASSERT_EQ(blocks_map[7].size(), num_columns);
    ASSERT_EQ(blocks_map[7][0].size(), 1);
    ASSERT_EQ(blocks_map[8].size(), num_columns);
    ASSERT_EQ(blocks_map[8][0].size(), 5);
    ASSERT_EQ(blocks_map[9].size(), num_columns);
    ASSERT_EQ(blocks_map[9][0].size(), 1);
    ASSERT_EQ(blocks_map[10].size(), num_columns);
    ASSERT_EQ(blocks_map[10][0].size(), 3);
    ASSERT_EQ(blocks_map[11].size(), num_columns);
    ASSERT_EQ(blocks_map[11][0].size(), 1);
}

TEST(GetByteStartOfBlocks, test_tsv) {

    int compressed_header_size = 6;
    vector<string> block_end_bytes = {"10", "20", "30", "40", "50", "60"};
    // chrm, bp, line, byte
    vector<vector<int>> genomic_index = {
            {1,100,1,0},
            {1,500,6,0},
            {2,100,11,0},
            {2,500,16,0},
            {3,100,21,0},
            {3,500,26,0}
    };

    get_byte_start_of_blocks(
            compressed_header_size,
            block_end_bytes,
            genomic_index);

    ASSERT_EQ(genomic_index.size(), 6);
    ASSERT_EQ(genomic_index[0], vector<int>({1,100,1,10}));
    ASSERT_EQ(genomic_index[1], vector<int>({1,500,6,20}));
    ASSERT_EQ(genomic_index[2], vector<int>({2,100,11,30}));
    ASSERT_EQ(genomic_index[3], vector<int>({2,500,16,40}));
    ASSERT_EQ(genomic_index[4], vector<int>({3,100,21,50}));
    ASSERT_EQ(genomic_index[5], vector<int>({3,500,26,60}));
}

TEST(GetBlockHeader, test_tsv){
    vector<string> compressed_block = {"aaa", "bbb", "ccc", "ddd", "eee", "fff"};
    vector<string> block_end_bytes = get_block_header(compressed_block);

    ASSERT_EQ(block_end_bytes.size(), 6);
    ASSERT_EQ(block_end_bytes[0], "3");
    ASSERT_EQ(block_end_bytes[1], "6");
    ASSERT_EQ(block_end_bytes[2], "9");
    ASSERT_EQ(block_end_bytes[3], "12");
    ASSERT_EQ(block_end_bytes[4], "15");
    ASSERT_EQ(block_end_bytes[5], "18");
}

TEST(GetBlockLength, test_tsv){
    vector<string> compressed_block = {"aaa", "bbb", "ccc", "ddd", "eee", "fff"};
    int block_lengths = get_block_length(compressed_block);

    ASSERT_EQ(block_lengths, 18);
}

TEST(CompressBlock, test_tsv){
    vector<string> codecs_list = {"zlib", "zlib", "zlib", "zlib", "zlib", "zlib"};
    vector<vector<string>> block = {{"aaa", "bbb", "ccc", "ddd", "eee", "fff"},
                                    {"aaa", "bbb", "ccc", "ddd", "eee", "fff"},
                                    {"aaa", "bbb", "ccc", "ddd", "eee", "fff"},
                                    {"aaa", "bbb", "ccc", "ddd", "eee", "fff"},
                                    {"aaa", "bbb", "ccc", "ddd", "eee", "fff"},
                                    {"aaa", "bbb", "ccc", "ddd", "eee", "fff"}};

    vector<string> compressed_block = compress_block(block, codecs_list);
    ASSERT_EQ(compressed_block.size(), 6);
}