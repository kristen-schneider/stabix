#include "utils.h"
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

class Indexer {
  protected:
    std::string indexPath;
    BlockLineMap *blockLineMap;

  public:
    virtual float value_to_bin(std::string line) = 0;
    virtual std::string bin_to_value(float bin) = 0;
    virtual ~Indexer() = default;

    Indexer(std::string indexPath, BlockLineMap *blockLineMap);
    void build_index(std::string inPath, int queryColumn);
    std::unordered_set<int> query_index(std::function<bool(float)> predicate);
};

enum class ComparisonType {
    LessThan,
    LessThanOrEqual,
    Equal,
    GreaterThan,
    GreaterThanOrEqual
};

class PValIndexer : public Indexer {
  private:
    std::vector<float> bins;
    float nearest_bin(float value);

  public:
    PValIndexer(std::string indexPath, BlockLineMap *blockLineMap,
                std::vector<float> bins);
    float value_to_bin(std::string line) override;
    std::string bin_to_value(float bin) override;
    std::unordered_set<int> compare_query(float threshold,
                                          ComparisonType compType);
};

bool compare_values(
        string config_query,
        float value_to_compare);
