#include "SimCal.h"
#include "DBoperate.h"
#include "Matrix.h"
#include "Cov.h"

#include <cmath>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <objbase.h>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace Eigen;

CSimCal::CSimCal(vector<Signatrue>* pSigInfoList)
{
    m_SigInfoList = pSigInfoList;
}

CSimCal::~CSimCal()
{
    m_SigInfoList = 0;
}

bool CSimCal::SimilarityCal(Signatrue* pSig1,
                            vector<TmpResult>* pKLDResult,
                            vector<TmpResult>* pMaDResult)//���������ǲ���ǩ��
{

    cout << "Start to calculate TestSigID = " << pSig1->nId << endl ;

    char str[50];

    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig1->nId
      );
    sprintf(str,"middle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }

    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();

    for (; iter1 != iter2; iter1++)
    {
        Signatrue UsableTestSig;
        Signatrue UsableTrainSig;

        bool  bIs = CompareDiff(pSig1, &(*iter1), &UsableTestSig, &UsableTrainSig);
        if (!bIs)
        {
            continue;
        }

        CalFormat cfTest;
        CalFormat cfTrain;

        ConverSig(&UsableTestSig, &cfTest);
        ConverSig(&UsableTrainSig, &cfTrain);

        // �����������
        VectorXd vcTest_Train;
        vcTest_Train = (cfTest.vecMean_Ut) - (cfTrain.vecMean_Ut);

        // �������ת��_.transpose()
        RowVectorXd RvcTest_TrainTp;
        RvcTest_TrainTp = vcTest_Train.transpose();

        // �������
        MatrixXd trainCovIv;
        trainCovIv = (cfTrain.matCov).inverse();

        double td = (cfTrain.matCov).determinant();

//        MatrixXi trainCovIvi;
//        trainCovIvi = trainCovIv.cast<int>();��
//        cout << "SigID = " << UsableTrainSig.nId << endl;
//        cout << "determinant = " << td << endl;
//        cout << "Matrix Cov = " << endl;
//        cout << cfTrain.matCov << endl;
//        cout << "Matrix InverseCov = " << endl;
//        cout << trainCovIv << endl;
//        cout << "float Matrix = " << endl;
//        cout << trainCovIvf << endl;
//        system("pause");


        // ���Ծ�����ѵ�����������ˣ���
        MatrixXd mtTestTrainMp;
        mtTestTrainMp = cfTest.matCov * (cfTrain.matCov).inverse();
        int nMtSize = mtTestTrainMp.rows();

        // �õ���λ����
        MatrixXd mtIdentity;
        mtIdentity.setIdentity(nMtSize,nMtSize);
        // �ټ�ȥ��λ����
        MatrixXd mtTestTrainMp_I;
        mtTestTrainMp_I = (mtTestTrainMp - mtIdentity.setIdentity(nMtSize,nMtSize));

// ����MaD���ƶȣ�prod�ľ������Ϊ��ֵ���������Ϊ��ֵ����Ӱ��KLD�㷨

//****************MaD�㷨 start********************
        double prod;
        prod = RvcTest_TrainTp * trainCovIv * vcTest_Train;
        if ( prod >= 0)
//        if (true)
        {
//            prod = fabs(prod);// ��prodȡ����ֵ��
            TmpResult sigreMaD;
            long double MaDKey =  sqrt(prod);
            sigreMaD.CalResult = MaDKey;
            sigreMaD.SigID = (*iter1).nId;
            pMaDResult->push_back(sigreMaD);
            cout << "         " ;
            cout << "CalculateMaD TrainSigID = " << (*iter1).nId ;
            cout << " TestSigID = " << pSig1->nId << " Down! " << endl;
        }
        else
        {
            cout << "         " ;
            cout << "no count MaD!" << endl;
        }

//****************MaD�㷨 end**********************
        double mtTrace;
        mtTrace = mtTestTrainMp_I.trace();// �����ļ�
//********************KLD�㷨 start**********************************
        double mtdeter = (float)mtTestTrainMp.determinant();// ����������ʽ
//        if (true)
        if (mtdeter >= 0)
        {
            TmpResult sigreKLD;
//            mtdeter = fabs(mtdeter); // ��mtdeterȡ����ֵ��
            double dblog = log(mtdeter);

            double KldKey = (prod + mtTrace - dblog)/2;
//            double KldKey = (prod + mtTrace)/2;

            sigreKLD.CalResult = KldKey;
            sigreKLD.SigID = (*iter1).nId;
            pKLDResult->push_back(sigreKLD);

            dataFile << (*iter1).nId << " " << "dbProd=";
            dataFile << prod << " " << "dbMtTrace=";
            dataFile << mtTrace << " " << "dblog=";
            dataFile << dblog  << " " << "KldKey=";
            dataFile << KldKey << endl;

            cout << "         " ;
            cout << "CalculateKLD TrainSigID = " << (*iter1).nId ;
            cout << " TestSigID = " << pSig1->nId << " Down! " << endl;
        }
        else
        {
            cout << "         " ;
            cout << "no count KLD!" << endl;
        }
//*********************KLD�㷨 end*********************************
    }
    dataFile.close();
}

bool CSimCal::ConverSig(Signatrue* sig, CalFormat* cf)
{
    vector<stuMean>::iterator iter1 = sig->vecMean.begin();
    vector<stuMean>::iterator iter2 = sig->vecMean.end();

    int nCellID = sig->vecMean.size();

    // ת����վ�ź;�ֵ��Ϊ������
    VectorXi vecCellID_It(nCellID);
    VectorXd vecMean_Ut(nCellID);

    for (int i =0; iter1 != iter2; iter1++,i++)
    {
        vecCellID_It[i] = (*iter1).neNB;
        vecMean_Ut[i] = (*iter1).dbMean;
    }
    vector<vector<double> >::iterator iter3 = sig->vecCov.begin();
    vector<vector<double> >::iterator iter4 = sig->vecCov.end();

    int nCov = sig->vecCov.size();

    MatrixXd matCov(nCov, nCov);

    for (int i = 0; iter3 != iter4; iter3++, i++)
    {
        vector<double> data = (*iter3);
        vector<double>::iterator iter5 = data.begin();
        vector<double>::iterator iter6 = data.end();

        for (int j = 0; iter5 != iter6; iter5++, j++)
        {
            // ��vector�����ֵ��ת�����������棬ת��Э�������
            matCov(i,j) = (float)(*iter5);
        }

    }
    cf->matCov         = matCov;
    cf->vecCellID_It   = vecCellID_It;
    cf->vecMean_Ut     = vecMean_Ut;
    return true;
}

