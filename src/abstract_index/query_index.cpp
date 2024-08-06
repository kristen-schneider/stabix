#include "utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <strings.h>
#include <vector>

// TODO: need a way to parse these values in an accessible way other than
// passing raw strings
bool block_predicate(float blockVal) { return blockVal < .3; }

bool row_predicate(float rowVal) { return true; }

/*
 * Find block IDs to decompress, relevant for query based
 * on the block predicate function.
 */
std::vector<int> query_blocks(std::string indexPath) {
    std::ifstream indexFile(indexPath, std::ios::ate);

    if (!indexFile.is_open()) {
        // TODO: throwing generic exceptions is not an ideal error handling
        // system
        throw std::runtime_error("Cannot open index file.");
    }

    // 1. Get length of footer

    char item;
    do {
        if (indexFile.tellg() < 2) { // Check before seeking
            throw std::runtime_error(
                "Footer is misformatted. Missing footer size.");
        }

        indexFile.seekg(-2, std::ios::cur);
        indexFile.get(item);
    } while (item != '\n');

    int footerEnd = indexFile.tellg();
    std::string footerSizeStr;
    getline(indexFile, footerSizeStr);
    int footerSize = std::stoi(footerSizeStr);

    // 2. Read footer for relevant bin entries

    auto searchBins = std::vector<int>(); // output
    indexFile.seekg(footerEnd - footerSize);
    std::string line;

    while ((int)indexFile.tellg() < footerEnd) {
        if (indexFile.tellg() == -1) {
            break;
        }

        getline(indexFile, line);
        auto delimPos = line.find(' ');
        float binValue = std::stof(line.substr(0, delimPos));

        if (block_predicate(binValue)) {
            int entryLine = std::stoi(line.substr(delimPos + 1));
            searchBins.push_back(entryLine);
        }
    }

    // 3. Aggregate block IDs from blocks in the file

    std::sort(searchBins.begin(), searchBins.end());
    auto blockIDs = std::vector<int>(); // output

    for (int i = 0; i < searchBins.size(); i++) {
        indexFile.seekg(searchBins[i]);
        std::string value;
        getline(indexFile, value);
        auto idExpressions = split_string(value, ' ');

        for (auto idExpression : idExpressions) {
            int idParsed = std::stoi(idExpression);
            blockIDs.push_back(idParsed);
        }
    }

    return blockIDs;
}

/*
 * Helper function to parse a row in raw (string) form
 * and determine if it should be contained in a query
 * based on the row predicate function.
 */
bool take_row(std::string row, int colId) {
    auto split = split_string(row, '\t');
    auto relevant = split[colId];
    float value = std::stof(relevant);
    return row_predicate(value);
}

int main() {
    query_blocks("../gwas_files/test_idx/p_value.idx");
    // query_blocks("/Users/simon/jiba");

    return 0;
}
