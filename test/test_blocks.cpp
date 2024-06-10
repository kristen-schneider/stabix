#include <vector>
#include <map>

#include "blocks.h"
#include "googletest/googletest/include/gtest/gtest.h"

using namespace std;

TEST(MakeBlocksMap, test) {
    string gwas_file = "/Users/krsc0813/CLionProjects/gwas_local/gwas_files/test.tsv";
    int num_columns = 10;
    map<int, vector<uint32_t>> chrm_block_bp_ends = {
            {1, {5000, 9000, 13000}},
            {2, {5000, 9000, 13000}},
            {3, {5000, 9000, 13000}}
    };
    char delim = '\t';

    vector<vector<vector<string>>> blocks = make_blocks_map(gwas_file,
                                                            num_columns,
                                                            chrm_block_bp_ends,
                                                            delim);
    ASSERT_EQ(blocks.size(), 12);
    ASSERT_EQ(blocks[0].size(), 10);
    ASSERT_EQ(blocks[0][0].size(), 5);
    ASSERT_EQ(blocks[1][0].size(), 1);
    ASSERT_EQ(blocks[2][0].size(), 3);
    ASSERT_EQ(blocks[3][0].size(), 1);

}

//TEST(MakeBlocksMap, test) {
//    string gwas_file = "/Users/krsc0813/CLionProjects/gwas_local/data/test.tsv";
//    int num_columns = 10;
//    map<int, vector<uint32_t>> chrm_block_bp_ends = {
//            {1, {5000, 9000, 13000}},
//            {2, {5000, 9000, 13000}},
//            {3, {5000, 9000, 13000}}
//    };
//    char delim = '\t';
//
//    vector<vector<vector<string>>> blocks = make_blocks_map(gwas_file,
//                                                            num_columns,
//                                                            chrm_block_bp_ends,
//                                                            delim);
//    ASSERT_EQ(blocks.size(), 12);
//    ASSERT_EQ(blocks[0].size(), 10);
//    ASSERT_EQ(blocks[0][0].size(), 5);
//    ASSERT_EQ(blocks[1][0].size(), 1);
//    ASSERT_EQ(blocks[2][0].size(), 3);
//    ASSERT_EQ(blocks[3][0].size(), 1);
//
//}