bool CSimCal::CompareDiff(Signatrue* pTest, Signatrue* pTrain,
                          Signatrue* pUsableTestSig,
                          Signatrue* pUsableTrainSig)
{
/*
// �㷨�Ľ���һ�ֽ������
    Signatrue* pTestSig;
    Signatrue* pTrainSig;

    if ((pTest->vecMean.size()) == (pTrain->vecMean.size()))
    {
        bool bIsMacth = CheckCellID(pTest, pTrain);

        if (bIsMacth)
        {
            pTestSig = pTest;
            pTrainSig = pTrain;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

*/

    Signatrue* pTestSig;
    Signatrue* pTrainSig;
    pTestSig = pTest;
    pTrainSig = pTrain;

    vector<stuMean>::iterator iter1 = pTestSig->vecMean.begin();
    vector<stuMean>::iterator iter2 = pTestSig->vecMean.end();

    vector<stuMean> vecStuTestMean;
    vector<stuMean> vecStuTrainMean;

    vector<vector<double> > comVecTsetRsrp;
    vector<vector<double> > comVecTrainRsrp;


    for (; iter1 != iter2; iter1++)
    {
        vector<stuMean>::iterator iter3 = pTrainSig->vecMean.begin();
        vector<stuMean>::iterator iter4 = pTrainSig->vecMean.end();

        for (; iter3 != iter4; iter3++)
        {
            if ((*iter1).neNB == (*iter3).neNB)
            {
                stuMean intTestMean;
                intTestMean.neNB    = (*iter1).neNB;
                intTestMean.dbMean  = (*iter1).dbMean;
                vecStuTestMean.push_back(intTestMean);

                stuMean intTrainMean;
                intTrainMean.neNB   = (*iter1).neNB;//���Լ���ѵ�����Թ���һ����վ�š�
                intTrainMean.dbMean = (*iter3).dbMean;
                vecStuTrainMean.push_back(intTrainMean);

                comVecTsetRsrp.push_back( (*iter1).rsrp);
                comVecTrainRsrp.push_back((*iter3).rsrp);
            }
        }
    }
    if (vecStuTestMean.size() == 0 || vecStuTrainMean.size() == 0)
    {
        return false;
    }

/**************************�鿴comVecTsetRsrp start************************/
/*    vector<vector<double> >::iterator iter5 = comVecTsetRsrp.begin();
    cout << "rsrp Matrix = " << endl;
    for (; iter5 != comVecTsetRsrp.end(); iter5++ )
    {
        vector<double>::iterator iter6 = (*iter5).begin();
        for (; iter6 != (*iter5).end(); iter6++)
        {
            cout << (*iter6) << " " ;
        }
        cout << endl;
    }
*/
/**************************�鿴comVecTsetRsrp end**************************/
    CCov covTsetRsrp;
	covTsetRsrp.SetData(&comVecTsetRsrp);

	int nRaw = 0,nCol = 0;
	if(!covTsetRsrp.Calculate(&nRaw,&nCol))
		return false;

	vector<vector<double> >* pTestCov = covTsetRsrp.GetResult();

    CCov covTrainRsrp;
	covTrainRsrp.SetData(&comVecTrainRsrp);
	if(!covTrainRsrp.Calculate(&nRaw,&nCol))
		return false;

	vector<vector<double> >* pTrainCov = covTrainRsrp.GetResult();

	pUsableTestSig->vecMean = vecStuTestMean;
	pUsableTestSig->vecCov  = *pTestCov;
	pUsableTestSig->nId     = pTestSig->nId;

	pUsableTrainSig->vecMean = vecStuTrainMean;
	pUsableTrainSig->vecCov  = *pTrainCov;
	pUsableTrainSig->nId     = pTrainSig->nId;
    return true;
}

bool compare(const TmpResult& t1, const TmpResult& t2)
{
    return t1.CalResult < t2.CalResult;
}


bool CSimCal::GetMin(vector<TmpResult>* pVec,TmpResult *pDb)
{
	if(pVec->size() <= 0)
		return false;
    sort(pVec->begin(), pVec->end(), compare);

    pDb->CalResult = (*pVec)[0].CalResult;
    pDb->SigID = (*pVec)[0].SigID;
	return true;
}

bool compareMax(const TmpResult& t1, const TmpResult& t2)
{
    return t1.CalResult > t2.CalResult;
}


bool CSimCal::GetMax(vector<TmpResult>* pVec,TmpResult *pDb)
{
	if(pVec->size() <= 0)
		return false;
    sort(pVec->begin(), pVec->end(), compareMax);
    vector<TmpResult>::iterator iter1 = pVec->begin();
    vector<TmpResult>::iterator iter2 = pVec->end();
//    for (; iter1 != iter2; iter1++)
//    {
//        cout << "GetMax " << (*iter1).CalResult << endl;
//    }
//    system("pause");

    pDb->CalResult = (*pVec)[0].CalResult;
    pDb->SigID = (*pVec)[0].SigID;
	return true;
}

bool compareDiff(const TmpResult& t1, const TmpResult& t2)
{
    return t1.stuVar.dbDiff < t2.stuVar.dbDiff;
}

bool compareVar(const TmpResult& t1, const TmpResult& t2)
{
    return t1.stuVar.dbVar < t2.stuVar.dbVar;
}

bool CSimCal::GetVarMin(vector<TmpResult>* pVec,TmpResult *pDb, int nThresholdVar)
{
	if(pVec->size() <= 0)
		return false;
    sort(pVec->begin(), pVec->end(), compareDiff);
    vector<TmpResult> vecTenResult;
    vector<TmpResult>::iterator iter1 = pVec->begin();
    vector<TmpResult>::iterator iter2 = pVec->end();
    int i = 0;
    if ( pVec->size() <= nThresholdVar)
        nThresholdVar = pVec->size();
    for (; i != nThresholdVar; iter1++,i++)
    {
        vecTenResult.push_back((*iter1));
    }
    sort(vecTenResult.begin(), vecTenResult.end(), compareVar);

    pDb->CalResult = vecTenResult[0].CalResult;
    pDb->SigID = vecTenResult[0].SigID;
	return true;
}

