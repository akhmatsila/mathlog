//
// Created by indalamar on 05.06.19.
//

#ifndef NUCLEAR_PROOF_H
#define NUCLEAR_PROOF_H

#include <string>
#include "operation.h"
#include <unordered_map>
#include <vector>
#include "parser.h"
#include <unordered_set>
#include "axiom.h"
#include <fstream>


struct operation_hash {
    std::size_t operator()(const operation *k) const {
        return k->hash();
    }
};

struct operation_equality {
    bool operator()(operation *k, operation *s) const {
        return k->operator==(s);
    }
};


struct META {
    int way = -1;
    int id = -1;
};

struct MP {
    std::unordered_set<operation *, operation_hash, operation_equality> possible_M = {};
};

struct MP_total {
    operation *L = nullptr;
};


class proof {
private :


    operation *A_op=nullptr;
    std::string needed;

    std::vector<axiom> axioms;

    std::unordered_map<operation *, int, operation_hash, operation_equality> hypothesis;

    std::unordered_map<operation *, META, operation_hash, operation_equality> printed;


    std::unordered_map<operation *, MP, operation_hash, operation_equality> MP_check;

    std::unordered_map<operation *, MP_total, operation_hash, operation_equality> MP_correct;


    std::vector<std::string> old_proof;
    std::vector<std::string> new_proof;


    parser parser1;


public :
    explicit proof() {
        std::vector<std::string> axiomss(10);
        axioms.resize(10);
        axiomss[0] = "A->(B->A)";
        axiomss[1] = "(A->B)->((A->B->C)->(A->C))";
        axiomss[2] = "A->(B->(A&B))";
        axiomss[3] = "(A&B)->A";
        axiomss[4] = "(A&B)->B";
        axiomss[5] = "A ->(A|B)";
        axiomss[6] = "B->(A|B)";
        axiomss[7] = "(A->C)->((B->C)->((A|B)->C))";
        axiomss[8] = "(A->B)->((A->!B)->!A)";
        axiomss[9] = "(!!A)->A";
        for (int i = 0; i < 10; ++i) axioms[i] = {parser1.parse(axiomss[i])};


    }


    std::vector<std::string>
    transform(std::vector<std::string> &proof, std::string &need, std::vector<std::string> &hypo) {
        new_proof.clear();
        old_proof.clear();
        A_op = parser1.parse(need);
        needed = A_op->print();
        old_proof = proof;
        for (int i = 0; i < hypo.size(); ++i) {
            hypothesis[parser1.parse(hypo[i])] = i;
        }
        get_proof();
        for (auto &i : printed) delete (i.first);
        printed.clear(), MP_check.clear(), MP_correct.clear();
        hypothesis.clear();
        return new_proof;
    }


    bool get_proof() {
        for (auto &i : old_proof) {
            operation *compose = parser1.parse(i);
            if (is_hypothesis(compose) || is_scheme(compose)) transform_ha(compose);
            else {
                if (is_modus_ponens(compose)) {
                    transform_mp(MP_correct[compose].L, compose);
                } else {
                    if (is_same(compose)) transform_kinch(compose);
                }
            }
            it_can_be_modens_ponens(compose);
        }
        return true;
    }

    bool is_same(operation *oper) {
        printed[oper]={1,0};
        return A_op->operator==(oper);
    }

    bool is_hypothesis(operation *oper) {
        if (hypothesis.find(oper) != hypothesis.end()) {
            if (printed.find(oper) == printed.end()) printed[oper] = {0, 0};
            return true;
        }
        return false;
    }

    bool is_scheme(operation *oper) {
        for (int i = 0; i < 10; ++i) {
            if (axioms[i].is_scheme(oper)) {
                if (printed.find(oper) == printed.end()) printed[oper] = {1, i + 1};
                return true;
            }
        }
        return false;
    }

    bool is_modus_ponens(operation *oper) {
        if (MP_check.find(oper) == MP_check.end()) return false;
        if (!MP_check[oper].possible_M.empty()) {
            for (auto j : MP_check[oper].possible_M) {
                if (printed.find(dynamic_cast<abstract_operaiton *>(j)->left) != printed.end()) {
                    if (printed.find(oper) == printed.end())printed[oper] = {2, 0};
                    MP_correct[oper].L = j;
                    return true;
                }
            }
        }
        return false;
    }

    bool it_can_be_modens_ponens(operation *oper) {
        if (oper->operator_sign == "->") {
            auto *box = dynamic_cast<abstract_operaiton *> (oper);
            MP_check[box->right].possible_M.insert(box);
            return true;
        }
        return false;
    }

    void transform_ha(operation *op) {
        std::ifstream input("term");
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                input.close();
                return;
            }
            std::string box;
            for (auto &i : line) {
                if (i != 'A' && i != 'B') box.push_back(i);
                else {
                    if (i == 'A') box += op->print();
                    else box += needed;
                }
            }
            new_proof.push_back(box);
        }
    }

    void transform_mp(operation *left, operation *right) {
        auto * shaluto = dynamic_cast<abstract_operaiton*>(left);
        std::ifstream input("mp");
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                input.close();
                return;
            }
            std::string box;
            for (auto &i : line) {
                if (i != 'A' && i != 'B' && i != 'C') box.push_back(i);
                else {
                    if (i == 'A') {
                        box += needed;
                    } else {
                        if (i == 'B') box += shaluto->left->print();
                        else box += right->print();
                    }
                }
            }
            new_proof.push_back(box);
        }
    }

    void transform_kinch(operation *compose) {
        std::ifstream input("huitanonetaineeta");
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                input.close();
                return;
            }
            std::string box;
            for (auto &i : line) {
                if (i != 'A') box.push_back(i);
                else box += compose->print();
            }
            new_proof.push_back(box);
        }

    }


};


#endif //NUCLEAR_PROOF_H
