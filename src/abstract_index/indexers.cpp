#include "indexers.h"
#include <algorithm>
#include <regex>
#include <iomanip>
#include <sstream>
#include "stabix_except.h"

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

PValIndexer::PValIndexer(std::string index_path, BlockLineMap *map)
    : Indexer(index_path, map) {
}

bool badFloatSemaphore = false;
float PValIndexer::value_to_bin(std::string line) {
    char *end;
    errno = 0;
    // handle NA values by putting them in top bin
    if (line == "NA") {
//        // return Inf
//        return HUGE_VALF;
        throw StabixExcept("Invalid float format.");
    }
    float value = std::strtof(line.c_str(), &end);

    // TODO: remove this check??
    // putting NA values in -INF bin lumps them in with anything below the lowest bin;
    // which is likely to be of interest to the user
    if (end == line.c_str()) {
        //        throw StabixExcept("Invalid float format.");
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
                                              ComparisonType comp_type) {

    float pivot_bin = this->nearest_bin(threshold);

    switch (comp_type) {
    case ComparisonType::LessThan:
    case ComparisonType::LessThanOrEqual:
        return query_index([pivot_bin](float val) { return val <= pivot_bin; });
    case ComparisonType::Equal:
        return query_index([pivot_bin](float val) { return val == pivot_bin; });
    case ComparisonType::GreaterThan:
    case ComparisonType::GreaterThanOrEqual:
        return query_index([pivot_bin](float val) { return val >= pivot_bin; });
    }

    // unreachable
    throw StabixExcept("Invalid comparison type.");
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


/*
 * compare to values
 */
bool compare_values(
        string config_query,
        float value_to_compare){
    bool compare_result;
    float query_val;
    ComparisonType op;

    // if value_to_compare == -HUGE_VALF or HUGE_VALF, return false ((NA))
    if (value_to_compare == -HUGE_VALF || value_to_compare == HUGE_VALF) {
        return false;
    }

    regex re("(>|<)(=?)\\s*(\\d*\\.?.*)");
    smatch matches;

    // parse the query
    if (regex_search(config_query, matches, re)) {
        if (matches[1].str() == "<") {
            op = matches[2].str() == "=" ? ComparisonType::LessThanOrEqual
                                         : ComparisonType::LessThan;
        } else {
            op = matches[2].str() == "=" ? ComparisonType::GreaterThanOrEqual
                                         : ComparisonType::GreaterThan;
        }

        string val_exp = matches[3].str();

        try {
            query_val = stof(val_exp);
        } catch (const invalid_argument &e) {
            throw invalid_argument("Value must be a float: " + val_exp);
        }
    } else {
        throw invalid_argument("Could not parse \"" + config_query + "\". Expected something like \"<= 0.3\".");
    }

    // compare the values
    switch (op) {
        case ComparisonType::LessThan:
            compare_result = value_to_compare < query_val;
            break;
        case ComparisonType::LessThanOrEqual:
            compare_result = value_to_compare <= query_val;
            break;
        case ComparisonType::Equal:
            compare_result = value_to_compare == query_val;
            break;
        case ComparisonType::GreaterThan:
            compare_result = value_to_compare > query_val;
            break;
        case ComparisonType::GreaterThanOrEqual:
            compare_result = value_to_compare >= query_val;
            break;
        default:
            throw invalid_argument("Invalid comparison operator.");
    }

    return compare_result;
}