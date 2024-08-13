#include <functional>
#include <string>
#include <vector>

class Indexer {
  protected:
    std::string indexPath;

  public:
    virtual float value_to_bin(std::string line) = 0;
    virtual std::string bin_to_value(float bin) = 0;
    virtual ~Indexer() = default;

    Indexer(std::string indexPath);
    void build_index(std::string inPath);
    std::vector<int> query_index(std::function<bool(float)> predicate);
};

enum class ComparisonType {
    LessThan,
    LessThanOrEqual,
    Equal,
    GreaterThanOrEqual,
    GreaterThan
};

class PValIndexer : public Indexer {
  private:
    std::vector<float> bins;
    float nearest_bin(float value);

  public:
    PValIndexer(std::vector<float> bins);
    float value_to_bin(std::string line) override;
    std::string bin_to_value(float bin) override;
    std::vector<int> compare_query(float threshold, ComparisonType compType);
};
