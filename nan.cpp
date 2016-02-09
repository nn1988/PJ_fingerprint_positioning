// NAN.cpp : Defines the entry point for the console application.

#include "SigCreate.h"
#include "Cov.h"
#include "FpCreate.h"
#include "DBoperate.h"
#include "SimCal.h"
#include "IrregularGrid.h"
#include   <stdlib.h>
#include   <fstream>
#include   <iostream>

using namespace std;

#if 0
void TestCov()
{
	ifstream fPos("Tmp.txt");
	if(!fPos)
		return ;

	string s;
	//�ȼ��������е��ֻ�	GetPhoneID();
	vector<vector<double> > ve;
	while( getline(fPos,s) )
	{
		std::vector<std::string> strList;
		strList = split(s,"\"");
		vector<double> v;
		//ת���ַ���
		double db = atof(strList.at(0).c_str());
		v.push_back(db);
		//ʱ��ת��
		db = atof(strList.at(1).c_str());
		v.push_back(db);
		//����ת��
		db= atof(strList.at(2).c_str());
		v.push_back(db);
		db = atof(strList.at(3).c_str());
		v.push_back(db);
		ve.push_back(v);
	}

	CCov cov;
	cov.SetData(&ve);
	int nCol,nRaw;
	bool b = cov.Calculate(&nCol,&nRaw);
	fPos.close();
}
#endif // 0

void TestDBInsert()
{
    CSigCreate sig;
//	TestCov();

	if(sig.CreateSigStruct())
		cout << "true" << endl;
	else
		cout << "false" << endl;

// ���Ե�ͼģ�飬�����ݿ����ǩ������һ����ͼ��
    CMatrix matmap;
    matmap.SetMapID(1);
    matmap.SetRange(3000.0, 3000.0, 8000.0, 10000.0);
    matmap.SetSize(40, 40);

	PhoneList* phlist = sig.GetPhoneList();
	CFpCreate fp(phlist);

//#if 0

// �������ݿ�����в��������
	vector<PhoneInfo>::iterator iter1 = phlist->phoneInfoList.begin();
	vector<PhoneInfo>::iterator iter2 = phlist->phoneInfoList.end();
	for (; iter1 != iter2; iter1++)
    {
        int nPhoneID = (*iter1).nPhoneID;

        Signatrue tmp;
        fp.GetSig(nPhoneID,&tmp);//ֻ��һ��ǩ��

        CDBoperate dbope;
        dbope.SaveSignature(&tmp);
        dbope.OpenDB("FingerPrintTestSet.db");
        dbope.CreateOneFingerPrint(matmap);
        dbope.CloseDB();

    }
}

void TestDBSelect()
{
    CDBoperate dbope;
    dbope.OpenDB("FingerPrint.db");
    dbope.SelectFingerPrint(0,10,1);

//    vector<Signatrue> sigList;
//    sigList = *(dbope.GetSignatrue());

    dbope.CloseDB();
    //����ȡ����ǩ���Ƿ���ȷ��ȡ����һ��ǩ���ĵ�һ����վ������rsrp��
    vector<stuMean> vecmean;
    vecmean = (dbope.GetSignatrue()->at(0)).vecMean;
    vector<double> rsrp;
    rsrp = (vecmean.at(0)).rsrp;
    cout << rsrp.at(0) << endl;
    cout << rsrp.at(1) << endl;
    cout << rsrp.at(2) << endl;
    cout << rsrp.at(3) << endl;
    cout << rsrp.at(4) << endl;
}

void TestSimCal()
{

// �������ƶȼ���ģ�鹦��
    CSigCreate sig;
	sig.CreateSigStruct();

    CMatrix matmap;
    matmap.SetMapID(1);
    matmap.SetRange(3000.0, 3000.0, 8000.0, 10000.0);
    matmap.SetSize(40, 40);

	PhoneList* phlist = sig.GetPhoneList();
	CFpCreate fp(phlist);
    Signatrue tmp;
    fp.GetSig(217,&tmp);

    CDBoperate dbope;
    dbope.OpenDB("FingerPrint.db");

    dbope.SelectFingerPrint(0,10, 1);
    CSimCal simcal(dbope.GetSignatrue());
//    simcal.KldSimilarityCal(&tmp);
    dbope.CloseDB();

}

void TestIrregula()
{
    CIrregularGrid CIrre ;//����һ����ͼ�����óߴ�
    CIrre.SetRange(1000.0, 2600.0, 1000, 865.0);
    CIrre.SetRectSize(10,20,40);
    CIrre.SetBSSize(500);
    CIrre.SetLveSize(3,3,1); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
    UnitRect uni;
    uni = CIrre.GetRect(1340, 1300,3);
//    CIrre.GetCorOfAllGrid();

    cout << "raw = " << uni.raw << endl;
    cout << "col = " << uni.col << endl;
    cout << "left = " << uni.left << endl;
    cout << "right = " << uni.right << endl;
    cout << "bottom = " << uni.bottom << endl;
    cout << "top = " << uni.top << endl;
    cout << "centerX = " << uni.centerX << endl;
    cout << "centerY = " << uni.centerY << endl;
    cout << "realX = " << uni.realX << endl;
    cout << "realY = " << uni.realY << endl;

}

void InsertData()
{
	CSigCreate sig;
	sig.CreateSigStruct();//��ԭʼ���ݶ��������浽�ڴ����档

    CIrregularGrid CIrre ;//����һ����ͼ�����óߴ�
    CIrre.SetMapID(1);
    CIrre.SetRange(1000.0, 2600.0, 2000.0, 865.0);
    CIrre.SetRectSize(5,10,20);
    CIrre.SetBSSize(500);


	PhoneList* phlist = sig.GetPhoneList();//���ڴ��������������ǩ�����ɺ���
	CFpCreate fp(phlist);



// �������ݿ�Ĳ��룬���ڴ����������ǩ���������ݿ�.
	vector<PhoneInfo>::iterator iter1 = phlist->phoneInfoList.begin();
	vector<PhoneInfo>::iterator iter2 = phlist->phoneInfoList.end();
	for (; iter1 != iter2; iter1++)
    {


        int nPhoneID = (*iter1).nPhoneID;
        Signatrue tmp;
        fp.GetSig(nPhoneID,&tmp);//ֻ��һ��ǩ��

        CDBoperate dbope;
        dbope.SaveSignature(&tmp);
        dbope.OpenDB("FingerPrintTrainSet.db");
        dbope.CreateOneFingerPrint(CIrre);
        dbope.CloseDB();
    }
}

