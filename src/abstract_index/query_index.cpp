#include "indexers.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <strings.h>
#include <unordered_set>
#include <vector>
#include "stabix_except.h"

/*
 * Find block IDs to decompress, relevant for query based
 * on the block predicate function.
 */
std::unordered_set<int>
Indexer::query_index(std::function<bool(float)> predicate) {
    // Filter bins based on predicate

    auto flagged_bins = std::vector<int>();

    for (auto &entry : this->read_bin_pos()) {
        float bin = entry.first;
        if (predicate(bin)) {
            flagged_bins.push_back(entry.second);
        }
    }

    // 3. Aggregate block IDs from blocks in the file

    std::sort(flagged_bins.begin(), flagged_bins.end());
    auto block_ids = std::unordered_set<int>(); // output
    auto index_file = try_open_ate(this->indexPath);

    for (int i = 0; i < flagged_bins.size(); i++) {
        index_file.seekg(flagged_bins[i]);
        std::string value;
        getline(index_file, value);
        auto id_expressions = split_string(value, ' ');

        for (auto exp : id_expressions) {
            int id = std::stoi(exp);
            block_ids.insert(id);
        }
    }

    return block_ids;
}

/*
 * Helper function to parse a row in raw (string) form
 * and determine if it should be contained in a query
 * based on the row predicate function.
 */
// bool take_row(std::string row, int colId, std::function<bool(int)> predicate)
// {
//     auto split = split_string(row, '\t');
//     auto relevant = split[colId];
//     float value = std::stof(relevant);
//     return predicate(value);
// }
