#pragma once
#include <memory>
template <typename T, typename Parameter>
class NamedType
{
public:
    explicit NamedType(T const& value) : value_(value) {}
    explicit NamedType(T&& value) : value_(std::move(value)) {}
    T& get() { return value_; }
    T const& get() const {return value_; }

    friend std::ostream &operator<<(std::ostream& os, const NamedType & other){
        return os<<other.get();
    }

    NamedType &operator+=(const NamedType & other){
        value_ += other.value_;
        return *this;
    }

    NamedType &operator-=(const NamedType & other){
        value_ -= other.value_;
        return *this;
    }


private:
    T value_;
};

using Cash = NamedType<int, struct CashParameter>;
using Price = NamedType<Cash, struct PriceParameter>;

bool operator <= (const Cash cash, const Price price){
    if(cash.get() <= price.get().get())
        return true;
    return false;
}
