#include <iostream>

#include "index.h"
#include "decompress.h"
#include "googletest/googletest/include/gtest/gtest.h"

using namespace std;


TEST(TestIndex, Test1) {

    // chrm, bp, byte, block_idx
    map<int, map<int, tuple<int, int>>> index_file_map = {
            {1, {
                          {100,  make_tuple(10, 0)},
                          {500,  make_tuple(20, 1)},
                          {1000, make_tuple(30, 2)}
                  }},
            {2, {
                          {1500, make_tuple(40, 3)},
                          {2000, make_tuple(50, 4)},
                          {2500, make_tuple(60, 5)}
                  }},
            {3, {
                          {3000, make_tuple(70, 6)},
                          {3500, make_tuple(80, 7)},
                          {4000, make_tuple(90, 8)}
                  }}
    };

    map<int, int> index_block_map = {
            {0, 10},
            {1, 20},
            {2, 30},
            {3, 40},
            {4, 50},
            {5, 60},
            {6, 70},
            {7, 80},
            {8, 90}

    };

//    int a = get_block_idx(1, 100, index_file_map);
//    ASSERT_EQ(a, 0);

    ASSERT_EQ(get_block_idx(1, 100, index_file_map), 0);
    ASSERT_EQ(get_block_idx(1, 500, index_file_map), 1);
    ASSERT_EQ(get_block_idx(1, 1000, index_file_map), 2);
    ASSERT_EQ(get_block_idx(2, 1500, index_file_map), 3);
    ASSERT_EQ(get_block_idx(2, 2000, index_file_map), 4);
    ASSERT_EQ(get_block_idx(2, 2500, index_file_map), 5);
    ASSERT_EQ(get_block_idx(3, 3000, index_file_map), 6);
    ASSERT_EQ(get_block_idx(3, 3500, index_file_map), 7);
    ASSERT_EQ(get_block_idx(3, 4000, index_file_map), 8);

    ASSERT_EQ(get_start_byte(0, index_block_map), 10);
    ASSERT_EQ(get_start_byte(1, index_block_map), 20);
    ASSERT_EQ(get_start_byte(2, index_block_map), 30);
    ASSERT_EQ(get_start_byte(3, index_block_map), 40);
    ASSERT_EQ(get_start_byte(4, index_block_map), 50);
    ASSERT_EQ(get_start_byte(5, index_block_map), 60);
    ASSERT_EQ(get_start_byte(6, index_block_map), 70);
    ASSERT_EQ(get_start_byte(7, index_block_map), 80);
    ASSERT_EQ(get_start_byte(8, index_block_map), 90);

    vector<string> query_list_simple = {"1:100-101", "1:500-501", "1:1000-1001",
                                        "2:1500-1501", "2:2000-2001", "2:2500-2501",
                                        "3:3000-3001", "3:3500-3501", "3:4000-4001"};
    vector<string> query_list_overlap = {"1:100-501", "1:150-1001", "1:550-1001",
                                         "2:1500-2050", "2:1500-3001", "2:2000-2501",
                                         "3:3000-3501", "3:3500-4001", "3:3000-4001"};

    vector<tuple<int, int>> all_query_block_indexes_simple = get_start_end_block_idx(query_list_simple,
                                                                              index_file_map,
                                                                              index_block_map);
    vector<tuple<int, int>> all_query_block_indexes_overlap = get_start_end_block_idx(query_list_overlap,
                                                                                      index_file_map,
                                                                                      index_block_map);
    ASSERT_EQ(get<0>(all_query_block_indexes_simple[0]), 0);
    ASSERT_EQ(get<1>(all_query_block_indexes_simple[0]), 0);
    ASSERT_EQ(get<0>(all_query_block_indexes_simple[1]), 1);
    ASSERT_EQ(get<1>(all_query_block_indexes_simple[1]), 1);
    ASSERT_EQ(get<0>(all_query_block_indexes_simple[2]), 2);
    ASSERT_EQ(get<1>(all_query_block_indexes_simple[2]), 2);
    ASSERT_EQ(get<0>(all_query_block_indexes_simple[3]), 3);
    ASSERT_EQ(get<1>(all_query_block_indexes_simple[3]), 3);

    ASSERT_EQ(get<0>(all_query_block_indexes_overlap[0]), 0);
    ASSERT_EQ(get<1>(all_query_block_indexes_overlap[0]), 1);
    ASSERT_EQ(get<0>(all_query_block_indexes_overlap[1]), 0);
    ASSERT_EQ(get<1>(all_query_block_indexes_overlap[1]), 2);
    ASSERT_EQ(get<0>(all_query_block_indexes_overlap[2]), 1);
    ASSERT_EQ(get<1>(all_query_block_indexes_overlap[2]), 2);
    ASSERT_EQ(get<0>(all_query_block_indexes_overlap[3]), 3);
    ASSERT_EQ(get<1>(all_query_block_indexes_overlap[3]), 4);
}



