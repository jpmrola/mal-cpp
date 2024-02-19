#include <stdexcept>
#include <string>
#include <stack>

#include "read.hpp"

void Tokenizer::iterator::extractToken() {
    if(m_input == nullptr) {
	return;
    }
    while(m_input->peek() != EOF) {
	char c = m_input->peek();
	switch(c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	case ',':
	    m_input->get();
	    break;
	case ';': {
	    token = "";
	    char tmp = m_input->get();
	    while(tmp != EOF) {
		tmp = m_input->get();
	    }
	    break;
	}
	case '~':{
	    token = c;
	    m_input->get();
	    char peeked = m_input->peek();
	    if(peeked == '@') {
		token += peeked;
		m_input->get();
	    }
	    return;
	}
	case '(':
	case ')':
	case '{':
	case '}':
	case '[':
	case ']':
	case '\'':
	case '`':
	case '^':
	case '@':
	    token = c;
	    m_input->get();
	    return;
	case '"': {
	    token = m_input->get();
	    char tmp = m_input->peek();
	    if(tmp == '"') {
		token += tmp;
		m_input->get();
		return;
	    } else {
		while(tmp != '"') {
		    if(tmp == EOF) {
			throw std::runtime_error("Unterminated string");
		    }
		    tmp = m_input->get();
		    token += tmp;
		}
	    }
	    return;
	}
	default: {
	    token = m_input->get();
	    bool finished = false;
	    while(!finished) {
		char tmp = m_input->peek();
		switch(tmp) {
		case ' ':
		case '\n':
		case '\r':
		case '\t':
		case '(':
		case ')':
		case '{':
		case '}':
		case '[':
		case ']':
		case '"':
		case '\'':
		case '`':
		case '^':
		case '~':
		case '@':
		case ',':
		case ';':
		case EOF:
		    finished = true;
		    break;
		default:
		    token += m_input->get();
		}
	    }
	    return;
	}
	}
    }
    if(m_input->peek() == EOF) {
	m_input = nullptr;
    }
}

static bool isNumber(const std::string& token) {
   if(std::isdigit(token[0]) || (token[0] == '-' && std::isdigit(token[1]))) {
       return true; 
   }
   return false;
}

Value classify(const std::string &token) {
    if(token == "nil") {
        return Nil{};
    } else if(token[0] == '(') {
        return Sym { token , token };
    } else if(token[0] == ')') {
        return Sym { token, token };
    } else if(token[0] == '"' || token[0] == '\'') {
	auto val = token;
	val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
        return Str{val, token};
    } else if(isNumber(token)) {
        return Int{std::stoi(token), token };
    } else if(token == "true" || token == "false") {
        return Bool{token == "true", token };
    } else {
        return Sym{token, token};
    }
}


Ast READ(std::string input) {
    Ast ast;
    std::stack<List> listStack; 
    Tokenizer tokenizer(input);
    for(const std::string& token : tokenizer) {
       Value classified = classify(token); 

       if(std::holds_alternative<Sym>(classified)) {
	   if(std::get<Sym>(classified).token == "(") {
	       listStack.push(List{});
	       continue;
	   } else if(std::get<Sym>(classified).token == ")") {
	       if(listStack.empty()) {
		   throw std::runtime_error("Mismatched parentheses: unexpected closing ')'");
	       }
	       List currentList = listStack.top();
	       listStack.pop();
	       if(listStack.empty()) {
		   ast.value.push_back(currentList);
	       } else {
		   listStack.top().value.push_back(currentList);
	       }
	       continue;
	   } 
       }
       if (listStack.empty()) {
	   throw std::runtime_error("Syntax Error: Expressions must be inside parentheses");
       } else {
	   listStack.top().value.push_back(classified);
       }
    }

    if (!listStack.empty()) {
        throw std::runtime_error("Mismatched parentheses: unclosed list(s)");
    }

    return ast;
}

