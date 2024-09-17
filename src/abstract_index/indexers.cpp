#include "indexers.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

float PValIndexer::nearest_bin(float value) {
    // TODO: at query time, these bins should be provided from the index file
    for (float bin : this->bins) {
        if (value >= bin) {
            return bin;
        }
    }

    return -HUGE_VALF;
}

Indexer::Indexer(std::string indexPath, BlockLineMap *map) {
    this->indexPath = indexPath;
    this->blockLineMap = map;
}

PValIndexer::PValIndexer(std::string index_path, BlockLineMap *map,
                         vector<float> bins)
    : Indexer(index_path, map) {
    // sort bins in descending order
    std::sort(bins.begin(), bins.end(), std::greater<float>());
    this->bins = bins;
}

bool badFloatSemaphore = false;
float PValIndexer::value_to_bin(std::string line) {
    char *end;
    errno = 0;
    float value = std::strtof(line.c_str(), &end);

    if (end == line.c_str()) {
        //        throw std::runtime_error("Invalid float format.");
        // skip NA values
        return -HUGE_VALF;
    }

    if (errno == ERANGE && !badFloatSemaphore) {
        std::cerr << "Warning: rounding some values because they cannot fit in "
                     "float, such as: "
                  << line << std::endl;
        badFloatSemaphore = true;
    }

    return this->nearest_bin(value);
}

unordered_set<int> PValIndexer::compare_query(float threshold,
                                              ComparisonType compType) {

    float pivotBin = this->nearest_bin(threshold);

    switch (compType) {
    case ComparisonType::LessThan:
    case ComparisonType::LessThanOrEqual:
        return query_index([pivotBin](float val) { return val <= pivotBin; });
    case ComparisonType::Equal:
        return query_index([pivotBin](float val) { return val == pivotBin; });
    case ComparisonType::GreaterThan:
    case ComparisonType::GreaterThanOrEqual:
        return query_index([pivotBin](float val) { return val >= pivotBin; });
    }

    // unreachable
    throw std::runtime_error("Invalid comparison type.");
}

std::string precise_to_string(double value, int precision = 7) {
    // TODO: is precision 7 enough for all cases?
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(precision) << value;
    return oss.str();
}

std::string PValIndexer::bin_to_value(float bin) {
    return precise_to_string(bin);
}