bool CalMain()
{
    int nBeginSigFromTest        = 1080 ; // ȡ����ǩ����λ��
    int nSelectTestSig           = 120 ; // ȡ���ٸ�����ǩ��
    int nBeginSigFromTrain       = 0 ; // ȡѵ��ǩ����λ��
    int nSelectTrainSig          = 1080 ; // �����ȶԵ�ѵ������С��

    // �Ӳ��Լ�����ȡ��ǩ��
    CDBoperate dbopeSelectTest;
    dbopeSelectTest.OpenDB("FingerPrintTrainSet.db");
    cout << "Start to Select \"Test\" FingerPrint " << endl;
    dbopeSelectTest.SelectFingerPrint(nBeginSigFromTest,nSelectTestSig,1);
    // ��Ϊ���Լ�����ֻ��200��ǩ������ȡ����Ȼ��ȥ�ȶԡ�

    vector<Signatrue> testSigList;
    testSigList = *(dbopeSelectTest.GetSignatrue());
    // testSigList�����ŴӲ��Լ�����ȡ����������ǩ����

    dbopeSelectTest.CloseDB();

// ��ѵ��������ȡǩ������ʼ���㣬ȫ��ȡ������
    CDBoperate dbopeSelectTrain;

    dbopeSelectTrain.OpenDB("FingerPrintTrainSet.db");

//    int nTrainSig = dbopeSelectTrain.CountSig();  // ���һ��ȫȡ��������֪������
    cout << "Start to Select \"Train\" FingerPrint " << endl;

    dbopeSelectTrain.SelectFingerPrint(nBeginSigFromTrain, nSelectTrainSig, 0);
    vector<Signatrue>* TrainSigList;
    TrainSigList = dbopeSelectTrain.GetSignatrue();

    dbopeSelectTrain.CloseDB();

    // ��ʼ�������ƶ�
    CSimCal simcal(TrainSigList);

    vector<FianlSigResult> AllFinalResult;// 200��ǩ��ÿ��ͬ1200��ǩ���ȶԺ����Сֵ
    vector<Signatrue>::iterator iter1 = testSigList.begin();
    vector<Signatrue>::iterator iter2 = testSigList.end();
    for (; iter1 != iter2; iter1++)
    {
        vector<TmpResult> simKLDResult;
        vector<TmpResult> simMaDResult;
        simcal.SimilarityCal(&(*iter1), &simKLDResult,&simMaDResult);

        TmpResult theKLDMin;
        TmpResult theMaDMin;
        simcal.GetMin(&simKLDResult, &theKLDMin);// �ֱ���㲻ͬ�㷨�õ�����Сֵ��
        simcal.GetMin(&simMaDResult, &theMaDMin);

//*********************************************************************
        BestCoor best;
        simcal.CalBestCoor(&(*iter1), &best);// ������ѵ�ǩ���;��붼�Ѿ��������
//*********************************************************************
//        cout << "KLDID = " << theKLDMin.KldID << " KLD = " << theKLDMin.KldResult << endl;
//        cout << "MaDID = " << theMaDMin.MaDID << " MaD = " << theMaDMin.MaDResult << endl;


        cout << "Start to calculate minKey SigID = " << (*iter1).nId << endl;

        FianlSigResult fsr;
        fsr.KldResult = theKLDMin.CalResult;
        fsr.MaDResult = theMaDMin.CalResult;
        fsr.TrainKldSigID = theKLDMin.SigID;
        fsr.TrainMaDSigID = theMaDMin.SigID;
        fsr.TestSigID = (*iter1).nId;
        fsr.BestDisResult = best.CalDisResult;
        fsr.BestCoorID = best.BestID;
        AllFinalResult.push_back(fsr);
    }

// �Ӳ��Լ���ѵ��������ȡ������ֵ
    vector<FianlSigResult>::iterator iter3 = AllFinalResult.begin();
    vector<FianlSigResult>::iterator iter4 = AllFinalResult.end();

    CDBoperate SelectTestCoor;
    SelectTestCoor.OpenDB("FingerPrintTrainSet.db");

    CDBoperate SelectTrainCoor;
    SelectTrainCoor.OpenDB("FingerPrintTrainSet.db");

    vector<FianlSigResult> WriteToFileResult;// ȡ�ÿ���д���ļ��Ľ������

    for (; iter3 != iter4; iter3++)
    {
        FianlSigResult fsr1;// �������

        GetCoor testSig;
        SelectTestCoor.GetCoordinateFromTable((*iter3).TestSigID, &testSig);
        fsr1.TestSigCoordinateX = testSig.TestSigX;
        fsr1.TestSigCoordinateY = testSig.TestSigY;

        GetCoor trainKlDSig;
        SelectTrainCoor.GetCoordinateFromTable((*iter3).TrainKldSigID, &trainKlDSig);
        fsr1.TrainSigKldX = trainKlDSig.TrainSigX;
        fsr1.TrainSigKldY = trainKlDSig.TrainSigY;

        GetCoor trainMaDSig;
        SelectTrainCoor.GetCoordinateFromTable((*iter3).TrainMaDSigID, &trainMaDSig);
        fsr1.TrainSigMaDX = trainMaDSig.TrainSigX;
        fsr1.TrainSigMaDY = trainMaDSig.TrainSigY;

        GetCoor bestCoor;
        SelectTrainCoor.GetCoordinateFromTable((*iter3).TrainKldSigID, &bestCoor);
        fsr1.BestSigX = bestCoor.BestSigX;
        fsr1.BestSigY = bestCoor.BestSigY;

        fsr1.TestSigID = (*iter3).TestSigID;
        fsr1.TrainKldSigID = (*iter3).TrainKldSigID;
        fsr1.TrainMaDSigID = (*iter3).TrainMaDSigID;

        fsr1.KldResult = (*iter3).KldResult;
        fsr1.MaDResult = (*iter3).MaDResult;
        fsr1.BestCoorID = (*iter3).BestCoorID;
        fsr1.BestDisResult = (*iter3).BestDisResult;

        WriteToFileResult.push_back(fsr1);
    }

// ����������txt�ļ�
    // ���˿��Ľ���ļ�
    char str[50];
    string buf;
    buf = "TestResult";
    sprintf(str,"Result\\%s.txt",buf.c_str());

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    // ���������Ľ���ļ�
    char strCal[50];
    string bufCal;
    bufCal = "TestResultToCal";
    sprintf(strCal,"Result\\%s.txt",bufCal.c_str());

    ofstream dataFileToCal;
    dataFileToCal.open(strCal, ios_base::out);
    if (!dataFileToCal)
    {
        cout << "Create false !" << endl;
        return false;
    }

    vector<FianlSigResult>::iterator iter5 = WriteToFileResult.begin();
	vector<FianlSigResult>::iterator iter6 = WriteToFileResult.end();

	for (; iter5 != iter6; iter5++ )
    {
        double dbKld;
        double dbMaD;
        simcal.CalDistanse((*iter5).TestSigCoordinateX,
                        (*iter5).TestSigCoordinateY,
                        (*iter5).TrainSigKldX,
                        (*iter5).TrainSigKldY,
                        &dbKld);
        simcal.CalDistanse((*iter5).TestSigCoordinateX,
                        (*iter5).TestSigCoordinateY,
                        (*iter5).TrainSigMaDX,
                        (*iter5).TrainSigMaDY,
                        &dbMaD);

        dataFile << "TID = " << (*iter5).TestSigID << " ";
        dataFile << "BID = " << (*iter5).BestCoorID << " ";
        dataFile << "KID = " << (*iter5).TrainKldSigID << " ";
        dataFile << "MID = " << (*iter5).TrainMaDSigID << " ";
        dataFile << "B(X,Y) = (" << (*iter5).BestSigX << ",";
        dataFile << (*iter5).BestSigY << ") ";
        dataFile << "T(X,Y) = (" << (*iter5).TestSigCoordinateX << ",";
        dataFile << (*iter5).TestSigCoordinateY << ") ";
        dataFile << "K(X,Y) = (" << (*iter5).TrainSigKldX << ",";
        dataFile << (*iter5).TrainSigKldY << ") ";
        dataFile << "M(X,Y) = (" << (*iter5).TrainSigMaDX << ",";
        dataFile << (*iter5).TrainSigMaDX << ") ";
        dataFile << "KCal = " << (*iter5).KldResult << " ";
        dataFile << "MCal = " << (*iter5).MaDResult << " ";
        dataFile << "BD = " << (*iter5).BestDisResult << " ";
        dataFile << "KD = " << dbKld << " ";
        dataFile << "MD = " << dbMaD ;
        dataFile << endl;

        // ��������Լ����
        dataFileToCal << (*iter5).BestDisResult << " ";
        dataFileToCal << dbKld << " ";
        dataFileToCal << dbMaD ;
        dataFileToCal << endl;



    }

    dataFile.close();
    dataFileToCal.close();


}