bool CSimCal::CalDistanse(double x1, double y1,
                         double x2, double y2,
                         double* pD)
{
    *pD = sqrt((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
}

bool CSimCal::CalBestCoor(Signatrue* sig, BestCoor* pBestCoor)
{
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    vector<TmpResult> vecBest;
    for (; iter1 != iter2; iter1++)
    {
        TmpResult Best;
        CalDistanse(sig->nX,sig->nY,
                    (*iter1).nX, (*iter1).nY,
                    &(Best.CalResult));

        Best.SigID = (*iter1).nId;
        vecBest.push_back(Best);
    }
    TmpResult sr;
    GetMin(&vecBest,&sr);
    pBestCoor->BestID = sr.SigID ;
    pBestCoor->CalDisResult = sr.CalResult;

}

bool CSimCal::CheckCellID(Signatrue* pTest, Signatrue* pTrain)
{
    if ((pTest->vecMean.size()) == (pTrain->vecMean.size()))
    {
        vector<stuMean>::iterator iter1 = pTest->vecMean.begin();
        vector<stuMean>::iterator iter2 = pTest->vecMean.end();
        for (; iter1 != iter2; iter1++)
        {
            bool bIsMacth = 1;
            vector<stuMean>::iterator iter3 = pTrain->vecMean.begin();
            vector<stuMean>::iterator iter4 = pTrain->vecMean.end();
            for (int i = 1; iter3 != iter4; iter3++,i++)
            {
                if ((*iter1).neNB == (*iter3).neNB)
                    break;// �Ƚ�����ǩ���Ļ�վ��,�������ȵ�ֱ�Ӽ�����һ����վ��
                if (i == (pTrain->vecMean).size())
                {
                    // �жϵ����Ҳû����ȵģ���˵�����������վ��ѵ��ǩ����û��
                    bIsMacth = 0;
                }
            }
            if (!bIsMacth)
                return false;//ֻҪ��һ����վ�Ų���ȣ�������ֹ������false
        }
    }
    else
    {
        return false;
    }
    return true;// ��ƥ��ɹ��Ļ�������true��

}


bool CSimCal::SimCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
//*********************************
//  ����������ƶ�ʱÿ������ǩ��������ѵ�����ȶ�ʱ�����Ľ����
//  ��һ��ֻ�Ǵ���һ���ļ�
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
//**********************************
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;

    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    int i = 1;
    for (int j = 1; iter1 != iter2; iter1++,j++)
    {
//        cout << "Calculate TrainSigID = " << (*iter1).nId ;
//        cout << " TestSigID = " << pSig->nId << " Down! " << endl;
        /** �ϲ���վ **/
//        Signatrue* pTeSig = new Signatrue;
//        MergeCellID(pSig,pTeSig);
//
//        Signatrue* pTRSig = new Signatrue;
//        MergeCellID(&(*iter1),pTRSig);
        /** �ϲ���վ **/

        /** ���ϲ���վ **/
        Signatrue* pTestSig;
        Signatrue* pTrainSig;
        /** ���ϲ���վ **/

        // ���ǩ���Ļ�վ���Ƿ���ȫ�����
//        bool bIsMacth = CheckCellID(pTeSig, pTRSig);
        bool bIsMacth = CheckCellID(pSig, &(*iter1));

        if (bIsMacth)
        {
            // �����վ��ȫ����ȣ���Ϊ��վ��ƥ��Ⱥܸߡ�
            // ֱ�ӽ�����ǩ��ȫ����������������
//            pTestSig = pTeSig;
//            pTrainSig = pTRSig;
            pTestSig = pSig;
            pTrainSig = &(*iter1);
        }
        else
        {
            // ��������ֱ��continue��������ǰѵ��ǩ��
            if (i == j )
            {
                // i��j���������жϲ���ǩ���Ƿ��Ѿ���ȫ��
                // ��ѵ��ǩ���ȶԹ�������������û�л�վ��ȫ����ȵĶ�һֱcontinue
                if (i == m_SigInfoList->size())
                {
                    // �����ֲ���ǩ����ѵ�������Ҳ�����վ��ȫ����ȵ�ǩ���ͻ�ʹ��
                    // MeanCellIDCal�ò���ǩ�������е�ѵ����ǩ��ȥ�ȶԡ�
                    // �൱�ڵ���������һ��Mean�㷨��һ������ǩ���ּ�����һ�飬
                    // ����һ������������Ὣ���������pSum���棬��������SimCellIDCal

                    MeanCellIDCal(pSig,pSum);
                    return false;
                }
            }
            i += 1;
            continue;
        }



        // ���濪ʼ���þ�ֵ�����
        vector<stuMean>::iterator iter3 = pTestSig->vecMean.begin();
        vector<stuMean>::iterator iter4 = pTestSig->vecMean.end();
        vector<stuMean>::iterator iter5 = pTrainSig->vecMean.begin();

        vector<double> vecDiff;// ����һ������ǩ����һ��ѵ��ǩ��ÿ����վ��ֵ�Ĳ�ֵ
        for (; iter3 != iter4; iter3++,iter5++)
        {
            double dbDiff;

//            stuMean conTestRsrp;
//            stuMean conTrainRsrp;
//            CalPowerMean(&(*iter3), &conTestRsrp);
//            CalPowerMean(&(*iter5), &conTrainRsrp);

            dbDiff = (*iter3).dbMean - (*iter5).dbMean;
//            dbDiff = conTestRsrp.dbMean - conTrainRsrp.dbMean;


            dbDiff = fabs(dbDiff);
            vecDiff.push_back(dbDiff);
        }
//        double dbValue = SumDiffCal(&vecDiff);
//        double dbValue = MultiDiffCal(&vecDiff);
//        double dbValue = SumWeighing(&vecDiff);
//        double dbValue = CalVar(&vecDiff);
        double dbValue = Euclidean(&vecDiff);


        double dbCoor = 0;
        TmpResult oneSum;
        oneSum.SigID = (*iter1).nId;
        oneSum.CalResult = dbValue;
        pSum->push_back(oneSum);

        CDBoperate SelTrainCoor;
        SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
        GetCoor trainCoor, testCoor; // ����ǩ��������
        SelTrainCoor.GetCoordinateFromTable((*iter1).nId, &trainCoor);
        SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        // �м���ȫ�����档
        dataFile << "TrainID = " << (*iter1).nId << "  ";
        dataFile << "dbValue = " << dbValue << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;

        vector<double>::iterator iterW1 = vecDiff.begin();
        vector<double>::iterator iterW2 = vecDiff.end();
        for (; iterW1 != iterW2; iterW1++)
        {
            dataFile << (*iterW1) << "  ";
        }
        dataFile << endl;


    }
    dataFile.close();

}

bool CSimCal::MeanCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;

    int nTotalNum = 0;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++)
    {

        /** �ϲ���վ **/
//        Signatrue* pTeSig = new Signatrue;
//        MergeCellID(pSig,pTeSig);
//
//        Signatrue* pTRSig = new Signatrue;
//        MergeCellID(&(*iter1),pTRSig);
        /** �ϲ���վ **/

        Signatrue TestSig;
        Signatrue TrainSig;

        /** �ϲ���վ **/
//        TestSig = *pTeSig;
//        TrainSig = *pTRSig;
        /** �ϲ���վ **/

        // ��Ҫ������������������
        bool  bIs = MeanMachCal(pSig, &(*iter1), &TestSig, &TrainSig);

        if (!bIs)
        {
            continue;
        }

        int nD = pSig->vecMean.size() - TrainSig.vecMean.size();
        nD = fabs(nD);
        if ( nD <= 3 )
        {
            nTotalNum++;
            vector<stuMean>::iterator iter3 = TestSig.vecMean.begin();
            vector<stuMean>::iterator iter4 = TestSig.vecMean.end();
            vector<stuMean>::iterator iter5 = TrainSig.vecMean.begin();

//            cout << "testID = " << TestSig.nId << endl;
//            cout << "trainID = " << TrainSig.nId << endl;
//            system("pause");

            vector<double> vecDiff;
            for (; iter3 != iter4; iter3++,iter5++)
            {
                double dbDiff;

    //            stuMean conTestRsrp;
    //            stuMean conTrainRsrp;
    //            CalPowerMean(&(*iter3), &conTestRsrp);
    //            CalPowerMean(&(*iter5), &conTrainRsrp);

                dbDiff = (*iter3).dbMean - (*iter5).dbMean;
//                dbDiff = conTestRsrp.dbMean - conTrainRsrp.dbMean;
                dbDiff = fabs(dbDiff);
                vecDiff.push_back(dbDiff);
            }

            double dbValue = SumDiffCal(&vecDiff);
//            double dbValue = MultiDiffCal(&vecDiff);
//            double dbValue = CalVar(&vecDiff);

            TmpResult oneSum;
            oneSum.SigID = (*iter1).nId;
            oneSum.CalResult = dbValue;
            oneSum.nTestCell = pSig->vecMean.size();
            oneSum.nTrainCell = TrainSig.vecMean.size();
            oneSum.nCellIDDiff = nD;

            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << "  ";
            vector<double>::iterator iterW1 = vecDiff.begin();
            vector<double>::iterator iterW2 = vecDiff.end();
            for (; iterW1 != iterW2; iterW1++)
            {
                dataFile << (*iterW1) << "  ";
            }
            dataFile << endl;

        }


    }

    m_dbTotal += nTotalNum;
