#ifndef CONTROLPROGRAM_H
#define CONTROLPROGRAM_H

#include "../InformationSource/InformationSourceUniform.h"
#include "../Operator/Operator.h"
#include "../Computer/Computer.h"
#include "../Memory/Memory.h"
#include "../StatisticsBlock/StatisticsBlock.h"
#include <math.h>

const int ARRAY_SIZE = 6;

const int INFORMATION_SOURSE_INDEX = 0;
const int FIRST_OPERATOR_INDEX = 1;
const int SECOND_OPERATOR_INDEX = 2;
const int THIRD_OPERATOR_INDEX = 3;
const int FIRST_COMPUTER_INDEX = 4;
const int SECOND_COMPUTER_INDEX = 5;

const double EPSILON = 1e-3;
const double IDLE_TIME = -1.0;

const double FIRST_COMPUTER_PROCESSING_TIME = 15;
const double SECOND_COMPUTER_PROCESSING_TIME = 30;

class ControlProgram : public QObject
{
    Q_OBJECT
private:
    InformationSourceUniform* informationSource;   // источник информации
    ProcessingUnit* firstOperator;          // операторы
    ProcessingUnit* secondOperator;
    ProcessingUnit* thirdOperator;
    ProcessingUnit* firstComputer;          // ЭВМ
    ProcessingUnit* secondComputer;
    StatisticsBlock* statisticsBlock;       // блок обработки статистики
    Memory* firstStorage;                   // накопители
    Memory* secondStorage;
    double timeArray[ARRAY_SIZE];
    double currentModelingTime;
    int requestNumberToBeProcessed;
    int requestDropNumber;
    int requestProcessedNumber;
public:
    ControlProgram();
    ~ControlProgram();

    void ConfigureSystem(int maxMemorySize, double aFirstOperator,
                         double bFirstOperator, double aSecondOperator, double bSecondOperator,
                         double aThirdOperator, double bThirdOperator, double aInfSource, double bInfSouce,
                         int requestNumberToBeProcessed);
    void StartModeling();
private:
    double GetMinTime();
    void RealizeEvents();
    void CleanTimeArray();
    void FreeSystem();
public slots:
    void StatisticsCollected(double requestDropKoff);
signals:
    void StatisticsCollectedSignal(double requestDropKoff);
    void ModelingFinishedSignal();
};

#endif // CONTROLPROGRAM_H
