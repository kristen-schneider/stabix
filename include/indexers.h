#include <string>

class Indexer {
  public:
    virtual int value_to_bin(std::string queryVal) = 0;
    virtual std::string bin_to_value(int bin) = 0;
    virtual ~Indexer() = default;

    void build_index(std::string inPath, std::string outPath);
};

class PValIndexer : public Indexer {
  public:
    int value_to_bin(std::string line) override;
    std::string bin_to_value(int bin) override;
};