//    cout << "dbTotal = " << m_dbTotal << endl;
    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "nTestCell = " << (*iter3).nTestCell << "  ";
        dataFile << "nTrainCell = " << (*iter3).nTrainCell << "  ";
        dataFile << "nDiff = " << (*iter3).nCellIDDiff << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();

}

bool CSimCal::EuclideanCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;

        // ��Ҫ������������������
        bool  bIs = MeanMachCal(pSig, &(*iter1), &TestSig, &TrainSig);

        if (!bIs)
        {
            continue;
        }

        int nD = pSig->vecMean.size() - TrainSig.vecMean.size();
        nD = fabs(nD);
        if ( nD <= 3 )
        {
            vector<stuMean>::iterator iter3 = TestSig.vecMean.begin();
            vector<stuMean>::iterator iter4 = TestSig.vecMean.end();
            vector<stuMean>::iterator iter5 = TrainSig.vecMean.begin();

//            cout << "testID = " << TestSig.nId << endl;
//            cout << "trainID = " << TrainSig.nId << endl;
//            system("pause");
            double dbDiff = 0;
            vector<double> vecDiff;
            for (; iter3 != iter4; iter3++,iter5++)
            {
                dbDiff += ((*iter3).dbMean - (*iter5).dbMean)*((*iter3).dbMean - (*iter5).dbMean);
            }
            double dbValue;
            dbValue = sqrt(dbDiff);

            TmpResult oneSum;
            oneSum.SigID = (*iter1).nId;
            oneSum.CalResult = dbValue;
            oneSum.nTestCell = pSig->vecMean.size();
            oneSum.nTrainCell = TrainSig.vecMean.size();
            oneSum.nCellIDDiff = nD;

            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << "  ";
            vector<double>::iterator iterW1 = vecDiff.begin();
            vector<double>::iterator iterW2 = vecDiff.end();
            for (; iterW1 != iterW2; iterW1++)
            {
                dataFile << (*iterW1) << "  ";
            }
            dataFile << endl;

        }

    }


    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "nTestCell = " << (*iter3).nTestCell << "  ";
        dataFile << "nTrainCell = " << (*iter3).nTrainCell << "  ";
        dataFile << "nDiff = " << (*iter3).nCellIDDiff << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();

}

bool CSimCal::KNNCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;

        // ��Ҫ������������������
        bool  bIs = MeanMachCal(pSig, &(*iter1), &TestSig, &TrainSig);

        if (!bIs)
        {
            continue;
        }
        if (pSig->vecMean.size() == 0 || TrainSig.vecMean.size() == 0)
            continue;

//        cout << "testID = " << pSig->nId << " trainID = " << (*iter1).nId << " size = " << TrainSig.vecMean.size() << endl;
        int nD = pSig->vecMean.size() - TrainSig.vecMean.size();
        nD = fabs(nD);
        if ( 1 )
        {
            vector<stuMean>::iterator iter3 = TestSig.vecMean.begin();
            vector<stuMean>::iterator iter4 = TestSig.vecMean.end();
            vector<stuMean>::iterator iter5 = TrainSig.vecMean.begin();

            double dbDiff = 0;
            vector<double> vecDiff;
            for (; iter3 != iter4; iter3++,iter5++)
            {
                dbDiff += ((*iter3).dbMean - (*iter5).dbMean)*((*iter3).dbMean - (*iter5).dbMean);
            }
            double dbValue;
            dbValue = sqrt(dbDiff);

            TmpResult oneSum;
            oneSum.SigID = (*iter1).nId;
            oneSum.CalResult = dbValue;
            oneSum.nTestCell = pSig->vecMean.size();
            oneSum.nTrainCell = TrainSig.vecMean.size();
            oneSum.nCellIDDiff = nD;

            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << "  ";
            vector<double>::iterator iterW1 = vecDiff.begin();
            vector<double>::iterator iterW2 = vecDiff.end();
            for (; iterW1 != iterW2; iterW1++)
            {
                dataFile << (*iterW1) << "  ";
            }
            dataFile << endl;

        }

    }


    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "nTestCell = " << (*iter3).nTestCell << "  ";
        dataFile << "nTrainCell = " << (*iter3).nTrainCell << "  ";
        dataFile << "nDiff = " << (*iter3).nCellIDDiff << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();
    cout << "TestSigID = " << pSig->nId << endl ;
}

bool compare2(const stuMean& t1, const stuMean& t2)
{
    return t1.dbMean > t2.dbMean;
}

bool CSimCal::CellIDCompareCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;

    int nTag = 1;
    int nNN = 0;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++,nTag++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;

        TestSig = *pSig;
        TrainSig = *iter1;
