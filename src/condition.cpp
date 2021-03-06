#include <json-c/json.h>
#include "condition.hpp"

bool LogicalCondition::evaluate() const
{
    bool toRet = false; // init with first condition!

    int nrConds = _condlist ? _condlist->size() : 0;

    if (nrConds==0) return toRet; // we return false for nullary!

    if (nrConds>=1) toRet = _condlist->front()->evaluate(); // for unary we ignore the operator

    if (nrConds>1){
        std::list<Condition *>::const_iterator it =_condlist->cbegin();
        ++it; // first value is already in toRet
        do{
            bool rhs = (*it)->evaluate();
//            printf("LogicalCondition::evaluate lhs=%s, rhs=%s\n",
//                   toRet ? "true" : "false", rhs? "true" : "false");
            toRet = _op(toRet, rhs);
            ++it;
        } while (it != _condlist->cend());

    }

    return toRet;
}
std::ostream &LogicalCondition::operator<<(std::ostream &os) const
{
    os << _opname << std::string(" ( ");
    auto it = _condlist->cbegin();
    for (; it!=_condlist->cend(); ++it){
        os << **it;
        os << std::string(" ");
    }

    os << std::string(")");
    return os;
}


bool UnaryCondition::evaluate() const
{
    bool toRet = false; // init with first condition!

    int nrConds = _condlist ? _condlist->size() : 0;

    if (nrConds==0) return toRet; // we return false for nullary!

    if (nrConds==1) return _op(_condlist->front()->evaluate());

    if (nrConds>1){
        throw "UnaryCondition::evaluate more than one cond!";
    }

    return toRet;
}

std::ostream &UnaryCondition::operator<<(std::ostream &os) const
{
    os << _opname << std::string(" ( ");
    auto it = _condlist->cbegin();
    for (; it!=_condlist->cend(); ++it){
        os << **it;
        os << std::string(" ");
    }

    os << std::string(")");
    return os;
}


bool BinaryComparison::evaluate() const
{
    double lhs = _lhs->value();
    double rhs = _rhs->value();
    return _op(lhs, rhs);
}

std::ostream &BinaryComparison::operator<<(std::ostream &os) const
{
    if (!_lhs || !_rhs) return os;
    os << *_lhs << _opname << *_rhs;
    return os;
}


double BinaryFct::value() const
{
    double lhs = _lhs->value();
    double rhs = _rhs->value();
    return _op(lhs, rhs);
}

std::ostream &operator<<(std::ostream &os, const Condition &c)
{
    return c.operator<<(os);
}

std::ostream &operator<<(std::ostream &os, const Value &v)
{
    return v.operator<<(os);
}

