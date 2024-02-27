#pragma once

#include "types.hpp"
#include "env.hpp"

Value evalAst(const Value& ast);

Value EVAL(const Value& ast, ReplEnv& curr_env);