//
//        bool  bIs = MeanMachCal(pSig, &(*iter1), &TestSig, &TrainSig);
//
//        if (!bIs)
//        {
//            continue;
//        }
//
//        int nD = pSig->vecMean.size() - TrainSig.vecMean.size();
//        nD = fabs(nD);
        int nNu = 0;

        double dbValue = 0;
        int neNB;
//        if ( nD <= 3 )
//        {
        sort (TestSig.vecMean.begin(),TestSig.vecMean.end(),compare2);
        sort (TrainSig.vecMean.begin(),TrainSig.vecMean.end(),compare2);

//        one_TrainSig(&TestSig, &TrainSig, pSum);


/******/
        if (TrainSig.vecMean.at(0).neNB == TestSig.vecMean.at(0).neNB &&
            TrainSig.vecMean.at(1).neNB == TestSig.vecMean.at(1).neNB &&
            TrainSig.vecMean.at(2).neNB == TestSig.vecMean.at(2).neNB)
        {
//            cout << " three " << endl;
            double dbTradiffTes0 = TrainSig.vecMean.at(0).dbMean - TestSig.vecMean.at(0).dbMean;
            double dbTradiffTes1 = TrainSig.vecMean.at(1).dbMean - TestSig.vecMean.at(1).dbMean;
            double dbTradiffTes2 = TrainSig.vecMean.at(2).dbMean - TestSig.vecMean.at(2).dbMean;
            dbValue = sqrt((dbTradiffTes0*dbTradiffTes0) + (dbTradiffTes1*dbTradiffTes1) +
                           (dbTradiffTes2*dbTradiffTes2));
            nNN = 1;
            TmpResult oneSum;
            oneSum.SigID = (*iter1).nId;
            oneSum.CalResult = dbValue;
            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << endl;
            dataFile << "Train eNB_1 = " <<TrainSig.vecMean.at(0).neNB << " ";
            dataFile << "dbMean = " << TrainSig.vecMean.at(0).dbMean << " ";
            dataFile << "Train eNB_2 = " << TrainSig.vecMean.at(1).neNB << " ";
            dataFile << "dbMean = " << TrainSig.vecMean.at(1).dbMean << " ";
            dataFile << "Train eNB_3 = " << TrainSig.vecMean.at(2).neNB << " ";
            dataFile << "dbMean = " << TrainSig.vecMean.at(2).dbMean << " ";
            dataFile << "value = " << dbValue << endl;
        }
        else if (iter1 == (iter2-1) && nNN == 0)
        {
            CellIDCompareCal_two(pSig, pSum);
        }

/******/
    }

    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();
    return 1;
}

bool CSimCal::CellIDCompareCal_two(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;

    int nTag = 1;
    int nNN = 0;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++,nTag++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;

        TestSig = *pSig;
        TrainSig = *iter1;

        int nNu = 0;

        double dbValue = 0;
        int neNB;

        sort (TestSig.vecMean.begin(),TestSig.vecMean.end(),compare2);
        sort (TrainSig.vecMean.begin(),TrainSig.vecMean.end(),compare2);
/******/
        if (TrainSig.vecMean.at(0).neNB == TestSig.vecMean.at(0).neNB &&
            TrainSig.vecMean.at(1).neNB == TestSig.vecMean.at(1).neNB)
        {
//            cout << " two " << endl;
            double dbTradiffTes0 = TrainSig.vecMean.at(0).dbMean - TestSig.vecMean.at(0).dbMean;
            double dbTradiffTes1 = TrainSig.vecMean.at(1).dbMean - TestSig.vecMean.at(1).dbMean;
            dbValue = sqrt((dbTradiffTes0*dbTradiffTes0) + (dbTradiffTes1*dbTradiffTes1) );
            nNN = 1;
            TmpResult oneSum;
            oneSum.SigID = TrainSig.nId;
            oneSum.CalResult = dbValue;
            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << endl;
            dataFile << "Train eNB_1 = " <<TrainSig.vecMean.at(0).neNB << " ";
            dataFile << "dbMean = " << TrainSig.vecMean.at(0).dbMean << " ";
            dataFile << "Train eNB_2 = " << TrainSig.vecMean.at(1).neNB << " ";
            dataFile << "dbMean = " << TrainSig.vecMean.at(1).dbMean << " ";
            dataFile << "value = " << dbValue << endl;
            return true;
        }
        else if (iter1 == (iter2-1) && nNN == 0)
        {
            CellIDCompareCal_one(pSig, pSum);
        }
    }
    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();
    return 1;
}

bool CSimCal::CellIDCompareCal_one(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;

    int nTag = 1;
    int nNN = 0;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++,nTag++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;

        TestSig = *pSig;
        TrainSig = *iter1;

        int nNu = 0;

        double dbValue = 0;
        int neNB;

        sort (TestSig.vecMean.begin(),TestSig.vecMean.end(),compare2);
        sort (TrainSig.vecMean.begin(),TrainSig.vecMean.end(),compare2);
/******/
        if (TrainSig.vecMean.at(0).neNB == TestSig.vecMean.at(0).neNB )
        {
//            cout << " one " << endl;
            dbValue = TrainSig.vecMean.at(0).dbMean;
            nNN = 1;
            TmpResult oneSum;
            oneSum.SigID = TrainSig.nId;
            oneSum.CalResult = dbValue;
            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << " ";
            dataFile << "dbMean = " << dbValue << endl;
            return true;
        }
        else if ( iter1 == (iter2-1) && nNN == 0)
        {
            cout << "return 0" << endl;
            return 0;
        }

    }

    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();
    return 1;
}

bool CSimCal::VarCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;
        bool  bIs = MeanMachCal(pSig, &(*iter1), &TestSig, &TrainSig);
        if (!bIs)
        {
            continue;
        }
        int nD = pSig->vecMean.size() - TrainSig.vecMean.size();
        nD = fabs(nD);
        if ( nD <= 3 )
//        if ( true )
        {
            vector<stuMean>::iterator iter3 = TestSig.vecMean.begin();
            vector<stuMean>::iterator iter4 = TestSig.vecMean.end();
            vector<stuMean>::iterator iter5 = TrainSig.vecMean.begin();
            vector<double> vecDiff;
            for (; iter3 != iter4; iter3++,iter5++)
            {
                double dbDiff = 0;
                dbDiff = (*iter3).dbMean - (*iter5).dbMean;
                dbDiff = fabs(dbDiff);
                vecDiff.push_back(dbDiff);
            }

            VarMin stuVar;
            SumVar(&vecDiff, &stuVar);

            TmpResult oneSum;
            oneSum.SigID = (*iter1).nId;
            oneSum.CalResult = stuVar.dbDiff;
            oneSum.nTestCell = pSig->vecMean.size();
            oneSum.nTrainCell = TrainSig.vecMean.size();
            oneSum.nCellIDDiff = nD;
            oneSum.stuVar = stuVar; // ������¼ӵġ�
            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << "  ";
            vector<double>::iterator iterW1 = vecDiff.begin();
            vector<double>::iterator iterW2 = vecDiff.end();
            for (; iterW1 != iterW2; iterW1++)
            {
                dataFile << (*iterW1) << "  ";
            }
            dataFile << endl;
        }
    }
    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);
        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);
        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "dbVar = " << (*iter3).stuVar.dbVar << "  ";
        dataFile << "nTestCell = " << (*iter3).nTestCell << "  ";
        dataFile << "nTrainCell = " << (*iter3).nTrainCell << "  ";
        dataFile << "nDiff = " << (*iter3).nCellIDDiff << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }
    dataFile.close();
}

