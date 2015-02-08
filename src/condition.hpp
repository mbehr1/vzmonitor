#ifndef __condition_hpp_
#define __condition_hpp_

#include <string>
#include <list>
#include <functional>

class Condition
{
public:
//    Condition() {}
    virtual ~Condition(){};
    virtual bool evaluate() const =0;
    virtual std::ostream &operator<<(std::ostream &os) const =0;

};

extern std::ostream &operator<<(std::ostream &os, const Condition &c);

class ConstCondition : public Condition
{
public:
    ConstCondition(bool value) : _value(value) {};
    virtual bool evaluate() const {return _value;}
    virtual ~ConstCondition() {};
    virtual std::ostream &operator<<(std::ostream &os) const {os << std::string(_value ? "true " : "false "); return os;};
protected:
    bool _value;
};

class LogicalCondition : public Condition
{
public:
    LogicalCondition( const char *opname, std::function<bool(const bool &, const bool &)> op,
                      std::list<Condition *> *condlist) :
                       _opname(opname), _op(op), _condlist(condlist) {};
    virtual bool evaluate() const;
    virtual ~LogicalCondition(){ if (_condlist) delete _condlist;}
    virtual std::ostream &operator<<(std::ostream &os) const;
protected:
    std::string _opname;
    std::function<bool(const bool &, const bool &)> _op;
    std::list<Condition *> *_condlist;
};

class UnaryCondition : public Condition
{
public:
    UnaryCondition( const char *opname, std::function<bool(const bool &)> op,
                      std::list<Condition *> *condlist) : _opname(opname), _op(op), _condlist(condlist)
        { if (!condlist || condlist->size()!=1) throw "UnaryCondition: not one condition!";};
    virtual bool evaluate() const;
    virtual ~UnaryCondition(){ if (_condlist) delete _condlist;}
    virtual std::ostream &operator<<(std::ostream &os) const;
protected:
    std::string _opname;
    std::function<bool(const bool &)> _op;
    std::list<Condition *> *_condlist;
};

class Value
{
public:
    virtual ~Value(){};
    virtual double value() const = 0;
    virtual std::ostream &operator<<(std::ostream &os) const=0;
};

extern std::ostream &operator<<(std::ostream &os, const Value &v);


class ConstValue : public Value
{
public:
    ConstValue(const double &val) : _value(val) {};
    virtual double value() const {return _value;};
    virtual ~ConstValue() {};
    virtual std::ostream &operator<<(std::ostream &os) const {os << std::to_string(_value); return os;};
protected:
    double _value;
};

class NullAryFct : public Value
{
public:
    NullAryFct(const char *opname, std::function<double()> fct) : _opname(opname), _fct(fct) {};
    virtual double value() const {return _fct();};
    virtual ~NullAryFct() {};
    virtual std::ostream &operator<<(std::ostream &os) const {os << _opname << std::string("()"); return os;};
protected:
    std::string _opname;
    std::function<double()> _fct;
};

class UnaryFct : public Value
{
public:
    UnaryFct(const char *opname, std::function<double(const std::string &)> op,
              const char *p1) : _opname(opname), _op(op), _p1(p1)  { if (!_p1.length()) throw "UnaryFct: parameter missing";};
    virtual double value() const {return _op(_p1);};
    virtual ~UnaryFct() {};
    virtual std::ostream &operator<<(std::ostream &os) const {os << _opname << std::string("(") << _p1 << std::string(")"); return os;};
protected:
    std::string _opname;
    std::function<double(const std::string &)> _op;
    std::string _p1;
};


class BinaryFct : public Value
{
public:
    BinaryFct(const char *opname, std::function<double(const double &, const double &)> op,
              Value *lhs, Value *rhs) : _opname(opname), _op(op), _lhs(lhs), _rhs(rhs)  { if (!_lhs || !_rhs) throw "BinaryFct: lhs/rhs missing!";};
    virtual double value() const;
    virtual ~BinaryFct() {};
    virtual std::ostream &operator<<(std::ostream &os) const {os << std::string("(") << *_lhs << _opname << *_rhs << std::string(")"); return os;};
protected:
    std::string _opname;
    std::function<double(const double &, const double &)> _op;
    Value *_lhs, *_rhs;
};

class BinaryComparison : public Condition
{
public:
    BinaryComparison( const char *opname, std::function<bool(const double &, const double &)> op,
                      Value *lhs, Value *rhs) : _opname(opname), _op(op), _lhs(lhs), _rhs(rhs) { if (!_lhs || !_rhs) throw "BinaryComparision: lhs/rhs missing!";};
    virtual bool evaluate() const;
    virtual ~BinaryComparison() {delete _lhs; delete _rhs;}
    virtual std::ostream &operator<<(std::ostream &os) const;
protected:
    std::string _opname;
    std::function<bool(const double &, const double &)> _op;
    Value *_lhs, *_rhs;
};

#endif
