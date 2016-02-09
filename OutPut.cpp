#include "OutPut.h"

COutPut::COutPut()
{
    //ctor
}

COutPut::~COutPut()
{
    //dtor
}


bool COutPut::OutPut(string bufFile,CIrregularGrid* cTrainSigCor,
                     vector<CellMeanResult>* AllFinalResult, vector<Signatrue>* testSigList,vector<Signatrue>* TrainSigList)
{
    vector<CellMeanResult>::iterator iter3 = AllFinalResult.begin();
    vector<CellMeanResult>::iterator iter4 = AllFinalResult.end();

    CDBoperate SelectTestCoor;
    SelectTestCoor.OpenDB("FingerPrintTrainSet.db");

    CDBoperate SelectTrainCoor;
    SelectTrainCoor.OpenDB("FingerPrintTrainSet.db");

    UnitRect uni;


    vector<CellMeanResult> WriteToFileResult;// 取得可以写入文件的结果数据

    for (; iter3 != iter4; iter3++)
    {
        CellMeanResult cellResult1;// 存放数据

        GetCoor testSig; // 测试签名的坐标
        SelectTestCoor.GetCoordinateFromTable((*iter3).TestSigID, &testSig);
        cellResult1.TestSigX = testSig.TestSigX;
        cellResult1.TestSigY = testSig.TestSigY;

        vector<Signatrue>::iterator iters4 = TrainSigList->begin();
        vector<Signatrue>::iterator iters5 = TrainSigList->end();
        for (; iters4 != iters5; iters4++)
        {
            if ((*iter3).TrainSigID == (*iters4).nId)
            {
                uni = cTrainSigCor.GetRect((*iters4).nX, (*iters4).nY);

                cellResult1.TrainSigX = uni.centerX;
                cellResult1.TrainSigY = uni.centerY;
            }
        }

        GetCoor bestCoor; // 训练签名的坐标
        SelectTrainCoor.GetCoordinateFromTable((*iter3).BestCoorID, &bestCoor);
        cellResult1.BestSigX = bestCoor.TrainSigX;
        cellResult1.BestSigY = bestCoor.TrainSigY;

        cellResult1.TestSigID = (*iter3).TestSigID;
        cellResult1.TrainSigID = (*iter3).TrainSigID;

        cellResult1.MeanResult = (*iter3).MeanResult;

        cellResult1.BestCoorID = (*iter3).BestCoorID;
        cellResult1.BestDisResult = (*iter3).BestDisResult;

        WriteToFileResult.push_back(cellResult1);
    }


    /** 第2个输出，用来计算的结果**/
    char strCal[50];
    string bufCal;
    bufCal = "TestResultToCal";
    sprintf(strCal,"%s\\%s.txt",bufFile,bufCal.c_str());

    ofstream dataFileToCal;
    dataFileToCal.open(strCal, ios_base::out);
    if (!dataFileToCal)
    {
        cout << "Create false !" << endl;
        return false;
    }
    /** 第3个输出，排序的结果**/
    char strSort[50];
    string bufSort;
    bufSort = "TestResultSort";
    sprintf(strSort,"%s\\%s.txt",bufFile,bufSort.c_str());

    ofstream dataFileSort;
    dataFileSort.open(strSort, ios_base::out);
    if (!dataFileSort)
    {
        cout << "Create false !" << endl;
        return false;
    }

    vector<ResultSort> vecRS;

    vector<CellMeanResult>::iterator iter5 = WriteToFileResult.begin();
	vector<CellMeanResult>::iterator iter6 = WriteToFileResult.end();

	for (; iter5 != iter6; iter5++ )
    {
        ResultSort stuRS;
        double dbMean;
        simcal.CalDistanse((*iter5).TestSigX,
                        (*iter5).TestSigY,
                        (*iter5).TrainSigX,
                        (*iter5).TrainSigY,
                        &dbMean);

        // 计算机可以计算的
        dataFileToCal << (*iter5).BestDisResult << " ";
        dataFileToCal << dbMean << " ";
        dataFileToCal << endl;

        stuRS.BestCoorID = (*iter5).BestCoorID;
        stuRS.BestDisResult = (*iter5).BestDisResult;
        stuRS.MeanDis = dbMean;
        stuRS.MeanResult = (*iter5).MeanResult;
        stuRS.TestSigID = (*iter5).TestSigID;
        stuRS.TrainSigID = (*iter5).TrainSigID;
        vecRS.push_back(stuRS);

    }

    sort(vecRS.begin(), vecRS.end(), compare);

    vector<ResultSort>::iterator iter7 = vecRS.begin();
    vector<ResultSort>::iterator iter8 = vecRS.end();
    for (; iter7 != iter8; iter7++)
    {
        dataFileSort << "TID = " << (*iter7).TestSigID << " ";
        dataFileSort << "BID = " << (*iter7).BestCoorID << " ";
        dataFileSort << "MID = " << (*iter7).TrainSigID << " ";
        dataFileSort << "MCal = " << (*iter7).MeanResult << " ";
        dataFileSort << "BD = " << (*iter7).BestDisResult << " ";
        dataFileSort << "MeanD = " << (*iter7).MeanDis << " ";
        dataFileSort << endl;
    }

//    dataFile.close();
    dataFileToCal.close();
    dataFileSort.close();
}
