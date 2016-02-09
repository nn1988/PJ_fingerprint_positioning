#ifndef COUTPUT_H
#define COUTPUT_H
#include "dataDef.h"
#include "DBoperate.h"
class COutPut
{
    public:
        COutPut();
        virtual ~COutPut();
    protected:
        bool OutPut(vector<CellMeanResult>* AllFinalResult, vector<Signatrue>* testSigList,vector<Signatrue>* TrainSigList);

    private:
};

#endif // COUTPUT_H
