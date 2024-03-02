#pragma once

#include <variant>
#include <string>
#include <functional>
#include <any>
#include <vector>

template <typename T>
struct ValueType {
    ValueType(T val, const std::string& tok) : value(val), token(tok) {}
    T value;
    const std::string token;
};

struct Nil {
    explicit Nil(const std::string& tok) : token(tok) {}
    const bool value = false;
    const std::string token;
};

using Int = ValueType<int>;
using Bool = ValueType<bool>;
using Str = ValueType<std::string>;
using Sym = ValueType<std::any>;

using Value = std::variant<Nil, Int, Bool, Str, Sym, struct List, struct Func>;

struct List {
    std::vector<Value> value;
    const std::string token = {};
};

using Ast = List;

struct Func {
    std::function<Value(const Value&)> value;
    const std::string token = {};
};

