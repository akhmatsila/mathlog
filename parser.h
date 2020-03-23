//
// Created by indalamar on 03.06.19.
//

#ifndef NUCLEAR_PARSER_H
#define NUCLEAR_PARSER_H

#include "operation.h"
#include <string>


class parser {
public :

    int current = 0;
    std::string str = "";

    parser() = default;

    std::string delete_spaces(std::string &exp) {
        std::string answer;
        for (auto &i : exp) if (i != ' ') answer.push_back(i);
        return answer;
    }

    operation *parse(std::string &exp) {
        str = delete_spaces(exp);
        current = 0;
        return parseSO();
    }


    operation *parseSO() {
        operation *answer = parseOR();
        while (current < str.size() && str[current] == '-') {
            current += 2;
            operation *answer2 = parseSO();
            answer = new SO(answer, answer2);
        }
        return answer;
    }

    operation *parseOR() {
        operation *answer = parseAND();
        while (current < str.size() && str[current] == '|') {
            ++current;
            operation *answer2 = parseAND();
            answer = new OR(answer, answer2);
        }
        return answer;
    }

    operation *parseAND() {
        operation *answer = parseB();
        for (;;) {
            if (current == str.size() || str[current] != '&') return answer;
            current++;
            operation *answer2 = parseB();
            answer = new AND(answer, answer2);
        }
    }

    operation *parseB() {
        if (str[current] == '!') {
            current++;
            operation *answer = parseB();
            return new DEC(answer);
        } else {
            if (str[current] == '(') {
                ++current;
                operation *answer = parseSO();
                if (str[current] == ')') {
                    current++;
                }
                return answer;
            }
        }
        return parseVar();
    }

    operation *parseVar() {
        std::string var;
        while (current < str.size() && !is_operation(str[current])) {
            var.push_back(str[current]);
            current++;
        }
        operation *operation1 = new VAR(var);
        return operation1;
    }

    bool is_operation(char t) {
        return t == '-' || t == '&' || t == '!' || t == '|' || t == '(' || t == ')';
    }

};

#endif //NUCLEAR_PARSER_H
