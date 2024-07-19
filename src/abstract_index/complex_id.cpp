#include <compare>
#include <iostream>
#include <string>

using namespace std;

class ComplexId {
    virtual std::strong_ordering operator<=>(const ComplexId &other) const = 0;
    virtual std::string to_string() const = 0;
};

// Concept for numeric types (C++20)
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <Numeric T> class NumericComplexId : public ComplexId {
  private:
    T value;

  public:
    NumericComplexId(T v) : value(v) {}

    std::strong_ordering operator<=>(const ComplexId &other) const override {
        if (const auto *derived =
                dynamic_cast<const NumericComplexId<T> *>(&other)) {
            return value <=> derived->value;
        }
        throw std::runtime_error("Comparing incompatible types");
    }

    auto operator<=>(const NumericComplexId<T> &) const = default;

    std::string to_string() const override { return std::to_string(value); }

    // Additional common numeric operations could be added here
    T getValue() const { return value; }
};

// Type aliases for convenience
using IntComplexId = NumericComplexId<int>;
using FloatComplexId = NumericComplexId<float>;
using DoubleComplexId = NumericComplexId<double>;

int main() {

    NumericComplexId<int> a(1);
    NumericComplexId<int> b(2);

    cout << (a > b) << endl;
    // cout << (a < b) << endl;
    // cout << (a == b) << endl;

    return 0;
}
