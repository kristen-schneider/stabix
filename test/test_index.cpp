#include <iostream>

#include "index.h"
#include "decompress.h"
#include "gtest/gtest.h"

using namespace std;

TEST(GetBlockIdx, test_tsv){
    map<int, map<int, vector<int>>> genomic_index_info_by_location = {
            {1, {
                    {100, {0, 1, 10}},
                    {500, {1, 6, 20}},
                    {900, {2, 11, 30}}
            }},
            {2, {
                    {100, {3, 16, 40}},
                    {500, {4, 21, 50}},
                    {900, {5, 26, 60}}
            }},
            {3, {
                    {100, {6, 31, 70}},
                    {500, {7, 36, 80}},
                    {900, {8, 41, 80}}
            }}
    };

    ASSERT_EQ(get_block_idx(1, 100, genomic_index_info_by_location), 0);
    ASSERT_EQ(get_block_idx(1, 500, genomic_index_info_by_location), 1);
    ASSERT_EQ(get_block_idx(1, 900, genomic_index_info_by_location), 2);
}

TEST (GetStartByte, test_tsv){
    map<int, vector<int>> index_block_map = {
            {0, {1, 10}},
            {1, {6, 20}},
            {2, {11, 30}}
    };

    ASSERT_EQ(get_start_byte(0, index_block_map), 10);
    ASSERT_EQ(get_start_byte(1, index_block_map), 20);
    ASSERT_EQ(get_start_byte(2, index_block_map), 30);
}

TEST(GetStartEndBlockIdx, test_tsv){

    vector<string> query_list {"1:100-700", "2:100-700", "3:100-700"};
    // chrm -> bp -> block_idx, line_start, byte_offset
    map<int, map<int, vector<int>>> genomic_index_info_by_location = {
            {1, {
                    {100, {0, 1, 10}},
                    {500, {1, 6, 20}},
                    {900, {2, 11, 30}}
            }},
            {2, {
                    {100, {3, 16, 40}},
                    {500, {4, 21, 50}},
                    {900, {5, 26, 60}}
            }},
            {3, {
                    {100, {6, 31, 70}},
                    {500, {7, 36, 80}},
                    {900, {8, 41, 80}}
            }}
    };

    vector<tuple<int, int>> start_end_block_idx = get_start_end_block_idx(
            query_list,
            genomic_index_info_by_location);

    ASSERT_EQ(start_end_block_idx.size(), 3);
    ASSERT_EQ(start_end_block_idx[0], make_tuple(0, 1));
    ASSERT_EQ(start_end_block_idx[1], make_tuple(3, 4));
    ASSERT_EQ(start_end_block_idx[2], make_tuple(6, 7));

}

