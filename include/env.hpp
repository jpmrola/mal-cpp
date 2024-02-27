#pragma once

#include <stdexcept>
#include <unordered_map>
#include <string>
#include <utility>
#include <optional>

#include "types.hpp"

Value addOp(const Value &args);
Value subOp(const Value &args);
Value mulOp(const Value &args);
Value divOp(const Value &args);

struct ReplEnv {

    explicit ReplEnv(std::optional<ReplEnv*> outerEnv = std::nullopt): outer(outerEnv) {}

    Func add = { addOp };
    Func sub = { subOp };
    Func mul = { mulOp };
    Func div = { divOp };
    std::unordered_map<std::string, Value> envValues = {
	{ "+", add },
	{ "-", sub },
	{ "*", mul },
	{ "/", div },
    };

    void set(const std::string &&key, const Value &&val) {
	envValues.emplace(key, val);
    }
    void set(const std::string& key, const Value& val) {
	envValues.insert({key, val});
    }
    std::optional<const ReplEnv*> find(const std::string& key) const {
	if(envValues.find(key) == envValues.end()) {
	    if(outer.has_value()) {
		return outer.value()->find(key);
	    } else {
		return std::nullopt;
	    }
	} else {
	    return this;
	}
    }
    Value get(const std::string& key) const {
	std::optional<const ReplEnv*> env = find(key);
	if(env.has_value()) {
	    return env.value()->envValues.at(key);
	} else {
	    throw std::runtime_error("Undefined reference to symbol: " + key);
	}
    }

    std::optional<ReplEnv*> outer;
};

