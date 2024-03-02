#include "env.hpp"
#include "types.hpp"
#include "eval.hpp"
#include <stdexcept>
#include <variant>

struct EvalVisitor {
    explicit EvalVisitor(ReplEnv& curr_env) : env(curr_env) {}
    
    Value operator()(const List& list) {
	List newList;
	for(const Value& val : list.value) {
	    newList.value.emplace_back(EVAL(val, env));
	}
	return newList;
    }
    Value operator()(const Sym& symbol) { return env.get(symbol.token); }
    Value operator()(const auto& other) { return other; }
    ReplEnv& env;
};

Value evalAst(const Value& ast, ReplEnv& env) {
    EvalVisitor visitor(env);
    Value res = std::visit(visitor, ast);
    return res;
}

Value EVAL(const Value& ast, ReplEnv& env) {
    if(std::holds_alternative<List>(ast)) {
	const List& astList = std::get<List>(ast);
	if(astList.value.empty()) {
	    return ast;
	} else {
	    if(std::holds_alternative<Sym>(astList.value.at(0))) {
		const Sym& firstSymbol = std::get<Sym>(astList.value.at(0));
		if(astList.value.size() == 1) {
		    return EVAL(firstSymbol, env);
		}
		const bool secondIsSymbol = std::holds_alternative<Sym>(astList.value.at(1));
		if(firstSymbol.token == "def!") {
		    if(secondIsSymbol) {
			const Sym& secondSymbol = std::get<Sym>(astList.value.at(1));
			const Value body = astList.value.at(2);
			env.set(secondSymbol.token, EVAL(body, env));
		    } else {
			throw std::runtime_error("Function name must be a symbol");
		    }
		} else if(firstSymbol.token == "let*") {
		    ReplEnv newEnv(&env);
		    if(std::holds_alternative<List>(astList.value.at(1))) {
			const List& bindList = std::get<List>(astList.value.at(1));
			for(size_t i = 0; i < bindList.value.size(); i += 2) {
			    if(std::holds_alternative<Sym>(bindList.value.at(i))) {
				Sym key = std::get<Sym>(bindList.value.at(i));
				Value evalValue = EVAL(bindList.value.at(i + 1), newEnv); 
				newEnv.set(key.token, evalValue);
			    } else {
				throw std::runtime_error("let* definition needs a symbol"); 
			    }
			}
			return EVAL(astList.value.at(2), newEnv);
		    }
		} else {
		    const List evaluated = std::get<List>(evalAst(ast, env));
		    if(std::holds_alternative<Func>(evaluated.value.at(0))) {
			Func firstElem = std::get<Func>(evaluated.value.at(0));
			const List args = { .value = { evaluated.value.begin() + 1, evaluated.value.end() }};
			Value res = firstElem.value(args);
			return res;
		    } else {
			throw std::runtime_error("First element must be a function");
		    }
		}
	    }
	}
    } else {
	return evalAst(ast, env);
    }
    return ast;
}