bool CSimCal::PearsonCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;
        bool  bIs = MeanMachCal(pSig, &(*iter1), &TestSig, &TrainSig);
        if (!bIs)
        {
            continue;
        }
        int nD = pSig->vecMean.size() - TrainSig.vecMean.size();
        nD = fabs(nD);
        if ( nD <= 3 )
//        if ( true )
        {
            vector<double> vecX;
            vector<double> vecY;
            vector<double> vecXY;
            vector<double> vecXX;
            vector<double> vecYY;
            double dbTmpUp = 0;
            double dbTmpDown1 = 0;
            double dbTmpDown2 = 0;
            vector<stuMean>::iterator iter3 = TestSig.vecMean.begin();
            vector<stuMean>::iterator iter4 = TestSig.vecMean.end();
            vector<stuMean>::iterator iter5 = TrainSig.vecMean.begin();
            for (; iter3 != iter4; iter3++,iter5++)
            {
                double dbXY = ((*iter3).dbMean) * ((*iter5).dbMean);
                double dbXX = ((*iter3).dbMean) * ((*iter3).dbMean);
                double dbYY = ((*iter5).dbMean) * ((*iter5).dbMean);
                vecX.push_back((*iter3).dbMean);
                vecY.push_back((*iter5).dbMean);
                vecXY.push_back(dbXY);
                vecXX.push_back(dbXX);
                vecYY.push_back(dbYY);
            }
            double dbXMean = CalMean(&vecX);
            double dbYMean = CalMean(&vecY);
            dbTmpUp = CalMean(&vecXY) - dbXMean*dbYMean; // E(XY) - E(X)E(Y)
            dbTmpDown1 = sqrt(CalMean(&vecXX) -  dbXMean*dbXMean);
            dbTmpDown2 = sqrt(CalMean(&vecYY) -  dbYMean*dbYMean);
            double dbValue = dbTmpUp / (dbTmpDown1 * dbTmpDown2);

            TmpResult oneSum;
            oneSum.SigID = (*iter1).nId;
            oneSum.CalResult = dbValue;
            oneSum.nTestCell = pSig->vecMean.size();
            oneSum.nTrainCell = TrainSig.vecMean.size();
            oneSum.nCellIDDiff = nD;

            pSum->push_back(oneSum);
            dataFile << "trainID = " << TrainSig.nId << "  ";
            dataFile << dbValue << "  ";
            dataFile << endl;

        }

    }


    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "nTestCell = " << (*iter3).nTestCell << "  ";
        dataFile << "nTrainCell = " << (*iter3).nTrainCell << "  ";
        dataFile << "nDiff = " << (*iter3).nCellIDDiff << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();
}

bool CSimCal::WeightingCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
    char str[50];
    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"MeanMiddle\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }
    cout << "Start to calculate TestSigID = " << pSig->nId << endl ;
    vector<Signatrue>::iterator iter1 = m_SigInfoList->begin();
    vector<Signatrue>::iterator iter2 = m_SigInfoList->end();
    for (; iter1 != iter2; iter1++)
    {
        Signatrue TestSig;
        Signatrue TrainSig;
        bool  bIs = MeanMachCal(pSig, &(*iter1), &TestSig, &TrainSig);
        if (!bIs)
        {
            continue;
        }
        int nD = pSig->vecMean.size() - TrainSig.vecMean.size();
        nD = fabs(nD);
        if ( nD <= 3 )
        {
            vector<stuMean>::iterator iter3 = TestSig.vecMean.begin();
            vector<stuMean>::iterator iter4 = TestSig.vecMean.end();
            vector<stuMean>::iterator iter5 = TrainSig.vecMean.begin();
            vector<double> vecDiff;
            for (; iter3 != iter4; iter3++,iter5++)
            {
                double dbDiff;
                dbDiff = (*iter3).dbMean - (*iter5).dbMean;
                dbDiff = fabs(dbDiff);
                vecDiff.push_back(dbDiff);
            }

            double dbValue = WeghtDiffCal(&vecDiff, &(TrainSig.vecMean));

            TmpResult oneSum;
            oneSum.SigID = (*iter1).nId;
            oneSum.CalResult = dbValue;
            oneSum.nTestCell = pSig->vecMean.size();
            oneSum.nTrainCell = TrainSig.vecMean.size();
            oneSum.nCellIDDiff = nD;

            pSum->push_back(oneSum);

            dataFile << "trainID = " << TrainSig.nId << "  ";
            vector<double>::iterator iterW1 = vecDiff.begin();
            vector<double>::iterator iterW2 = vecDiff.end();
            for (; iterW1 != iterW2; iterW1++)
            {
                dataFile << (*iterW1) << "  ";
            }
            dataFile << endl;

        }

    }


    CDBoperate SelTrainCoor;
    GetCoor testCoor;
    SelTrainCoor.OpenDB("FingerPrintTrainSet.db");
    SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

    vector<TmpResult> vecCal((*pSum));
    sort(vecCal.begin(), vecCal.end(), compare);
    vector<TmpResult>::iterator iter3 = vecCal.begin();
	vector<TmpResult>::iterator iter4 = vecCal.end();
	for(; iter3 != iter4; iter3++)
	{
        double dbCoor = 0;
        GetCoor trainCoor;
        SelTrainCoor.GetCoordinateFromTable((*iter3).SigID, &trainCoor);


        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        dataFile << "TrainID = " << (*iter3).SigID << "  ";
        dataFile << "dbValue = " << (*iter3).CalResult << "  ";
        dataFile << "nTestCell = " << (*iter3).nTestCell << "  ";
        dataFile << "nTrainCell = " << (*iter3).nTrainCell << "  ";
        dataFile << "nDiff = " << (*iter3).nCellIDDiff << "  ";
        dataFile << "(X,Y) = (" << trainCoor.TrainSigX << ", ";
        dataFile <<  trainCoor.TrainSigY << ")" << "  ";
        dataFile << "Position Error = " << dbCoor << endl;
    }

    dataFile.close();

}

