#include <iostream>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <queue>
#include <sstream>
#include "operation.h"
#include "parser.h"
#include "axiom.h"
#include <fstream>
#include "proof.h"


struct reaper {
    proof proof1;
    parser parser1;
    std::unordered_set<operation *> variables;
    std::unordered_map<std::string, int> id;
    std::vector<bool> table;
    std::vector<std::vector<std::string>> proofs;
    std::vector<std::string> hypothesis;
    std::vector<std::string> supers;
    int size = 0;
    int main_mask = 0;
    std::vector<int> mains;
    operation *expr;

    explicit reaper(operation *oper) {
        variables.clear();
        expr = oper;
        collect(oper);
        proofs.emplace_back();
        build_table();
        main_mask = (1 << size) - 1;
        if (table[0] & !table[main_mask]) {
            std::cout << ":(";
        } else {
            if (!table[main_mask] & !table[0]) {
                proofs.clear();
                table.clear();
                proofs.emplace_back();
                expr = new DEC(expr);
                build_table();
                main_mask= 0 ;
            }
            do_some_shit();
        }
    }


    void collect(operation *oper) {
        oper->result = false;
        if (dynamic_cast<abstract_operaiton *>(oper) != nullptr) {
            auto *box = dynamic_cast<abstract_operaiton *> (oper);
            collect(box->left);
            collect(box->right);
        }
        if (dynamic_cast<DEC *>(oper) != nullptr) {
            auto *box = dynamic_cast<DEC *>(oper);
            collect(box->mid);
        }
        if (dynamic_cast<VAR *>(oper) != nullptr) {
            variables.insert(oper);
            if (id.count(oper->print()) == 0) {
                id[oper->print()] = size;
                size++;
            }
        }
    }

    void build_table() {
        int mask = 0;
        int max_mask = (1 << (size)) - 1;
        for (; mask <= max_mask; ++mask) {
            table.push_back(execute(mask));
            build_proof(expr);
            proof_negative(expr);
            proofs.emplace_back();
        }
        proofs.pop_back();
    }

    bool execute(int mask) {
        for (int i = 0; i < size; ++i) {
            for (auto &z : variables) {
                if (id[z->print()] == i) z->result = (mask & (1 << i)) != 0;
            }
        }
        return expr->evaluate();
    }

    void build_proof(operation *oper) {
        if (dynamic_cast<abstract_operaiton *>(oper) != nullptr) {
            auto *box = dynamic_cast<abstract_operaiton *>(oper);
            build_proof(box->left);
            build_proof(box->right);
            if (box->left->result) proof_negative(box->left);
            else proof_negative(new DEC(box->left));
            if (box->right->result) proof_negative(box->right);
            else proof_negative(new DEC(box->right));
            proof_example(box);
        }
        if (dynamic_cast<DEC *>(oper) != nullptr) {
            auto *box = dynamic_cast<DEC *>(oper);
            build_proof(box->mid);
        }
        if (dynamic_cast<VAR *>(oper) != nullptr) {
            if (dynamic_cast<VAR *>(expr) == nullptr) replace_var(oper->print(), oper->result);
            else proofs.back().push_back(oper->print());
        }
    }

