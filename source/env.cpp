#include <numeric>

#include "env.hpp"

static bool ListTypes(const Value& list) {
    List listArgs = std::get<List>(list);
    bool allNumeric = true;
    bool allStrings = true;
    for(const Value& val : listArgs.value) {
	std::visit([&](const auto& value) {
	    if constexpr (std::is_arithmetic_v<decltype(value.value)>) {
		allStrings = false; 
	    } else if constexpr (std::is_same_v<decltype(value.value), std::string>) {
		allNumeric = false;
	    } else {
		allNumeric = false;
		allStrings = false;
	    }
	}, val);
    }
    if (!allNumeric && !allStrings) {
	throw std::runtime_error("Syntax error: List contains a mix of numeric and string types");
    } else if(allNumeric) {
	return true;
    } else {
	return false;
    }
}

static auto constexpr IntVisitor = [](const auto& x) -> int {
    if constexpr(std::is_arithmetic_v<decltype(x.value)>) {
	return x.value;
    } else {
	return 0;
    }
};

static auto constexpr StrVisitor = [](const auto& x) -> std::string {
    if constexpr(std::is_same_v<decltype(x.value), std::string>) {
	return x.value;
    } else {
	return {};
    }
};

Value addOp(const Value &args) {
    if(std::holds_alternative<List>(args)) {
	if(ListTypes(args)) {
	    List listArgs = std::get<List>(args);
	    int res = std::accumulate(
		listArgs.value.begin(), listArgs.value.end(), 0,
		[](int acc, Value &curr) {
		    return acc + std::visit(IntVisitor, curr);
		});
	    return Int{res, std::to_string(res)};
        } else {
	    List listArgs = std::get<List>(args);
	    std::string res = std::accumulate(
		listArgs.value.begin(), listArgs.value.end(), std::string(""),
		[](const std::string &acc, Value &curr) {
		    return acc + std::visit(StrVisitor, curr);
		});
	    return Str{res, res};
        }
    } else {
	return args;
    }
}

Value subOp(const Value &args) {
    if (std::holds_alternative<List>(args)) {
	if (ListTypes(args)) {
	    List listArgs = std::get<List>(args);
	    int res = std::accumulate(
		listArgs.value.begin() + 1, listArgs.value.end(), std::get<Int>(listArgs.value.at(0)).value,
		[](int acc, Value &curr) {
		    return acc - std::visit(IntVisitor, curr);
		});
	    return Int{res, std::to_string(res)};
        } else {
	    throw std::runtime_error("Syntax Error: operator '-' not supported for strings");
        }
    } else {
	return args;
    }
}

Value mulOp(const Value &args) {
    if (std::holds_alternative<List>(args)) {
	if (ListTypes(args)) {
	    List listArgs = std::get<List>(args);
	    int res = std::accumulate(
		listArgs.value.begin(), listArgs.value.end(), 1,
		[](int acc, Value &curr) {
		    return acc * std::visit(IntVisitor, curr);
		});
	    return Int{res, std::to_string(res)};
        } else {
	    throw std::runtime_error("Syntax Error: operator '*' not supported for strings");
        }
    } else {
	return args;
    }
}

Value divOp(const Value &args) {
    if (std::holds_alternative<List>(args)) {
	if (ListTypes(args)) {
	    List listArgs = std::get<List>(args);
            int res = std::accumulate(
		listArgs.value.begin() + 1, listArgs.value.end(), std::get<Int>(listArgs.value.at(0)).value,
		[](int acc, Value &curr) {
		    return acc / std::visit(IntVisitor, curr);
		});
	    return Int{res, std::to_string(res)};
        } else {
	    throw std::runtime_error("Syntax Error: operator '/' not supported for strings");
        }
    } else {
	return args;
    }
}