double CSimCal::MultiDiffCal(vector<double>* vecDiff)
{

    vector<double>::iterator iterS1 = vecDiff->begin();
    vector<double>::iterator iterS2 = vecDiff->end();
    double dbValue = (*iterS1);
    for(; iterS1 != iterS2; iterS1++)
    {
        dbValue *= (*iterS1);
    }
    dbValue = 1 / dbValue;
    return dbValue;
}

double CSimCal::SumDiffCal(vector<double>* vecDiff)
{
    double dbValue = 0;
    vector<double> vecSingleDiff;
    vector<double>::iterator iterS1 = vecDiff->begin();
	vector<double>::iterator iterS2 = vecDiff->end();
	for(; iterS1 != iterS2; iterS1++)
	{
		dbValue += (*iterS1);
	}
	dbValue /= vecDiff->size();
	return dbValue;
}



double CSimCal::WeghtDiffCal(vector<double>* vecDiff, vector<stuMean>* TestSigMean)
{
    double dbValue = 0;
    vector<double> vecSingleDiff;
    vector<double>::iterator iterS1 = vecDiff->begin();
	vector<double>::iterator iterS2 = vecDiff->end();
	vector<stuMean>::iterator iterS3 = TestSigMean->begin();
	for(; iterS1 != iterS2; iterS1++)
	{
		dbValue += ((*iterS1) / (*iterS3).dbMean) ;

	}
	dbValue /= vecDiff->size();
	return abs(dbValue);
}

bool CSimCal::SumVar(vector<double>* vecDiff, VarMin* stuVal)
{
    stuVal->dbDiff = SumDiffCal(vecDiff);
    stuVal->dbVar = CalVar(vecDiff);




	return true;
}

double CSimCal::CalVar(vector<double>* vecDiff)
{
    double dbValue = 0;
    double dbMean = SumDiffCal(vecDiff);
    vector<double>::iterator iter1 = vecDiff->begin();
	vector<double>::iterator iter2 = vecDiff->end();
	for(; iter1 != iter2; iter1++)
	{
		dbValue += ((*iter1) - dbMean)*((*iter1) - dbMean);
	}
    dbValue /= vecDiff->size();
    return dbValue;
}

double CSimCal::Euclidean(vector<double>* vecDiff)
{
    double dbValue = 0;
    vector<double>::iterator iter1 = vecDiff->begin();
	vector<double>::iterator iter2 = vecDiff->end();
	for(; iter1 != iter2; iter1++)
	{
		dbValue += (*iter1) *(*iter1);
	}
    dbValue = sqrt(dbValue);
    return dbValue;
}

bool CSimCal::MeanMachCal(Signatrue* pTestSig,
                          Signatrue* pTrainSig,
                          Signatrue* pUsableTestSig,
                          Signatrue* pUsableTrainSig)
{

    vector<stuMean>::iterator iter1 = pTestSig->vecMean.begin();
    vector<stuMean>::iterator iter2 = pTestSig->vecMean.end();

    vector<stuMean> vecStuTestMean;
    vector<stuMean> vecStuTrainMean;

    vector<vector<double> > comVecTestRsrp;
    vector<vector<double> > comVecTrainRsrp;

    for (; iter1 != iter2; iter1++)
    {
        vector<stuMean>::iterator iter3 = pTrainSig->vecMean.begin();
        vector<stuMean>::iterator iter4 = pTrainSig->vecMean.end();

        for (; iter3 != iter4; iter3++)
        {
            if ((*iter1).neNB == (*iter3).neNB)
            {
                stuMean intTestMean;
                intTestMean.neNB    = (*iter1).neNB;
                intTestMean.dbMean  = (*iter1).dbMean;
                vecStuTestMean.push_back(intTestMean);

                stuMean intTrainMean;
                intTrainMean.neNB   = (*iter1).neNB;//���Լ���ѵ�����Թ���һ����վ�š�
                intTrainMean.dbMean = (*iter3).dbMean;
                vecStuTrainMean.push_back(intTrainMean);

                comVecTestRsrp.push_back( (*iter1).rsrp);
                comVecTrainRsrp.push_back((*iter3).rsrp);
            }
        }
    }

    if (vecStuTestMean.size() == 0 || vecStuTrainMean.size() == 0)
    {
        return false;
    }

	pUsableTestSig->vecMean = vecStuTestMean;
	pUsableTestSig->nId     = pTestSig->nId;

	pUsableTrainSig->vecMean = vecStuTrainMean;
	pUsableTrainSig->nId     = pTrainSig->nId;

    return true;
}

bool CSimCal::LOG_CreatSimCalReasult(Signatrue* pSig,
                                     vector<TmpResult>* pTmpKLD,
                                     vector<TmpResult>* pTmpMaD)
{
    char str[50];

    static char buf[64] = {0};
    snprintf(buf, sizeof(buf)
   , "%08d"
   , pSig->nId
      );
    sprintf(str,"SimResult\\%s.txt",buf);

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }

    vector<TmpResult>::iterator iter1 = pTmpKLD->begin();
    vector<TmpResult>::iterator iter2 = pTmpKLD->end();


    for (; iter1 != iter2; iter1++)
    {
        dataFile << (*iter1).SigID << " ";
        dataFile << (*iter1).CalResult << endl;
    }
    dataFile << "         ************************" << endl;
    vector<TmpResult>::iterator iter3 = pTmpMaD->begin();
    vector<TmpResult>::iterator iter4 = pTmpMaD->end();
    for (; iter3 != iter4; iter3++)
    {
        dataFile << (*iter3).SigID << " ";
        dataFile << (*iter3).CalResult << endl;
    }

    dataFile.close();
}

