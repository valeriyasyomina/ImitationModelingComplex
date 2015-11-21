#include "ControlProgram.h"

ControlProgram::ControlProgram()
{
    informationSource = NULL;

    firstOperator = NULL;
    secondOperator = NULL;
    thirdOperator = NULL;

    firstComputer = NULL;
    secondComputer = NULL;

    firstStorage = NULL;
    secondStorage = NULL;

    statisticsBlock = NULL;

    currentModelingTime = 0.0;

    requestNumberToBeProcessed = 0;
    requestDropNumber = 0;
    requestProcessedNumber = 0;

    CleanTimeArray();
}

ControlProgram::~ControlProgram()
{
    FreeSystem();
}

void ControlProgram::ConfigureSystem(int maxMemorySize, double aFirstOperator,
                                     double bFirstOperator, double aSecondOperator, double bSecondOperator,
                                     double aThirdOperator, double bThirdOperator, double aInfSource, double bInfSouce,
                                     int requestNumberToBeProcessed)
{
    try
    {
        if (maxMemorySize <= 0)
            throw ErrorInputDataException("Error input parameters in ControlProgram::ConfigureSystem");

        FreeSystem();

        informationSource = new InformationSourceUniform(aInfSource, bInfSouce);

        firstOperator = new Operator(aFirstOperator, bFirstOperator);
        secondOperator = new Operator(aSecondOperator, bSecondOperator);
        thirdOperator = new Operator(aThirdOperator, bThirdOperator);

        statisticsBlock = new StatisticsBlock();

        firstStorage = new Memory(maxMemorySize);
        secondStorage = new Memory(maxMemorySize);

        firstComputer = new Computer(FIRST_COMPUTER_PROCESSING_TIME);
        secondComputer = new Computer(SECOND_COMPUTER_PROCESSING_TIME);

        connect(statisticsBlock, SIGNAL(CollectStatisticsSignal(double)), this,
                SLOT(StatisticsCollected(double)));

        this->requestNumberToBeProcessed = requestNumberToBeProcessed;
        requestProcessedNumber = 0;
        requestDropNumber = 0;

        timeArray[INFORMATION_SOURSE_INDEX] = informationSource->GetProcessTime();
        timeArray[FIRST_OPERATOR_INDEX] = IDLE_TIME;
        timeArray[SECOND_OPERATOR_INDEX] = IDLE_TIME;
        timeArray[THIRD_OPERATOR_INDEX] = IDLE_TIME;
        timeArray[FIRST_COMPUTER_INDEX] = IDLE_TIME;
        timeArray[SECOND_COMPUTER_INDEX] = IDLE_TIME;
    }
    catch (std::bad_alloc& exception)
    {
        throw AllocMemoryException("Erro allocate memory in ControlProgram::ConfigureSystem");
    }
}

void ControlProgram::StartModeling()
{
    while (requestProcessedNumber < requestNumberToBeProcessed)
    {
        currentModelingTime = GetMinTime();
        RealizeEvents();
    }  
    emit ModelingFinishedSignal();
    statisticsBlock->CollectStatistics(requestProcessedNumber, requestDropNumber);
}

