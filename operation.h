//
// Created by indalamar on 03.06.19.
//

#ifndef NUCLEAR_OPERATION_H
#define NUCLEAR_OPERATION_H

#include <string>
#include <unordered_map>


class operation {
public:
    mutable bool result = false ;
    std::string operator_sign;

    virtual std::string print() = 0;

    virtual size_t hash() const = 0;

    virtual bool operator==(operation *oper) const = 0;

    virtual bool operator!=(operation *oper) const {
        return !(this->operator==(oper));
    }

    virtual bool evaluate () const = 0 ;

    virtual ~operation() = default;


};

class abstract_operaiton : virtual public operation {
public :
    operation *left;
    operation *right;

    bool evaluate () const override {
        result=action(left->evaluate(),right->evaluate());
        return result;
    }

    virtual bool action (bool first , bool second) const  = 0;

    abstract_operaiton(operation *left, operation *right) : left(left), right(right) {};

    std::string print() override {
        return  "("+left->print() + operator_sign  + right->print()+")" ;
    }

    size_t hash() const override {
        return left->hash() * 31 + right->hash() * 17 + (std::hash<std::string>{}(operator_sign));
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<abstract_operaiton *> (oper) != nullptr) {
            auto *box = dynamic_cast<abstract_operaiton *> (oper);
            return operator_sign == box->operator_sign && left->operator==(box->left) &&
                   right->operator==(box->right);
        }
        return false;
    }


};

using AO = abstract_operaiton;
using first = operation;

class AND :virtual public AO {
public :
    AND(first *left, first *right) : abstract_operaiton(left, right) {
        this->operator_sign = "&";
    };

    bool action (bool first, bool second)const override {
        return first&second ;
    }
};

class OR :virtual public AO {
public :
    OR(first *left, first *right) : AO(left, right) {
        this->operator_sign = "|";
    };
    bool action (bool first, bool second)const  override {
        return first | second ;
    }
};

class SO :virtual public AO {
public :
    SO(first *left, first *right) : AO(left, right) {
        this->operator_sign = "->";
    };

    bool action (bool first , bool second )const override {
        return (!first) | second ;
    }
};

class DEC :virtual public operation {
public :
    first *mid;

    explicit DEC(first *mid) : mid(mid) { this->operator_sign = "!"; };

    std::string print() override {
        return "!" + mid->print();
    }

    size_t hash() const override {
        return mid->hash() * 71;
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<DEC * > (oper) != nullptr) {
            auto *box = dynamic_cast<DEC *> (oper);
            return mid->operator==(box->mid);
        }
        return false;
    }

    bool evaluate( )const override {
        result = !mid->evaluate();
        return result;
    }

};


class VAR :virtual public operation {
public :
    std::string var;


    explicit VAR(std::string &var) : var(var) {
        this->operator_sign = "var";
    };

    std::string print() override {
        return var;
    }

    size_t hash() const override {
        return (std::hash<std::string>{}(var));
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<VAR *> (oper) != nullptr) {
            auto *box = dynamic_cast<VAR *> (oper);
            return var == box->var;
        }
        return false;
    }

    bool evaluate() const override {
        return result ;
    }

};


#endif //NUCLEAR_OPERATION_H