// ֻ������һ��ǩ��
bool CSimCal::MergeCellID(Signatrue* pSig, Signatrue* pMergeSig)
{
    int nThreeSectorCell = 171; // Ŀǰ�Ļ�վ�����ǹ̶��ģ��������ó��˳�����

    vector<stuMean> vecSaveCellInfo;
    vector<stuMean> vecTmpCellInfo;
    vector<stuMean>::iterator iter1 = pSig->vecMean.begin();
    vector<stuMean>::iterator iter2 = pSig->vecMean.end();

    int nflag = 0;
    int nBig = ((*iter1).neNB) / 3 ;
    double dbSmall = ((*iter1).neNB) % 3 ;
    if (dbSmall != 0)
        nflag = nBig + 1;
    else
        nflag = nBig;

    for (int i = 1; iter1 != iter2; iter1++,i++)
    {
        if ((*iter1).neNB <= nThreeSectorCell)
        {
            stuMean mean;
            nBig = ((*iter1).neNB) / 3 ;
            dbSmall = ((*iter1).neNB) % 3 ;
            if (dbSmall != 0)
            {
                mean.neNB = nBig + 1;
            }
            else
            {
                mean.neNB = nBig;
            }
            mean.rsrp = (*iter1).rsrp;
            mean.dbMean = (*iter1).dbMean;

            if ( mean.neNB == nflag )
            {
                vecTmpCellInfo.push_back(mean);
                if ((vecTmpCellInfo.size() == 3) && (i == pSig->vecMean.size()))
                {
                    stuMean tmpMean;
                    MergeThreeCellInfo(&vecTmpCellInfo, &tmpMean);
                    pMergeSig->vecMean.push_back(tmpMean);
                }

            }
            else
            {
                nflag = mean.neNB;
                if (vecTmpCellInfo.size() == 3)
                {
                    stuMean tmpMean;
                    MergeThreeCellInfo(&vecTmpCellInfo, &tmpMean);
                    pMergeSig->vecMean.push_back(tmpMean);
                    vecTmpCellInfo.clear();
                    vecTmpCellInfo.push_back(mean);

                }
                else
                {
//                    cout << "vecTmpCellInfo = " << vecTmpCellInfo.size() << " ";
//                    cout << "vecTmpCellInfo.at(0) = " << vecTmpCellInfo.at(0).neNB << endl;
//                    system("pause");
                    vecTmpCellInfo.clear();
                    vecTmpCellInfo.push_back(mean);
                }
            }
        }
        else
        {
            pMergeSig->vecMean.push_back((*iter1));
        }
    }

    pMergeSig->nId = pSig->nId;
    pMergeSig->nX = pSig->nX;
    pMergeSig->nY = pSig->nY;
//    pMergeSig->vecMean = vecSaveCellInfo;

}

bool CSimCal::MergeThreeCellInfo(vector<stuMean>* pVecStuMean, stuMean* pMean)
{

    pMean->neNB = (pVecStuMean->at(0)).neNB;
    vector<double> vecRsrp;
    stuMean mean1 = pVecStuMean->at(0);
    stuMean mean2 = pVecStuMean->at(1);
    stuMean mean3 = pVecStuMean->at(2);
    vector<double>::iterator iter1 = mean1.rsrp.begin();
    vector<double>::iterator iter2 = mean1.rsrp.end();
    vector<double>::iterator iter3 = mean2.rsrp.begin();
    vector<double>::iterator iter4 = mean3.rsrp.begin();
    for (; iter1 != iter2; iter1++,iter3++,iter4++)
    {
        double dbRsrp = (*iter1 + *iter3 + *iter4)/ 3;
        vecRsrp.push_back(dbRsrp);
    }
    pMean->rsrp = vecRsrp;
    double dbMean = (mean1.dbMean + mean2.dbMean + mean3.dbMean) / 3 ;
    pMean->dbMean = dbMean;
    return true;
}

double CSimCal::ConvertToW(double dbDBm)
{
    double dbW;
    dbW = (pow(10, (dbDBm / 10))) / 1000;
    return dbW;
}
//
//bool CSimCal::CalPowerMean(vecDiffstuMean* stuRsrp, stuMean* conRsrp)
//{
//    vector<double> vecPowerRsrp;
//    vector<double>::iterator iter1 = stuRsrp->rsrp.begin();
//    vector<double>::iterator iter2 = stuRsrp->rsrp.end();
//    for(; iter1 != iter2; iter1++)
//    {
//        double dbPower = ConvertToW((*iter1));
//        vecPowerRsrp.push_back(dbPower);
//    }
//	vector<double>::iterator iter3 = vecPowerRsrp.begin();
//	vector<double>::iterator iter4 = vecPowerRsrp.end();
//	double dbValue = 0;
//	for(; iter3 != iter4; iter3++)
//	{
//		dbValue += (*iter3);
//	}
//	dbValue /= vecPowerRsrp.size();
//	conRsrp->rsrp = vecPowerRsrp;
//	conRsrp->dbMean = dbValue;
//	conRsrp->neNB = stuRsrp->neNB;
//}

double CSimCal::CalMean(vector<double>* pV)
{
	vector<double>::iterator iter1 = pV->begin();
	vector<double>::iterator iter2 = pV->end();
	double dbValue = 0;
	for(; iter1 != iter2; iter1++)
	{
		dbValue += (*iter1);
	}
	dbValue /= pV->size();
	return dbValue;
}

bool CSimCal::KNNv2(int nK, vector<TmpResult>* pVec, KNNCor *pKNNcor)
{

    if(pVec->size() <= 0)
		return false;
    sort(pVec->begin(), pVec->end(), compare);
    vector<GetCoor> vecCor;

    if( pVec->size() > 1 )
    {
        vector<TmpResult>::iterator iter1 = pVec->begin();
        vector<TmpResult>::iterator iter2 = pVec->end();
        for (int i = 0; i != nK; iter1++,i++ )
        {
            vector<Signatrue>::iterator iter3 = m_SigInfoList->begin();
            vector<Signatrue>::iterator iter4 = m_SigInfoList->end();
            for(; iter3 != iter4; iter3++)
            {
                if ((*iter1).SigID == (*iter3).nId)
                {
                    GetCoor tmpCor;
                    tmpCor.TrainSigX = (*iter3).nX;
                    tmpCor.TrainSigY = (*iter3).nY;
                    vecCor.push_back(tmpCor);
    //                cout << "tmpCor.TrainSigX = " << tmpCor.TrainSigX << endl;
    //                cout << "tmpCor.TrainSigY = " << tmpCor.TrainSigY << endl;
    //                system("pause");
                }
            }
        }
    }
    else
    {
        vector<Signatrue>::iterator iter3 = m_SigInfoList->begin();
        vector<Signatrue>::iterator iter4 = m_SigInfoList->end();
        for(; iter3 != iter4; iter3++)
        {
            if (pVec->at(0).SigID == (*iter3).nId)
            {
                GetCoor tmpCor;
                tmpCor.TrainSigX = (*iter3).nX;
                tmpCor.TrainSigY = (*iter3).nY;
                vecCor.push_back(tmpCor);
//                cout << "tmpCor.TrainSigX = " << tmpCor.TrainSigX << endl;
//                cout << "tmpCor.TrainSigY = " << tmpCor.TrainSigY << endl;
//                system("pause");
            }
        }
    }

    double dbX = 0;
    double dbY = 0;
    vector<GetCoor>::iterator iter3 = vecCor.begin();
    vector<GetCoor>::iterator iter4 = vecCor.end();
    for (;iter3 != iter4; iter3++)
    {
        dbX += (*iter3).TrainSigX;
        dbY += (*iter3).TrainSigY;
    }
    pKNNcor->dbX = dbX / vecCor.size();
    pKNNcor->dbY = dbY / vecCor.size();


}
