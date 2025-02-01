#include "indexers.h"
#include "stabix_except.h"
#include <fstream>

int Indexer::footer_length() {
    auto index_file = try_open_ate(this->indexPath);

    char item;
    do {
        if (index_file.tellg() < 2) { // Check before seeking
            throw StabixExcept("Footer is misformatted. Missing footer size.");
        }

        index_file.seekg(-2, std::ios::cur);
        index_file.get(item);
    } while (item != '\n');

    std::string footer_size_str;
    getline(index_file, footer_size_str);
    int footer_size = std::stoi(footer_size_str);

    index_file.close();
    return footer_size;
}

int Indexer::get_footer_end() {
    /*
     * Get the end position of the footer in the index file,
     * excluding the final number at the end which indicates
     * the size of the footer.
     */

    auto index_file = try_open_ate(this->indexPath);

    char item;
    do {
        if (index_file.tellg() < 2) { // Check before seeking
            throw StabixExcept("Footer is misformatted. Missing footer size.");
        }

        index_file.seekg(-2, std::ios::cur);
        index_file.get(item);
    } while (item != '\n');

    int footer_end = index_file.tellg();
    index_file.close();
    return footer_end;
}

std::unordered_map<float, int> Indexer::read_bin_pos() {
    /*
     * Read the bin keys from the index file.
     * Returns a map of bin keys (floats) to their byte positions in the index
     * file.
     */

    const int footer_size = this->footer_length();
    const int footer_end = this->get_footer_end();
    const int footer_start = footer_end - footer_size;

    auto index_file = try_open_ate(this->indexPath);

    auto bins = std::unordered_map<float, int>(); // output
    index_file.seekg(footer_start);
    std::string line;

    while ((int)index_file.tellg() < footer_end) {
        if (index_file.tellg() == -1) {
            break;
        }

        getline(index_file, line);
        auto delim_pos = line.find(' ');
        float bin_value = std::stof(line.substr(0, delim_pos));
        int entry_line = std::stoi(line.substr(delim_pos + 1));
        bins[bin_value] = entry_line;
    }

    index_file.close();
    return bins;
}

void Indexer::read_index() {
    /*
     * Initialization, for query time.
     */

    this->bin_pos = this->read_bin_pos();

    // sorted bin keys

    auto bins = std::vector<float>();

    for (auto &entry : this->bin_pos) {
            bins.push_back(entry.first);
    }

    std::sort(bins.begin(), bins.end(), std::greater<float>());
    this->bins = bins;
}