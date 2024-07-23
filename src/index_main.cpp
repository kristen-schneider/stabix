#include "indexers.h"
#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw runtime_error("1 argument required: config_path");
    }

    string configPath = argv[1];
    cout << "Reading config options from: " << configPath << endl;
    map<string, string> configOptions = read_config_file(configPath);
    add_default_config_options(configOptions);
    auto gwasPath = fs::path(configOptions["gwas_file"]);
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

    auto outDir = gwasPath.parent_path() / (gwasPath.stem().string() + "_idx");

    for (int i = 0; i < gwasColumns.size(); i++) {
        // TODO: parallelize
        Indexer *indexer = indexers[i];

        if (indexer == NULL) {
            cout << "...Skipping column: " << gwasColumns[i] << endl;
            continue;
        }

        string columnName = gwasColumns[i];
        cout << "...Building index for column: " << columnName << endl;

        // big operation
        auto outPath = outDir / (columnName + ".idx");
        fs::create_directories(outPath.parent_path());
        indexer->build_index(gwasPath, outPath.string());
        cout << "......Success." << endl;

        delete indexer;
    }

    cout << "Complete." << endl;
    return 0;
}
