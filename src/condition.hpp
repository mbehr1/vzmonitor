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
};

class ConstCondition : public Condition
{
public:
    ConstCondition(bool value) : _value(value) {};
    virtual bool evaluate() const {return _value;}
    virtual ~ConstCondition() {};
protected:
    bool _value;
};

class LogicalCondition : public Condition
{
public:
    LogicalCondition( std::function<bool(const bool &, const bool &)> op,
                      std::list<Condition *> *condlist) :
                       _op(op), _condlist(condlist) {};
    virtual bool evaluate() const;
    virtual ~LogicalCondition(){ if (_condlist) delete _condlist;}

protected:
    std::function<bool(const bool &, const bool &)> _op;
    std::list<Condition *> *_condlist;
};

class UnaryCondition : public Condition
{
public:
    UnaryCondition( std::function<bool(const bool &)> op,
                      std::list<Condition *> *condlist) : _op(op), _condlist(condlist)
        { if (!condlist || condlist->size()!=1) throw "UnaryCondition: not one condition!";};
    virtual bool evaluate() const;
    virtual ~UnaryCondition(){ if (_condlist) delete _condlist;}

protected:
    std::function<bool(const bool &)> _op;
    std::list<Condition *> *_condlist;
};

class Value
{
public:
    virtual ~Value(){};
    virtual double value() const = 0;
};

class ConstValue : public Value
{
public:
    ConstValue(const double &val) : _value(val) {};
    virtual double value() const {return _value;};
    virtual ~ConstValue() {};
protected:
    double _value;
};

class NullAryFct : public Value
{
public:
    NullAryFct(std::function<double()> fct) : _fct(fct) {};
    virtual double value() const {return _fct();};
    virtual ~NullAryFct() {};
protected:
    std::function<double()> _fct;
};

class UnaryFct : public Value
{
public:
    UnaryFct(std::function<double(const std::string &)> op,
              const char *p1) : _op(op), _p1(p1)  { if (!_p1.length()) throw "UnaryFct: parameter missing";};
    virtual double value() const {return _op(_p1);};
    virtual ~UnaryFct() {};
protected:
    std::function<double(const std::string &)> _op;
    std::string _p1;
};


class BinaryFct : public Value
{
public:
    BinaryFct(std::function<double(const double &, const double &)> op,
              Value *lhs, Value *rhs) : _op(op), _lhs(lhs), _rhs(rhs)  { if (!_lhs || !_rhs) throw "BinaryFct: lhs/rhs missing!";};
    virtual double value() const;
    virtual ~BinaryFct() {};
protected:
    std::function<double(const double &, const double &)> _op;
    Value *_lhs, *_rhs;
};

class BinaryComparison : public Condition
{
public:
    BinaryComparison( std::function<bool(const double &, const double &)> op,
                      Value *lhs, Value *rhs) : _op(op), _lhs(lhs), _rhs(rhs) { if (!_lhs || !_rhs) throw "BinaryComparision: lhs/rhs missing!";};
    virtual bool evaluate() const;
    virtual ~BinaryComparison() {delete _lhs; delete _rhs;}
protected:
    std::function<bool(const double &, const double &)> _op;
    Value *_lhs, *_rhs;
};

#endif
