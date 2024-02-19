#pragma once

#include <string>
#include <sstream>

#include "types.hpp"

Ast READ(std::string input);

class Tokenizer {
public:
    class iterator {
    public:
        explicit iterator(std::istringstream& s) : m_input(&s) {
            extractToken();
	}
        iterator() : m_input(nullptr), token("") {}

        iterator& operator++() {
            extractToken();
            return *this;
        }
        std::string operator*() const {
            return token;
        }

        bool operator!=(const iterator& other) const {
            return m_input != (other.m_input);
        }

        bool operator==(const iterator& other) const {
            return m_input == (other.m_input);
        }

    private:
        std::istringstream* m_input;
        std::string token;

        void extractToken();
    };
    
    explicit Tokenizer(std::string& input) : m_input(std::move(input)) {}

    iterator begin() {
	return iterator(m_input);
    }
    iterator end() {
	return iterator();
    }

private:
    std::istringstream m_input;
};

Value classify(const std::string& token);