double ControlProgram::GetMinTime()
{
    double minTime = timeArray[0];
    for (int i = 1; i < ARRAY_SIZE; i++)
    {
        if (timeArray[i] < minTime && timeArray[i] != IDLE_TIME)
            minTime = timeArray[i];
    }
    return minTime;
}
void ControlProgram::RealizeEvents()
{
    if (fabs(timeArray[INFORMATION_SOURSE_INDEX] - currentModelingTime) < EPSILON)
    {
        if (!firstOperator->HasAssignedRequest())
            firstOperator->SetRequestAssigned(true);
        else if (!secondOperator->HasAssignedRequest())
            secondOperator->SetRequestAssigned(true);
        else if (!thirdOperator->HasAssignedRequest())
            thirdOperator->SetRequestAssigned(true);
        else
            requestDropNumber++;

        timeArray[INFORMATION_SOURSE_INDEX] = informationSource->GetProcessTime() + currentModelingTime;
    }

    if (fabs(timeArray[FIRST_OPERATOR_INDEX] - currentModelingTime) < EPSILON &&
                firstOperator->HasAssignedRequest())
    {
        firstOperator->SetRequestAssigned(false);
        Request request(currentModelingTime);
        firstStorage->PutRequest(request);
        timeArray[FIRST_OPERATOR_INDEX] = IDLE_TIME;
    }
    if (fabs(timeArray[SECOND_OPERATOR_INDEX] - currentModelingTime) < EPSILON &&
                secondOperator->HasAssignedRequest())
    {
        secondOperator->SetRequestAssigned(false);
        Request request(currentModelingTime);
        firstStorage->PutRequest(request);
        timeArray[SECOND_OPERATOR_INDEX] = IDLE_TIME;
    }
    if (fabs(timeArray[THIRD_OPERATOR_INDEX] - currentModelingTime) < EPSILON &&
                thirdOperator->HasAssignedRequest())
    {
        thirdOperator->SetRequestAssigned(false);
        Request request(currentModelingTime);
        secondStorage->PutRequest(request);
        timeArray[THIRD_OPERATOR_INDEX] = IDLE_TIME;
    }

    if (firstOperator->HasAssignedRequest() && fabs(timeArray[FIRST_OPERATOR_INDEX] - IDLE_TIME) < EPSILON)
        timeArray[FIRST_OPERATOR_INDEX] = firstOperator->GetProcessTime() + currentModelingTime;
    if (secondOperator->HasAssignedRequest() && fabs(timeArray[SECOND_OPERATOR_INDEX] - IDLE_TIME) < EPSILON)
        timeArray[SECOND_OPERATOR_INDEX] = secondOperator->GetProcessTime() + currentModelingTime;
    if (thirdOperator->HasAssignedRequest() && fabs(timeArray[THIRD_OPERATOR_INDEX] - IDLE_TIME) < EPSILON)
        timeArray[THIRD_OPERATOR_INDEX] = thirdOperator->GetProcessTime() + currentModelingTime;

    if (fabs(timeArray[FIRST_COMPUTER_INDEX] - currentModelingTime) < EPSILON &&
                firstComputer->HasAssignedRequest())
    {
        firstComputer->SetRequestAssigned(false);
        requestProcessedNumber++;
        timeArray[FIRST_COMPUTER_INDEX] = IDLE_TIME;
    }
    if (fabs(timeArray[SECOND_COMPUTER_INDEX] - currentModelingTime) < EPSILON &&
                secondComputer->HasAssignedRequest())
    {
        secondComputer->SetRequestAssigned(false);
        requestProcessedNumber++;
        timeArray[SECOND_COMPUTER_INDEX] = IDLE_TIME;
    }

    if (!firstStorage->isEmpty() && fabs(timeArray[FIRST_COMPUTER_INDEX] - IDLE_TIME) < EPSILON)
    {
        Request request = firstStorage->GetRequest();
        firstComputer->SetRequestAssigned(true);
        timeArray[FIRST_COMPUTER_INDEX] = firstComputer->GetProcessTime() + currentModelingTime;
    }

    if (!secondStorage->isEmpty() && fabs(timeArray[SECOND_COMPUTER_INDEX] - IDLE_TIME) < EPSILON)
    {
         Request request = secondStorage->GetRequest();
         secondComputer->SetRequestAssigned(true);
         timeArray[SECOND_COMPUTER_INDEX] = secondComputer->GetProcessTime() + currentModelingTime;
    }
}

void ControlProgram::CleanTimeArray()
{
    for (int i = 0; i < ARRAY_SIZE; i++)
        timeArray[i] = 0.0;
}

void ControlProgram::FreeSystem()
{
    if (informationSource)
    {
        delete informationSource;
        informationSource = NULL;
    }

    if (firstOperator)
    {
        delete firstOperator;
        firstOperator = NULL;
    }
    if (secondOperator)
    {
        delete secondOperator;
        secondOperator = NULL;
    }
    if (thirdOperator)
    {
        delete thirdOperator;
        thirdOperator = NULL;
    }

    if (firstComputer)
    {
        delete firstComputer;
        firstComputer = NULL;
    }
    if (secondComputer)
    {
        delete secondComputer;
        secondComputer = NULL;
    }

    if (firstStorage)
    {
        delete firstStorage;
        firstStorage = NULL;
    }
    if (secondStorage)
    {
        delete secondStorage;
        secondStorage = NULL;
    }

    if (statisticsBlock)
    {
        delete statisticsBlock;
        statisticsBlock = NULL;
    }
}

void ControlProgram::StatisticsCollected(double requestDropKoff)
{
    emit StatisticsCollectedSignal(requestDropKoff);
}

