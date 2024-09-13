#include <vector>
#include <map>

#include "blocks.h"
#include "gtest/gtest.h"

using namespace std;

//TEST(MakeBlocks, test_tsv){
//    string gwas_file = "/Users/krsc0813/CLionProjects/gwas_local/gwas_files/test.tsv";
//    int num_columns = 10;
//    int block_size = 0;
//    char delim = '\t';
//    vector<vector<int>> genomic_index;
//
//    // block_size = 5
//    block_size = 5;
//
//    vector<vector<vector<string>>> blocks_5 = make_blocks(gwas_file,
//                                                          num_columns,
//                                                          block_size,
//                                                          delim,
//                                                          genomic_index);
//    ASSERT_EQ(blocks_5.size(), 6);
//    ASSERT_EQ(blocks_5[0].size(), num_columns);
//    ASSERT_EQ(blocks_5[0][0].size(), block_size);
//    ASSERT_EQ(blocks_5[1].size(), num_columns);
//    ASSERT_EQ(blocks_5[1][0].size(), block_size);
//    ASSERT_EQ(blocks_5[2].size(), num_columns);
//    ASSERT_EQ(blocks_5[2][0].size(), block_size);
//
//    // block_size = 10
//    block_size = 10;
//    vector<vector<vector<string>>> blocks_10 = make_blocks(gwas_file,
//                                                        num_columns,
//                                                        block_size,
//                                                        delim,
//                                                        genomic_index);
//    ASSERT_EQ(blocks_10.size(), 3);
//    ASSERT_EQ(blocks_10[0].size(), num_columns);
//    ASSERT_EQ(blocks_10[0][0].size(), block_size);
//    ASSERT_EQ(blocks_10[1].size(), num_columns);
//    ASSERT_EQ(blocks_10[1][0].size(), block_size);
//    ASSERT_EQ(blocks_10[2].size(), num_columns);
//    ASSERT_EQ(blocks_10[2][0].size(), block_size);
//
//    // block_size = 15
//    block_size = 15;
//    vector<vector<vector<string>>> blocks_15 = make_blocks(gwas_file,
//                                                           num_columns,
//                                                           block_size,
//                                                           delim,
//                                                           genomic_index);
//    ASSERT_EQ(blocks_15.size(), 2);
//    ASSERT_EQ(blocks_15[0].size(), num_columns);
//    ASSERT_EQ(blocks_15[0][0].size(), block_size);
//    ASSERT_EQ(blocks_15[1].size(), num_columns);
//    ASSERT_EQ(blocks_15[1][0].size(), block_size);
//
//    // block_size = 20
//    block_size = 20;
//
//    vector<vector<vector<string>>> blocks_20 = make_blocks(gwas_file,
//                                                           num_columns,
//                                                           block_size,
//                                                           delim,
//                                                           genomic_index);
//    ASSERT_EQ(blocks_20.size(), 2);
//    ASSERT_EQ(blocks_20[0].size(), num_columns);
//    ASSERT_EQ(blocks_20[0][0].size(), block_size);
//    ASSERT_EQ(blocks_20[1].size(), num_columns);
//    ASSERT_EQ(blocks_20[1][0].size(), 10);
//    ASSERT_NE(blocks_20[1][0].size(), block_size);
//}
//
//TEST(MakeBlocksMap, test_tsv) {
//    string gwas_file = "/Users/krsc0813/CLionProjects/gwas_local/gwas_files/test.tsv";
//    int num_columns = 10;
//    char delim = '\t';
//
//    vector<vector<int>> genomic_index;map<int, vector<uint32_t>> chrm_block_bp_ends = {
//            {1, {100, 1, 10}},
//            {2, {100, 11, 20}},
//            {3, {100, 21, 30}}
//    };
//
//    vector<vector<vector<string>>> blocks = make_blocks_map(gwas_file,
//                                                            num_columns,
//                                                            chrm_block_bp_ends,
//                                                            delim,
//                                                            genomic_index);
//    ASSERT_EQ(blocks.size(), 12);
//    ASSERT_EQ(blocks[0].size(), 10);
//    ASSERT_EQ(blocks[0][0].size(), 5);
//    ASSERT_EQ(blocks[1][0].size(), 1);
//    ASSERT_EQ(blocks[2][0].size(), 3);
//    ASSERT_EQ(blocks[3][0].size(), 1);
//}

//TEST(GetByteStartOfBlocks, test_tsv) {
//
//    int compressed_header_size = 6;
//    vector<string> block_end_bytes = {"10", "20", "30"};
//    vector<vector<int>> genomic_index;map<int, vector<uint32_t>> chrm_block_bp_ends = {
//            {1, {100, 1, 0}},
//            {2, {100, 11, 0}},
//            {3, {100, 21, 0}}
//    };
//
//    get_byte_start_of_blocks(
//            compressed_header_size,
//            block_end_bytes,
//            genomic_index);
//
//    ASSERT_EQ(genomic_index.size(), 3);
//    ASSERT_EQ(genomic_index[1], vector<int>({100, 1, 10}));
//}