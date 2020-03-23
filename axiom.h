//
// Created by indalamar on 03.06.19.
//

#ifndef NUCLEAR_AXIOM_H
#define NUCLEAR_AXIOM_H

#include <unordered_map>
#include "operation.h"


class axiom {
private:
    operation *kernel;
    std::unordered_map<std::string, operation *> flow_variable_map;
public:
    axiom() = default;

    axiom(operation *ker) : kernel(ker) {};

    bool is_scheme(operation *kern, operation *expr) {
        if (dynamic_cast<abstract_operaiton *>(expr) != nullptr) {
            if (kern->operator_sign == expr->operator_sign) {
                auto *box1 = dynamic_cast<abstract_operaiton *> (kern);
                auto *box2 = dynamic_cast<abstract_operaiton *> (expr);
                return is_scheme(box1->left, box2->left) && is_scheme(box1->right, box2->right);
            } else return false;
        }
        if (dynamic_cast<DEC *>(expr) != nullptr) {
            if (kern->operator_sign == expr->operator_sign) {
                auto *box1 = dynamic_cast<DEC *> (kern);
                auto *box2 = dynamic_cast<DEC *> (expr);
                return is_scheme(box1->mid, box2->mid);
            } else return false;
        }
        if (dynamic_cast<VAR *>(expr) != nullptr) {
            std::string tree_string = expr->print();
            if (flow_variable_map.count(tree_string) == 0) {
                flow_variable_map[tree_string] = kern;
            } else return flow_variable_map[tree_string]->operator==(kern);
        }
        return true;
    }

    bool is_scheme(operation *expr) {
        flow_variable_map.clear();
        return is_scheme(expr, kernel);
    }
};

#endif //NUCLEAR_AXIOM_H
