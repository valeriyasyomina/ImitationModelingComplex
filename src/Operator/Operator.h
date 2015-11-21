#ifndef OPERATOR_H
#define OPERATOR_H

#include "../ProcessingUnit/ProcessingUnit.h"

class Operator : public ProcessingUnit
{
public:
    Operator(): ProcessingUnit(){}
    Operator(double a, double b) : ProcessingUnit(a, b) {}
    ~Operator() {}
};

#endif // OPERATOR_H
