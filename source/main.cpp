#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

#include "read.hpp"
#include "eval.hpp"

static void traverseLists(const List &list) {
    auto& tokens = list.value;
    for(const Value& val : tokens) {
	if(!std::holds_alternative<List>(val)) {
	    std::visit([](auto &&arg){ std::cout << "[" << arg.token << ", \"" << arg.typeInfo << "\"] ";  }, val);
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
	    return std::to_string(inVal.value);
	} else if constexpr(std::is_same_v<decltype(inVal.value), std::string>) {
	    return inVal.value;
	} else {
	    return "";
	}
    }, input);
}

std::string rep(std::string input) {
    std::string output = "Result:";
    Ast ast = READ(std::move(input));
    traverseLists(ast);
    std::cout << '\n';
    for(const Value& list : ast.value) {
	auto res = EVAL(list);
	output += "\n\tList Value: ";
	output += PRINT(res);
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