    void replace_var(std::string &&var, bool result) {
        std::ifstream input;
        if (result) input = std::ifstream("teil");
        else input = std::ifstream("mein");
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                input.close();
                return;
            }
            std::string box;
            for (auto &i : line) {
                if (i != 'A') box.push_back(i);
                else box += var;
            }
            proofs.back().push_back(box);
        }
        input.close();
    }


    void proof_negative(operation *oper) {
        if (dynamic_cast<DEC *>(oper) == nullptr) return;
        int depth = 0;
        while (dynamic_cast<DEC *>(oper) != nullptr) {
            auto *box = dynamic_cast<DEC *>(oper);
            oper = box->mid;
            depth++;
        }
        if (depth < 2) return;
        if (depth % 2 == 1) oper = new DEC(oper), depth--;
        std::string kinch = oper->print();
        for (int i = 0; i < depth; i += 2) {
            create_negotations(kinch);
            kinch = "!!" + kinch;
        }
    }

    void create_negotations(std::string &kinch) {
        std::ifstream input("teil");
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                input.close();
                return;
            }
            std::string box;
            for (auto &i : line) {
                if (i != 'A') box.push_back(i);
                else box += kinch;
            }
            proofs.back().push_back(box);
        }
        input.close();
    }

    void proof_example(abstract_operaiton *oper) {
        bool leo = oper->left->result;
        bool rio = oper->right->result;
        std::string arg;
        if (leo) arg.push_back('1');
        else arg.push_back('0');
        if (rio) arg.push_back('1');
        else arg.push_back('0');
        if (oper->operator_sign == "->")arg += ("SO");
        else {
            if (oper->operator_sign == "&") arg += "AND";
            else arg += "OR";
        }
        construct_proof_from_sample(arg, oper);
    }

    void construct_proof_from_sample(std::string &file, abstract_operaiton *oper) {
        std::string came;
        std::ifstream input(file);
        while (std::getline(input, came)) {
            if (came.empty()) {
                input.close();
                return;
            }
            std::string box;
            for (auto &i : came) {
                if (i != 'A' && i != 'B') box.push_back(i);
                else {
                    if (i == 'A') box += oper->left->print();
                    else box += oper->right->print();
                }
            }
            proofs.back().push_back(box);
        }
        input.close();
    }

    void find_mains() {
        for (int i = 0; i < size; ++i)
            for (auto &z : id)
                if (z.second == hypothesis.size())
                    hypothesis.push_back(z.first);
        int full = (1 << size) - 1;
        mains = flow_permutations();
        int mask = 0;
        for (int &main : mains) {
            mask += (1 << main);
        }
        int superhyp = mask ^(full);
        for (int i = 0; i < size; ++i) {
            if ((superhyp & (1 << i)) != 0) supers.push_back(main_mask > 0 ? hypothesis[i] : "!" + hypothesis[i]);
        }
    }


    void do_some_shit() {
        find_mains();
        std::string answer;
        for (auto &i : supers) answer += i + " ,";
        if (!answer.empty()) answer.pop_back();
        answer += "|- ";
        answer += proofs[main_mask].back();
        std::cout << answer << std::endl;
        if (mains.empty()) {
            for (auto &i : proofs[main_mask]) std::cout << i << std::endl;
            return;
        }
        auto test = find_best(0, main_mask);
        for (auto &i : test) std::cout << i << std::endl;
    }


    std::vector<std::string> find_best(int depth, int mask) {
        std::vector<std::string> hyp = supers;
        for (int i = 0; i < depth; ++i) {
            if ((mask & (1 << mains[i])) != 0)hyp.push_back(hypothesis[mains[i]]);
            else hyp.push_back("!" + hypothesis[mains[i]]);
        }
        if (depth == mains.size() - 1) {
            std::string need = (main_mask > 0 ? "" : "!") + hypothesis[mains[depth]];
            std::vector<std::string> l = proof1.transform(proofs[mask], need, hyp);
            need = main_mask > 0 ? "!" + need : hypothesis[mains[depth]];
            std::vector<std::string> r = proof1.transform(proofs[mask ^ (1 << (mains[depth]))], need, hyp);
            return unite(l, r);
        }
        std::string need = (main_mask > 0 ? "" : "!") + hypothesis[mains[depth]];
        auto l = find_best(depth + 1, mask);
        l = proof1.transform(l, need, hyp);
        need = main_mask > 0 ? "!" + need : hypothesis[mains[depth]];
        auto r = find_best(depth + 1, mask ^ (1 << mains[depth]));
        r = proof1.transform(r, need, hyp);
        return unite(l, r);
    }

    std::vector<std::string> unite(std::vector<std::string> &l, std::vector<std::string> &r) {
        auto *box = dynamic_cast<abstract_operaiton *>(parser1.parse(main_mask > 0 ? l.back() : r.back()));
        std::string B = box->left->print();
        std::string C = box->right->print();
        std::vector<std::string> answer = main_mask > 0 ? l : r;
        for (auto &i : main_mask > 0 ? r : l) answer.push_back(i);
        std::ifstream input("disj");
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                input.close();
                return answer;
            }
            std::string lc;
            for (auto &i : line) {
                if (i != 'B' && i != 'C') lc.push_back(i);
                else {
                    if (i == 'B') lc += B;
                    else lc += C;
                }
            }
            answer.push_back(lc);
        }
        input.close();
        return answer;
    }

    int perm(std::vector<int> &permutat, int depth, int mask, int &result) {
        if (depth == size - 1) {
            if (table[mask] == table[mask ^ (1 << permutat[depth])]) {
                if (table[mask]) {
                    result = depth;
                    return 1;
                }
                return 0;
            }
            return 0;
        }

        if (perm(permutat, depth + 1, mask, result) == 1 &&
            perm(permutat, depth + 1, mask ^ (1 << permutat[depth]), result) == 1) {
            result = depth;
            return 1;
        }
        return 0;
    }

    std::vector<int> flow_permutations() {
        std::vector<int> permutat(static_cast<unsigned long>(size));
        for (int i = 0; i < permutat.size(); ++i) permutat[i] = i;
        std::vector<int> answer;
        int huita = 10;
        for (int i = 0; i < factorial(size); ++i) {
            int check = 10;
            perm(permutat, 0, main_mask, check);
            if (check < huita) {
                huita = check;
                answer.clear();
                for (int z = check; z < size; ++z) answer.push_back(permutat[z]);
            }
            std::next_permutation(permutat.begin(), permutat.end());
        }
        return answer;
    }

    int factorial(int x) {
        int result = 1;
        for (int i = 1; i <= x; ++i) result *= i;
        return result;
    }


};


int main() {
    parser parser1;
    std::string kinch;
    std::getline(std::cin, kinch);
    operation *test = parser1.parse(kinch);
    reaper check(test);
    return 0;
}