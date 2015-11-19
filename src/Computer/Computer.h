#ifndef COMPUTER_H
#define COMPUTER_H

#include "../ProcessingUnit/ProcessingUnit.h"

class Computer : public ProcessingUnit
{
protected:
    double processTime;
public:
    Computer(): ProcessingUnit() {}
    Computer(double time) : ProcessingUnit(), processTime(time) {}
    Computer(double a, double b) : ProcessingUnit(a, b) {}
    ~Computer() {}

    double GetProcessTime() {return processTime;}
};

#endif // COMPUTER_H
