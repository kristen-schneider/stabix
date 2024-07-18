#include "utils.h"
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <strings.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

int value_to_bin(float queryVal) { return int(queryVal * 10); }

float bin_to_value(int bin) { return float(bin) / 10; }

int main() {
    string path = "../gwas_files/test.tsv";
    ifstream file(path);

    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }

    string lineStr;
    if (!getline(file, lineStr)) {
        cout << "Missing header line" << endl;
        return 1;
    }

    int blockSize = 3;
    int queryCol = 9;

    // TODO: ordered map is okay for small number of bins
    unordered_map<int, vector<int>> index;

    int lineId = 0;
    bool moreBlocks = true;
    while (moreBlocks) {
        int blockId = lineId / blockSize;
        set<int> bins;

        for (int i = 0; i < blockSize; i++) {
            if (!getline(file, lineStr)) {
                moreBlocks = false;
                break;
            }

            lineId++;
            auto rowVals = split_string(lineStr, '\t');
            float queryVal = stof(rowVals[queryCol]);
            int bin = value_to_bin(queryVal);
            bins.insert(bin);
        }

        for (int bin : bins) {
            index[bin].push_back(blockId);
        }
    }

    for (auto &entry : index) {
        cout << entry.first << ": ";
        for (int blockId : entry.second) {
            cout << blockId << " ";
        }
        cout << endl;
    }

    ofstream indexFile("simon.index");
    unordered_map<int, int> binPositions;

    for (auto &entry : index) {
        binPositions[entry.first] = indexFile.tellp();

        for (int i = 0; i < entry.second.size() - 1; i++) {
            indexFile << entry.second[i] << " ";
        }

        indexFile << entry.second[entry.second.size() - 1] << endl;
    }

    indexFile << endl;

    for (auto &entry : binPositions) {
        cout << entry.first << ": " << entry.second << endl;
    }

    int pos0 = indexFile.tellp();

    for (auto &entry : binPositions) {
        float key = bin_to_value(entry.first);
        indexFile << key << " " << entry.second << endl;
    }

    int footerSize = (int)(indexFile.tellp()) - pos0;
    indexFile << footerSize << endl;

    indexFile.close();
    return 0;
}
