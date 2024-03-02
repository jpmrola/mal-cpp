#include <iostream>
#include <chrono>
#include <readline/readline.h>
#include <readline/history.h>

#include "read.hpp"
#include "eval.hpp"
#include "env.hpp"
#include "types.hpp"

static void traverseLists(const List &list) {
    auto& tokens = list.value;
    for(const Value& val : tokens) {
	if(!std::holds_alternative<List>(val)) {
	    std::visit([](Value &&arg){
		if(std::holds_alternative<Sym>(arg)) {
		    std::cout << "[" << std::get<Sym>(arg).token << ", \"Symbol\"] ";
		} else if(std::holds_alternative<Str>(arg)) {
		    std::cout << "[" << std::get<Str>(arg).token << ", \"String\"] ";
		} else if(std::holds_alternative<Bool>(arg)) {
		    std::cout << "[" << std::get<Bool>(arg).token << ", \"Bool\"] ";
		} else if(std::holds_alternative<Int>(arg)) {
		    std::cout << "[" << std::get<Int>(arg).token << ", \"Integer\"] ";
		} else if(std::holds_alternative<Nil>(arg)) {
		    std::cout << "[" << std::get<Nil>(arg).token << ", \"Nil\"] ";
		} else if(std::holds_alternative<Func>(arg)) {
		    std::cout << "[" << std::get<Func>(arg).token << ", \"Function\"] ";
		}
	    }, val);
	} else if(std::holds_alternative<List>(val)) {
	    std::cout << "( ";
	    traverseLists(std::get<List>(val));
	    std::cout << ") ";
	}
    }
}

std::string PRINT(const Value& input) {
    return std::visit([](const auto& inVal) -> std::string {
	if constexpr(std::is_arithmetic_v<decltype(inVal.value)>) {
	    if constexpr(std::is_same_v<decltype(inVal.value), bool>) {
		std::string res = inVal.value ? "true" : "false";
		return res;
	    } else {
		return std::to_string(inVal.value);
	    }
	} else if constexpr(std::is_same_v<decltype(inVal.value), std::string>) {
	    return inVal.value;
	} else if constexpr(std::is_same_v<decltype(inVal.value), std::function<Value(const Value&)>>) {
	    return "#<function>";
	} else {
	    return "";
	}
    }, input);
}

std::string rep(std::string input) {
    static ReplEnv rootEnv;
    Ast ast = READ(std::move(input));
    traverseLists(ast);
    std::string output = "Result:";
    std::cout << '\n';
    for(const Value& list : ast.value) {
	auto start = std::chrono::high_resolution_clock::now();
	auto res = EVAL(list, rootEnv);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	output += "\n\tList Value: ";
	output += PRINT(res);
	output += "\n\tEval time: " + std::to_string(duration.count());
    }
    return output;
}

int main(int argc, char** argv) {
    char* input;
    while((input = readline("user> ")) != nullptr) {
	if(input[0] == '\0') {
	    free(input);
	}
	else {
	    add_history(input);
	    try {
		std::cout << rep(input) << '\n';
	    } catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	    }
	    free(input);
	}
    }
}

