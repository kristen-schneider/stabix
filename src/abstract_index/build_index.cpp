#include "curl_stream.h"
#include "indexers.h"
#include "stabix_except.h"
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <strings.h>
#include <unordered_map>
#include <vector>

void Indexer::build_index(std::string inPath, int queryColumn,
                          vector<float> bins) {
    // sort bins in descending order
    std::sort(bins.begin(), bins.end(), std::greater<float>());
    this->bins = bins;
    std::string outPath = this->indexPath;

    unique_ptr<istream> in_ptr;
    if (is_url(inPath)) {
        in_ptr = make_unique<CurlStream>(inPath);
    } else {
        in_ptr = make_unique<ifstream>(inPath);
        if (!static_cast<ifstream *>(in_ptr.get())->is_open()) {
            throw StabixExcept("Failed to open file: " + inPath);
        }
    }

    istream &inStream = *in_ptr;
    std::string lineStr;
    getline(inStream, lineStr);

    if (!std::getline(inStream, lineStr)) {
        throw StabixExcept("Missing header line");
    }

    std::unordered_map<float, std::unordered_set<int>> index;

    // 1. Bin the distribution of data

    int line_id = 1;

    while (std::getline(inStream, lineStr)) {
        auto row_vals = split_string(lineStr, '\t');
        std::string query_val = row_vals[queryColumn];
        try {
            float bin = value_to_bin(query_val);
            int block_id = this->blockLineMap->line_to_block(line_id);
            index[bin].insert(block_id);
            line_id++;
            if (bin == -HUGE_VALF) {
                continue;
            }
        } catch (StabixExcept &e) {
            line_id++;
            continue;
        }
        //        line_id++;
    }

    // 2. Serialize the index map

    std::ofstream indexFile(outPath);
    std::unordered_map<float, int> binPositions;

    for (auto &entry : index) {
        binPositions[entry.first] = indexFile.tellp();
        auto set = entry.second;

        string lineQueue = "";

        for (auto &block_id : set) {
            indexFile << lineQueue;
            indexFile << block_id;
            lineQueue = " ";
        }

        indexFile << std::endl;
    }

    // 3. Write the footer (map keys)

    indexFile << std::endl;

    int pos0 = indexFile.tellp();

    for (auto &entry : binPositions) {
        std::string key = bin_to_value(entry.first);
        indexFile << key << " " << entry.second << std::endl;
    }

    int footerSize = (int)(indexFile.tellp()) - pos0;
    indexFile << footerSize << std::endl;

    indexFile.close();
}
