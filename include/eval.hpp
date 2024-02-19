#pragma once

#include <unordered_map>
#include <string>

#include "types.hpp"

Value evalAst(Value ast);

Value EVAL(Value ast);

Value AddOp(const Value &args);
Value SubOp(const Value &args);
Value MulOp(const Value &args);
Value DivOp(const Value &args);

struct ReplEnv {

    Func add = { AddOp };
    Func sub = { SubOp };
    Func mul = { MulOp };
    Func div = { DivOp };
    std::unordered_map<std::string, Value> envValues = {
	{ "+", add },
	{ "-", sub },
	{ "*" , mul },
	{ "/" , div },
    };
};

