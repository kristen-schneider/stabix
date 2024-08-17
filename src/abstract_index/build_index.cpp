#include "indexers.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <strings.h>
#include <unordered_map>
#include <vector>

void Indexer::build_index(std::string inPath, int blockSize, int queryColumn) {
    auto outPath = this->indexPath;
    std::ifstream file(inPath);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file");
    }

    std::string lineStr;
    if (!std::getline(file, lineStr)) {
        throw std::runtime_error("Missing header line");
    }

    std::unordered_map<float, std::vector<int>> index;

    // 1. Bin the distribution of data

    int lineId = 0;
    bool moreBlocks = true;
    while (moreBlocks) {
        int blockId = lineId / blockSize;
        std::set<int> bins;

        for (int i = 0; i < blockSize; i++) {
            if (!std::getline(file, lineStr)) {
                moreBlocks = false;
                break;
            }

            lineId++;
            auto rowVals = split_string(lineStr, '\t');
            std::string queryVal = rowVals[queryColumn];
            float bin = value_to_bin(queryVal);
            bins.insert(bin);
        }

        for (int bin : bins) {
            index[bin].push_back(blockId);
        }
    }

    // 2. Serialize the index map

    std::ofstream indexFile(outPath);
    std::unordered_map<float, int> binPositions;

    for (auto &entry : index) {
        binPositions[entry.first] = indexFile.tellp();

        for (int i = 0; i < entry.second.size() - 1; i++) {
            indexFile << entry.second[i] << " ";
        }

        indexFile << entry.second[entry.second.size() - 1] << std::endl;
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
