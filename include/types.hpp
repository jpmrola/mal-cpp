#pragma once

#include <variant>
#include <string>
#include <deque>
#include <functional>
#include <any>
#include <vector>

template <typename T>
struct ValueType {
    ValueType(T val, const std::string& tok) : value(val), token(tok) {}
    T value;
    const std::string token;
    static inline const char* typeInfo;
};

struct Nil {
    const bool value = false;
    const std::string token;
    static inline const char* typeInfo = "nil";
};

template <>
const char* ValueType<int>::typeInfo = "int";
template <>
const char* ValueType<bool>::typeInfo = "bool";
template <>
const char* ValueType<std::string>::typeInfo = "string";
template <>
const char* ValueType<std::any>::typeInfo = "symbol";

using Int = ValueType<int>;
using Bool = ValueType<bool>;
using Str = ValueType<std::string>;
using Sym = ValueType<std::any>;

using Value = std::variant<Nil, Int, Bool, Str, Sym, struct List, struct Func>;

struct List {
    std::vector<Value> value;
    std::string token = {};
    static inline const char* typeInfo = "list";
};

using Ast = List;

struct Func {
    std::function<Value(const Value&)> value;
    std::string token = {};
    static inline const char* typeInfo = "function";
};

