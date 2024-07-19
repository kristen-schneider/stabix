#include "utils.h"
#include <fstream>
#include <iostream>
#include <set>
#include <strings.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Indexer {
  public:
    virtual int value_to_bin(string queryVal) = 0;
    virtual string bin_to_value(int bin) = 0;
    virtual ~Indexer() = default;

    void build_index(string inPath, string outPath) {
        ifstream file(inPath);

        if (!file.is_open()) {
            throw runtime_error("Error opening file");
        }

        string lineStr;
        if (!getline(file, lineStr)) {
            throw runtime_error("Missing header line");
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
                string queryVal = rowVals[queryCol];
                int bin = value_to_bin(queryVal);
                bins.insert(bin);
            }

            for (int bin : bins) {
                index[bin].push_back(blockId);
            }
        }

        // for (auto &entry : index) {
        //     cout << entry.first << ": ";
        //     for (int blockId : entry.second) {
        //         cout << blockId << " ";
        //     }
        //     cout << endl;
        // }

        ofstream indexFile(outPath);
        unordered_map<int, int> binPositions;

        for (auto &entry : index) {
            binPositions[entry.first] = indexFile.tellp();

            for (int i = 0; i < entry.second.size() - 1; i++) {
                indexFile << entry.second[i] << " ";
            }

            indexFile << entry.second[entry.second.size() - 1] << endl;
        }

        indexFile << endl;

        // for (auto &entry : binPositions) {
        //     cout << entry.first << ": " << entry.second << endl;
        // }

        int pos0 = indexFile.tellp();

        for (auto &entry : binPositions) {
            string key = bin_to_value(entry.first);
            indexFile << key << " " << entry.second << endl;
        }

        int footerSize = (int)(indexFile.tellp()) - pos0;
        indexFile << footerSize << endl;

        indexFile.close();
        return;
    }
};

class PValIndexer : public Indexer {
    int value_to_bin(string queryVal) { return int(stof(queryVal) * 10); }
    string bin_to_value(int bin) { return to_string(float(bin) / 10); }
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw runtime_error("1 argument required: config_path");
    }

    string configPath = argv[1];
    cout << "Reading config options from: " << configPath << endl;
    map<string, string> configOptions = read_config_file(configPath);
    add_default_config_options(configOptions);
    string gwasPath = configOptions["gwas_file"];
    // TODO: block_size via map file not implemented
    int blockSize = stoi(configOptions["block_size"]);
    cout << "...Success" << endl;

    cout << "Preparring indices..." << endl;

    string gwasColumnLine;
    ifstream gwasFile(gwasPath);
    if (!gwasFile.is_open() || !getline(gwasFile, gwasColumnLine)) {
        throw runtime_error("Error opening GWAS file");
    }

    auto gwasColumns = split_string(gwasColumnLine, '\t');
    gwasFile.close();

    cout << "...Successfully read GWAS file columns headers." << endl;
    cout << "Building indices..." << endl;

    // TODO: need to config indexing algorithms
    Indexer *indexers[] = {NULL, NULL, NULL, NULL, NULL,
                           NULL, NULL, NULL, NULL, new PValIndexer()};

    // TODO: parallelize
    for (int i = 0; i < gwasColumns.size(); i++) {
        Indexer *indexer = indexers[i];

        if (indexer == NULL) {
            cout << "...Skipping column: " << gwasColumns[i] << endl;
            continue;
        }

        string columnName = gwasColumns[i];
        cout << "...Building index for column: " << columnName << endl;

        // big operation
        string outPath = gwasPath + "." + columnName + ".idx";
        indexer->build_index(gwasPath, outPath);
        cout << "......Success." << endl;

        delete indexer;
    }

    cout << "Complete." << endl;
    return 0;
}