bool compare(const ResultSort& t1, const ResultSort& t2)
{
    return t1.MeanDis < t2.MeanDis;
}

bool CellMeanCal()
{
    int nBeginSigFromTest        = 0 ; // ȡ����ǩ����λ��
    int nSelectTestSig           = 300  ; // ȡ���ٸ�����ǩ��
    int nBeginSigFromTrain       = 300 ; // ȡѵ��ǩ����λ��
    int nSelectTrainSig          = 2700 ; // �����ȶԵ�ѵ������С��

/*********** 1 �Ӳ��Լ���ѵ��������ȡǩ��************/
    CDBoperate dbopeSelectTest;
    dbopeSelectTest.OpenDB("FingerPrintTrainSet.db");
    dbopeSelectTest.SelectFingerPrint(nBeginSigFromTest,nSelectTestSig, 1);
    vector<Signatrue> testSigList;
    testSigList = *(dbopeSelectTest.GetSignatrue());
    dbopeSelectTest.CloseDB();

    CDBoperate dbopeSelectTrain;
    dbopeSelectTrain.OpenDB("FingerPrintTrainSet.db");
//    int nTrainSig = dbopeSelectTrain.CountSig();  // ���һ��ȫȡ��������֪������
    dbopeSelectTrain.SelectFingerPrint(nBeginSigFromTrain, nSelectTrainSig, 0);
    vector<Signatrue>* TrainSigList;
    TrainSigList = dbopeSelectTrain.GetSignatrue();
    dbopeSelectTrain.CloseDB();

/*********** 2 �������ƶ�************/
    CSimCal simcal(TrainSigList); // ����CSimCal�Ĺ��캯������ʼ��Ҫ��ѵ��ǩ����ȥ

    int nEmpty = 0;
    vector<CellMeanResult> AllFinalResult;
    vector<Signatrue>::iterator iter1 = testSigList.begin();
    vector<Signatrue>::iterator iter2 = testSigList.end();
    for (; iter1 != iter2; iter1++)
    {
        vector<TmpResult> vecSum;
//        simcal.SimCellIDCal(&(*iter1),&vecSum);  // ������GetMin
        simcal.MeanCellIDCal(&(*iter1),&vecSum); // ������GetMin


//        bool nTag = 1;
//        nTag = simcal.CellIDCompareCal(&(*iter1),&vecSum); // ������GetMax
//        if ( nTag == 0)
//        {
//            nEmpty++;
//            cout << "nEmpty = " << nEmpty << endl;
//            system("pause");
//        }



//        simcal.VarCellIDCal(&(*iter1),&vecSum); // �����GetVarMin
//        simcal.PearsonCal(&(*iter1),&vecSum);  // �����GetMax
//        simcal.WeightingCal(&(*iter1),&vecSum);  // ������GetMin
//        simcal.EuclideanCal(&(*iter1),&vecSum); // ������GetMin
//        simcal.KNNCal(&(*iter1),&vecSum); // ������KNNv2

        TmpResult theCellMeanMin;
        simcal.GetMin(&vecSum, &theCellMeanMin);// �ֱ���㲻ͬ�㷨�õ�����Сֵ��
//        simcal.GetMax(&vecSum, &theCellMeanMin);// �ֱ���㲻ͬ�㷨�õ������ֵ��
//
//        int nThresholdVar = 5; // ��ֵ��ĸ���.
//        simcal.GetVarMin(&vecSum, &theCellMeanMin, nThresholdVar);// �����˷������Сֵ��



        BestCoor best;
        simcal.CalBestCoor(&(*iter1), &best);// ������ѵ�ǩ���;��붼�Ѿ��������

        CellMeanResult cellResult;
        cellResult.TestSigID = (*iter1).nId;
        cellResult.TrainSigID = theCellMeanMin.SigID;
        cellResult.MeanResult = theCellMeanMin.CalResult;
        cellResult.BestCoorID = best.BestID;
        cellResult.BestDisResult = best.CalDisResult;

        AllFinalResult.push_back(cellResult);
    }

// �Ӳ��Լ���ѵ��������ȡ������ֵ
    vector<CellMeanResult>::iterator iter3 = AllFinalResult.begin();
    vector<CellMeanResult>::iterator iter4 = AllFinalResult.end();

    CDBoperate SelectTestCoor;
    SelectTestCoor.OpenDB("FingerPrintTrainSet.db");

    CDBoperate SelectTrainCoor;
    SelectTrainCoor.OpenDB("FingerPrintTrainSet.db");

    vector<CellMeanResult> WriteToFileResult;// ȡ�ÿ���д���ļ��Ľ������

    for (; iter3 != iter4; iter3++)
    {
        CellMeanResult cellResult1;// �������

        GetCoor testSig; // ����ǩ��������
        SelectTestCoor.GetCoordinateFromTable((*iter3).TestSigID, &testSig);
        cellResult1.TestSigX = testSig.TestSigX;
        cellResult1.TestSigY = testSig.TestSigY;

        vector<Signatrue>::iterator iters4 = TrainSigList->begin();
        vector<Signatrue>::iterator iters5 = TrainSigList->end();
        for (; iters4 != iters5; iters4++)
        {
            if ((*iter3).TrainSigID == (*iters4).nId)
            {
                cellResult1.TrainSigX = (*iters4).nX;
                cellResult1.TrainSigY = (*iters4).nY;
            }
        }

        GetCoor bestCoor; // ѵ��ǩ��������
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

    vector<Signatrue> vecUR1;
    UnitRect stuTmpUR;
    vector<Signatrue>::iterator iteru1 = TrainSigList->begin();
    vector<Signatrue>::iterator iteru2 = TrainSigList->end();
    for (int i = 0; iteru1 != iteru2; iteru1++)
    {
        if (i == 0)
        {
            stuTmpUR.centerX = (*iteru1).nX;
            stuTmpUR.centerY = (*iteru1).nY;
            vecUR1.push_back((*iteru1));
        }
        else
        {
            if ((*iteru1).nX != stuTmpUR.centerX &&
                (*iteru1).nY != stuTmpUR.centerY)
            {
                stuTmpUR.centerX = (*iteru1).nX;
                stuTmpUR.centerY = (*iteru1).nY;
                vecUR1.push_back((*iteru1));
            }
        }
        i++;
    }

    int nTotalNum = vecUR1.size();
    cout << "nTotalNum = " << nTotalNum << endl;

// ����������txt�ļ�
//    /** ��һ�������δ����Ľ��**/
//    char str[50];
//    string buf;
//    buf = "TestResult";
//    sprintf(str,"MeanReasult\\%s.txt",buf.c_str());
//
//    ofstream dataFile;
//    dataFile.open(str, ios_base::out);
//    if (!dataFile)
//    {
//        cout << "Create false !" << endl;
//        return false;
//    }


    /** ��2���������������Ľ��**/
    char strCal[50];
    string bufCal;
    bufCal = "TestResultToCal";
    sprintf(strCal,"MeanReasult\\%s.txt",bufCal.c_str());

    ofstream dataFileToCal;
    dataFileToCal.open(strCal, ios_base::out);
    if (!dataFileToCal)
    {
        cout << "Create false !" << endl;
        return false;
    }
    /** ��3�����������Ľ��**/
    char strSort[50];
    string bufSort;
    bufSort = "TestResultSort";
    sprintf(strSort,"MeanReasult\\%s.txt",bufSort.c_str());

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

//        dataFile << "TID = " << (*iter5).TestSigID << " ";
//        dataFile << "BID = " << (*iter5).BestCoorID << " ";
//        dataFile << "MID = " << (*iter5).TrainSigID << " ";
//        dataFile << "B(X,Y) = (" << (*iter5).BestSigX << ",";
//        dataFile << (*iter5).BestSigY << ") ";
//        dataFile << "T(X,Y) = (" << (*iter5).TestSigX << ",";
//        dataFile << (*iter5).TestSigY << ") ";
//        dataFile << "M(X,Y) = (" << (*iter5).TrainSigX << ",";
//        dataFile << (*iter5).TrainSigY << ") ";
//        dataFile << "MCal = " << (*iter5).MeanResult << " ";
//        dataFile << "BD = " << (*iter5).BestDisResult << " ";
//        dataFile << "MeanD = " << dbMean << " ";
//        dataFile << endl;

        // ��������Լ����
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

bool CellMeanCalv2()
{
    int nBeginSigFromTest        = 0 ; // ȡ����ǩ����λ��
    int nSelectTestSig           = 300  ; // ȡ���ٸ�����ǩ��
    int nBeginSigFromTrain       = 300 ; // ȡѵ��ǩ����λ��
    int nSelectTrainSig          = 2700 ; // �����ȶԵ�ѵ������С��

/*********** 1 �Ӳ��Լ���ѵ��������ȡǩ��************/
    CDBoperate dbopeSelectTest;
    dbopeSelectTest.OpenDB("FingerPrintTrainSet.db");
    dbopeSelectTest.SelectFingerPrint(nBeginSigFromTest,nSelectTestSig, 1);
    vector<Signatrue> testSigList;
    testSigList = *(dbopeSelectTest.GetSignatrue());
    dbopeSelectTest.CloseDB();

    CDBoperate dbopeSelectTrain;
    dbopeSelectTrain.OpenDB("FingerPrintTrainSet.db");
//    int nTrainSig = dbopeSelectTrain.CountSig();  // ���һ��ȫȡ��������֪������
    dbopeSelectTrain.SelectFingerPrint(nBeginSigFromTrain, nSelectTrainSig, 1);
    vector<Signatrue>* TrainSigList;
    TrainSigList = dbopeSelectTrain.GetSignatrue();
    dbopeSelectTrain.CloseDB();

/*********** 2 �������ƶ�************/
    CSimCal simcal(TrainSigList); // ����CSimCal�Ĺ��캯������ʼ��Ҫ��ѵ��ǩ����ȥ

    int nEmpty = 0;
    vector<CellMeanResult> AllFinalResult;
    vector<Signatrue>::iterator iter1 = testSigList.begin();
    vector<Signatrue>::iterator iter2 = testSigList.end();
    for (; iter1 != iter2; iter1++)
    {
        vector<TmpResult> vecSum;
//        simcal.SimCellIDCal(&(*iter1),&vecSum);  // ������GetMin
        simcal.MeanCellIDCal(&(*iter1),&vecSum); // ������GetMin


//        bool nTag = 1;
//        nTag = simcal.CellIDCompareCal(&(*iter1),&vecSum); // ������GetMax
//        if ( nTag == 0)
//        {
//            nEmpty++;
//            cout << "nEmpty = " << nEmpty << endl;
//            system("pause");
//        }



//        simcal.VarCellIDCal(&(*iter1),&vecSum); // �����GetVarMin
//        simcal.PearsonCal(&(*iter1),&vecSum);  // �����GetMax
//        simcal.WeightingCal(&(*iter1),&vecSum);  // ������GetMin
//        simcal.EuclideanCal(&(*iter1),&vecSum); // ������GetMin


        TmpResult theCellMeanMin;
        simcal.GetMin(&vecSum, &theCellMeanMin);// �ֱ���㲻ͬ�㷨�õ�����Сֵ��
//        simcal.GetMax(&vecSum, &theCellMeanMin);// �ֱ���㲻ͬ�㷨�õ������ֵ��
//
//        int nThresholdVar = 5; // ��ֵ��ĸ���.
//        simcal.GetVarMin(&vecSum, &theCellMeanMin, nThresholdVar);// �����˷������Сֵ��


        BestCoor best;
        simcal.CalBestCoor(&(*iter1), &best);// ������ѵ�ǩ���;��붼�Ѿ��������

        CellMeanResult cellResult;
        cellResult.TestSigID = (*iter1).nId;
        cellResult.TrainSigID = theCellMeanMin.SigID;
        cellResult.MeanResult = theCellMeanMin.CalResult;
        cellResult.BestCoorID = best.BestID;
        cellResult.BestDisResult = best.CalDisResult;

        AllFinalResult.push_back(cellResult);
    }


// �Ӳ��Լ���ѵ��������ȡ������ֵ
    vector<CellMeanResult>::iterator iter3 = AllFinalResult.begin();
    vector<CellMeanResult>::iterator iter4 = AllFinalResult.end();

    CDBoperate SelectTestCoor;
    SelectTestCoor.OpenDB("FingerPrintTrainSet.db");

    CDBoperate SelectTrainCoor;
    SelectTrainCoor.OpenDB("FingerPrintTrainSet.db");

    CIrregularGrid cTrainSigCor;
    cTrainSigCor.SetRange(1000.0, 2600.0, 1000, 865.0);
    cTrainSigCor.SetRectSize(10,20,40);
    cTrainSigCor.SetBSSize(500);
    cTrainSigCor.SetLveSize(2,2,0); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
    UnitRect uni;


    vector<CellMeanResult> WriteToFileResult;// ȡ�ÿ���д���ļ��Ľ������

    for (; iter3 != iter4; iter3++)
    {
        CellMeanResult cellResult1;// �������

        GetCoor testSig; // ����ǩ��������
        SelectTestCoor.GetCoordinateFromTable((*iter3).TestSigID, &testSig);
        cellResult1.TestSigX = testSig.TestSigX;
        cellResult1.TestSigY = testSig.TestSigY;

        vector<Signatrue>::iterator iters4 = TrainSigList->begin();
        vector<Signatrue>::iterator iters5 = TrainSigList->end();
        for (; iters4 != iters5; iters4++)
        {
            if ((*iter3).TrainSigID == (*iters4).nId)
            {
                uni = cTrainSigCor.GetRect((*iters4).nX, (*iters4).nY,3);

                cellResult1.TrainSigX = uni.centerX;
                cellResult1.TrainSigY = uni.centerY;
            }
        }

        GetCoor bestCoor; // ѵ��ǩ��������
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

// ����������txt�ļ�
//    /** ��һ�������δ����Ľ��**/
//    char str[50];
//    string buf;
//    buf = "TestResult";
//    sprintf(str,"MeanReasult\\%s.txt",buf.c_str());
//
//    ofstream dataFile;
//    dataFile.open(str, ios_base::out);
//    if (!dataFile)
//    {
//        cout << "Create false !" << endl;
//        return false;
//    }


    /** ��2���������������Ľ��**/
    char strCal[50];
    string bufCal;
    bufCal = "TestResultToCal";
    sprintf(strCal,"MeanReasult\\%s.txt",bufCal.c_str());

    ofstream dataFileToCal;
    dataFileToCal.open(strCal, ios_base::out);
    if (!dataFileToCal)
    {
        cout << "Create false !" << endl;
        return false;
    }
    /** ��3�����������Ľ��**/
    char strSort[50];
    string bufSort;
    bufSort = "TestResultSort";
    sprintf(strSort,"MeanReasult\\%s.txt",bufSort.c_str());

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

//        dataFile << "TID = " << (*iter5).TestSigID << " ";
//        dataFile << "BID = " << (*iter5).BestCoorID << " ";
//        dataFile << "MID = " << (*iter5).TrainSigID << " ";
//        dataFile << "B(X,Y) = (" << (*iter5).BestSigX << ",";
//        dataFile << (*iter5).BestSigY << ") ";
//        dataFile << "T(X,Y) = (" << (*iter5).TestSigX << ",";
//        dataFile << (*iter5).TestSigY << ") ";
//        dataFile << "M(X,Y) = (" << (*iter5).TrainSigX << ",";
//        dataFile << (*iter5).TrainSigY << ") ";
//        dataFile << "MCal = " << (*iter5).MeanResult << " ";
//        dataFile << "BD = " << (*iter5).BestDisResult << " ";
//        dataFile << "MeanD = " << dbMean << " ";
//        dataFile << endl;

        // ��������Լ����
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

bool OutPut(string bufFile,
            CIrregularGrid* cTrainSigCor,
            CSimCal* simcal,
            vector<CellMeanResult>* AllFinalResult,
            vector<Signatrue>* testSigList,
            vector<Signatrue>* TrainSigList,
            int nTag)
{
    vector<CellMeanResult>::iterator iter3 = AllFinalResult->begin();
    vector<CellMeanResult>::iterator iter4 = AllFinalResult->end();
    CDBoperate SelectTestCoor;
    SelectTestCoor.OpenDB("FingerPrintTrainSet.db");
//
//    CDBoperate SelectTrainCoor;
//    SelectTrainCoor.OpenDB("FingerPrintTrainSet.db");
    UnitRect uni;
    vector<CellMeanResult> WriteToFileResult;// ȡ�ÿ���д���ļ��Ľ������

    for (; iter3 != iter4; iter3++)
    {
        CellMeanResult cellResult1;// �������

        GetCoor testSig; // ����ǩ��������
        SelectTestCoor.GetCoordinateFromTable((*iter3).TestSigID, &testSig);
        cellResult1.TestSigX = testSig.TestSigX;
        cellResult1.TestSigY = testSig.TestSigY;


        vector<Signatrue>::iterator iters4 = TrainSigList->begin();
        vector<Signatrue>::iterator iters5 = TrainSigList->end();
        for (; iters4 != iters5; iters4++)
        {
            if ((*iter3).BestCoorID == (*iters4).nId)
            {
                UnitRect stuUR;
                cellResult1.BestCoorID = (*iter3).BestCoorID;
                stuUR = cTrainSigCor->GetRect((*iters4).nX, (*iters4).nY,nTag);
                double dbBest = (stuUR.centerX - (*iters4).nX) * (stuUR.centerX - (*iters4).nX) + \
                (stuUR.centerY - (*iters4).nY) * (stuUR.centerY - (*iters4).nY);
                cellResult1.BestDisResult = sqrt(dbBest);
            }
//            UnitRect bestUR;
//            bestUR = cTrainSigCor->GetRect
            if ((*iter3).TrainSigID == (*iters4).nId)
            {
                uni = cTrainSigCor->GetRect((*iters4).nX, (*iters4).nY,nTag);

                cellResult1.TrainSigX = uni.centerX;
                cellResult1.TrainSigY = uni.centerY;
            }
        }

//        GetCoor bestCoor; // ѵ��ǩ��������
//        SelectTrainCoor.GetCoordinateFromTable((*iter3).BestCoorID, &bestCoor);
//        UnitRect bestFinal;
//        bestFinal = cTrainSigCor->GetRect(bestCoor.TestSigX, bestCoor.BestSigY, nTag);
//        cellResult1.BestSigX = bestCoor.TrainSigX;
//        cellResult1.BestSigY = bestCoor.TrainSigY;

        cellResult1.TestSigID = (*iter3).TestSigID;
        cellResult1.TrainSigID = (*iter3).TrainSigID;

        cellResult1.MeanResult = (*iter3).MeanResult;

//        cellResult1.BestCoorID = (*iter3).BestCoorID;
//        cellResult1.BestDisResult = (*iter3).BestDisResult;

        WriteToFileResult.push_back(cellResult1);
    }
    vector<UnitRect> vecUni;
    vector<Signatrue>::iterator iters4 = TrainSigList->begin();
    vector<Signatrue>::iterator iters5 = TrainSigList->end();
    for (; iters4 != iters5; iters4++)
    {
        UnitRect uni;
        uni = cTrainSigCor->GetRect((*iters4).nX, (*iters4).nY,nTag);
        vecUni.push_back(uni);
    }
    vector<UnitRect> vecUR1;
    UnitRect stuTmpUR;
    vector<UnitRect>::iterator iteru1 = vecUni.begin();
    vector<UnitRect>::iterator iteru2 = vecUni.end();
    for (int i = 0; iteru1 != iteru2; iteru1++)
    {
        if (i == 0)
        {
            stuTmpUR.centerX = (*iteru1).centerX;
            stuTmpUR.centerY = (*iteru1).centerY;
            vecUR1.push_back((*iteru1));
        }
        else
        {
            if ((*iteru1).centerX != stuTmpUR.centerX &&
                (*iteru1).centerY != stuTmpUR.centerY)
            {
                stuTmpUR.centerX = (*iteru1).centerX;
                stuTmpUR.centerY = (*iteru1).centerY;
                vecUR1.push_back((*iteru1));
            }
        }
        i++;
    }
    int nAllNum = vecUni.size();
    int nTotalNum = vecUR1.size();
    cout << bufFile << " " << "nAllNum = " << nAllNum;
    cout << " " << "nTotalNum = " << nTotalNum << endl;

    /** ��2���������������Ľ��**/
    char strCal[50];
    string bufCal;
    bufCal = "TestResultToCal";
    sprintf(strCal,"%s\\%s.txt",bufFile.c_str(),bufCal.c_str());

    ofstream dataFileToCal;
    dataFileToCal.open(strCal, ios_base::out);
    if (!dataFileToCal)
    {
        cout << "Create false !" << endl;
        return false;
    }
    /** ��3�����������Ľ��**/
    char strSort[50];
    string bufSort;
    bufSort = "TestResultSort";
    sprintf(strSort,"%s\\%s.txt",bufFile.c_str(),bufSort.c_str());

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
        simcal->CalDistanse((*iter5).TestSigX,
                        (*iter5).TestSigY,
                        (*iter5).TrainSigX,
                        (*iter5).TrainSigY,
                        &dbMean);

        // ��������Լ����
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

    dataFileToCal.close();
    dataFileSort.close();
}


bool CellMeanCalv3()
{
    int nBeginSigFromTest        = 0 ; // ȡ����ǩ����λ��
    int nSelectTestSig           = 300  ; // ȡ���ٸ�����ǩ��
    int nBeginSigFromTrain       = 300 ; // ȡѵ��ǩ����λ��
    int nSelectTrainSig          = 2700 ; // �����ȶԵ�ѵ������С��

/*********** 1 �Ӳ��Լ���ѵ��������ȡǩ��************/
    CDBoperate dbopeSelectTest;
    dbopeSelectTest.OpenDB("FingerPrintTrainSet.db");
    dbopeSelectTest.SelectFingerPrint(nBeginSigFromTest,nSelectTestSig, 1);
    vector<Signatrue> testSigList;
    testSigList = *(dbopeSelectTest.GetSignatrue());
    dbopeSelectTest.CloseDB();

    CDBoperate dbopeSelectTrain;
    dbopeSelectTrain.OpenDB("FingerPrintTrainSet.db");
//    int nTrainSig = dbopeSelectTrain.CountSig();  // ���һ��ȫȡ��������֪������
    dbopeSelectTrain.SelectFingerPrint(nBeginSigFromTrain, nSelectTrainSig, 1);
    vector<Signatrue>* TrainSigList;
    TrainSigList = dbopeSelectTrain.GetSignatrue();
    dbopeSelectTrain.CloseDB();

/*********** 2 �������ƶ�************/
    CSimCal simcal(TrainSigList); // ����CSimCal�Ĺ��캯������ʼ��Ҫ��ѵ��ǩ����ȥ

    int nEmpty = 0;
    vector<CellMeanResult> AllFinalResult;
    vector<Signatrue>::iterator iter1 = testSigList.begin();
    vector<Signatrue>::iterator iter2 = testSigList.end();
    for (; iter1 != iter2; iter1++)
    {
        vector<TmpResult> vecSum;
//        simcal.SimCellIDCal(&(*iter1),&vecSum);  // ������GetMin
        simcal.MeanCellIDCal(&(*iter1),&vecSum); // ������GetMin

        TmpResult theCellMeanMin;
        simcal.GetMin(&vecSum, &theCellMeanMin);// �ֱ���㲻ͬ�㷨�õ�����Сֵ��

        BestCoor best;
        simcal.CalBestCoor(&(*iter1), &best);// ������ѵ�ǩ���;��붼�Ѿ��������

        CellMeanResult cellResult;
        cellResult.TestSigID = (*iter1).nId;
        cellResult.TrainSigID = theCellMeanMin.SigID;
        cellResult.MeanResult = theCellMeanMin.CalResult;
        cellResult.BestCoorID = best.BestID;
        cellResult.BestDisResult = best.CalDisResult;

        AllFinalResult.push_back(cellResult);
    }
    //
    CIrregularGrid cTrainSigCor;
    cTrainSigCor.SetRange(1000.0, 2600.0, 1000, 865.0);
    cTrainSigCor.SetRectSize(10,20,40);
    cTrainSigCor.SetBSSize(500);

//    CIrregularGrid cTrainSigCor2;
//    cTrainSigCor2.SetRange(1000.0, 2600.0, 1000, 865.0);
//    cTrainSigCor2.GetRectSize(5,10,20);
//    cTrainSigCor2.SetBSSize(500);
//    cTrainSigCor2.SetLveSize(1,1,0); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
//
//    CIrregularGrid cTrainSigCor3;
//    cTrainSigCor3.SetRange(1000.0, 2600.0, 1000, 865.0);
//    cTrainSigCor3.GetRectSize(5,10,20);
//    cTrainSigCor3.SetBSSize(500);
//    cTrainSigCor3.SetLveSize(2,2,0); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
//
//    CIrregularGrid cTrainSigCor4;
//    cTrainSigCor4.SetRange(1000.0, 2600.0, 1000, 865.0);
//    cTrainSigCor4.GetRectSize(5,10,20);
//    cTrainSigCor4.SetBSSize(500);
//    cTrainSigCor4.SetLveSize(3,3,0); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
//
//    CIrregularGrid cTrainSigCor5;
//    cTrainSigCor5.SetRange(1000.0, 2600.0, 1000, 865.0);
//    cTrainSigCor5.GetRectSize(5,10,20);
//    cTrainSigCor5.SetBSSize(500);
//    cTrainSigCor5.SetLveSize(4,4,0); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
    int nLve = 3;
    cTrainSigCor.SetLveSize(1,1,1); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
    OutPut("--1",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(1,1,0);
//    OutPut("1-1-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(2,2,0);
//    OutPut("2-2-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(3,3,0);
//    OutPut("3-3-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(4,4,0);
//    OutPut("4-4-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(5,5,0);
//    OutPut("5-5-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(6,6,0);
//    OutPut("6-6-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(7,7,0);
//    OutPut("7-7-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(8,8,0);
//    OutPut("8-8-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(9,9,0);
//    OutPut("9-9-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);
//    cTrainSigCor.SetLveSize(10,10,0);
//    OutPut("10-10-0",&cTrainSigCor,&simcal,&AllFinalResult,&testSigList,TrainSigList,nLve);


}

bool KNNCal()
{
    int nBeginSigFromTest        = 0 ; // ȡ����ǩ����λ��
    int nSelectTestSig           = 120  ; // ȡ���ٸ�����ǩ��
    int nBeginSigFromTrain       = 120 ; // ȡѵ��ǩ����λ��
    int nSelectTrainSig          = 1080 ; // �����ȶԵ�ѵ������С��

/*********** 1 �Ӳ��Լ���ѵ��������ȡǩ��************/
    CDBoperate dbopeSelectTest;
    dbopeSelectTest.OpenDB("FingerPrintTrainSet.db");
    dbopeSelectTest.SelectFingerPrint(nBeginSigFromTest,nSelectTestSig, 1);
    vector<Signatrue> testSigList;
    testSigList = *(dbopeSelectTest.GetSignatrue());
    dbopeSelectTest.CloseDB();

    CDBoperate dbopeSelectTrain;
    dbopeSelectTrain.OpenDB("FingerPrintTrainSet.db");
//    int nTrainSig = dbopeSelectTrain.CountSig();  // ���һ��ȫȡ��������֪������
    dbopeSelectTrain.SelectFingerPrint(nBeginSigFromTrain, nSelectTrainSig, 0);
    vector<Signatrue>* TrainSigList;
    TrainSigList = dbopeSelectTrain.GetSignatrue();
    dbopeSelectTrain.CloseDB();

/*********** 2 �������ƶ�************/
    CSimCal simcal(TrainSigList); // ����CSimCal�Ĺ��캯������ʼ��Ҫ��ѵ��ǩ����ȥ


    vector<CellMeanResult> AllFinalResult;
    vector<Signatrue>::iterator iter1 = testSigList.begin();
    vector<Signatrue>::iterator iter2 = testSigList.end();
    for (; iter1 != iter2; iter1++)
    {
        vector<TmpResult> vecSum;

//        simcal.SimCellIDCal(&(*iter1),&vecSum);
        simcal.MeanCellIDCal(&(*iter1),&vecSum);
//        simcal.KNNCal(&(*iter1),&vecSum);
//
        KNNCor kNNcor;
        simcal.KNNv2(5,&vecSum,&kNNcor);
        BestCoor best;
        simcal.CalBestCoor(&(*iter1), &best);// ������ѵ�ǩ���;��붼�Ѿ��������


        CellMeanResult cellResult;
        cellResult.TestSigID = (*iter1).nId;
        cellResult.BestCoorID = best.BestID;
        cellResult.BestDisResult = best.CalDisResult;
        cellResult.TrainSigX = kNNcor.dbX;
        cellResult.TrainSigY = kNNcor.dbY;

        AllFinalResult.push_back(cellResult);
    }

// �Ӳ��Լ���ѵ��������ȡ������ֵ
    vector<CellMeanResult>::iterator iter3 = AllFinalResult.begin();
    vector<CellMeanResult>::iterator iter4 = AllFinalResult.end();

    CDBoperate SelectTestCoor;
    SelectTestCoor.OpenDB("FingerPrintTrainSet.db");

    CDBoperate SelectTrainCoor;
    SelectTrainCoor.OpenDB("FingerPrintTrainSet.db");

    vector<CellMeanResult> WriteToFileResult;// ȡ�ÿ���д���ļ��Ľ������

    for (; iter3 != iter4; iter3++)
    {
        CellMeanResult cellResult1;// �������

        GetCoor testSig; // ����ǩ��������
        SelectTestCoor.GetCoordinateFromTable((*iter3).TestSigID, &testSig);
        cellResult1.TestSigX = testSig.TestSigX;
        cellResult1.TestSigY = testSig.TestSigY;

        GetCoor bestCoor; // ѵ��ǩ��������
        SelectTrainCoor.GetCoordinateFromTable((*iter3).BestCoorID, &bestCoor);
        cellResult1.BestSigX = bestCoor.TrainSigX;
        cellResult1.BestSigY = bestCoor.TrainSigY;

        cellResult1.TestSigID = (*iter3).TestSigID;
        cellResult1.TrainSigID = (*iter3).TrainSigID;
        cellResult1.BestCoorID = (*iter3).BestCoorID;
        cellResult1.BestDisResult = (*iter3).BestDisResult;
        cellResult1.TrainSigX = (*iter3).TrainSigX;
        cellResult1.TrainSigY = (*iter3).TrainSigY;

        WriteToFileResult.push_back(cellResult1);
    }

    /** ��2���������������Ľ��**/
    char strCal[50];
    string bufCal;
    bufCal = "TestResultToCal";
    sprintf(strCal,"MeanReasult\\%s.txt",bufCal.c_str());

    ofstream dataFileToCal;
    dataFileToCal.open(strCal, ios_base::out);
    if (!dataFileToCal)
    {
        cout << "Create false !" << endl;
        return false;
    }
    /** ��3�����������Ľ��**/
    char strSort[50];
    string bufSort;
    bufSort = "TestResultSort";
    sprintf(strSort,"MeanReasult\\%s.txt",bufSort.c_str());

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

        // ��������Լ����
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

    dataFileToCal.close();
    dataFileSort.close();

}

//*******************************************************************
//*******************************************************************
//**************************��ʼ������*******************************
//*******************************************************************

int main(int argc, char* argv[])
{

//*****************************������***********************

//    TestCov();
//    TestDBInsert();
//    TestDBSelect();
//    TestSimCal();

//*****************************������***********************

//	CSigCreate sig;
//	sig.CreateSigStruct();//��ԭʼ���ݶ��������浽�ڴ����档
//
//    CMatrix matmap;//����һ����ͼ�����óߴ�
//    matmap.SetMapID(1);
//    matmap.SetRange(1000.0, 2600.0, 1000, 865);
//    matmap.SetSize(40, 40);
//
//	PhoneList* phlist = sig.GetPhoneList();//���ڴ��������������ǩ�����ɺ���
//	CFpCreate fp(phlist);
//
//
//
//// �������ݿ�Ĳ��룬���ڴ����������ǩ���������ݿ�.
//	vector<PhoneInfo>::iterator iter1 = phlist->phoneInfoList.begin();
//	vector<PhoneInfo>::iterator iter2 = phlist->phoneInfoList.end();
//	for (; iter1 != iter2; iter1++)
//    {
//
//
//        int nPhoneID = (*iter1).nPhoneID;
//        Signatrue tmp;
//        fp.GetSig(nPhoneID,&tmp);//ֻ��һ��ǩ��
//
//        CDBoperate dbope;
//        dbope.SaveSignature(&tmp);
//        dbope.OpenDB("FingerPrintTrainSet.db");
//        dbope.CreateOneFingerPrint(matmap);
//        dbope.CloseDB();
//    }

//    CalMain();
//    CellMeanCal();
//    TestIrregula();
//    InsertData();
//    CellMeanCalv2();
    CellMeanCalv3();
//    KNNCal();
	return 0;
}


