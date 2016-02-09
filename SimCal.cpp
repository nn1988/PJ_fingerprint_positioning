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
                            vector<TmpResult>* pMaDResult)//传进来的是测试签名
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

        // 两个矩阵相减
        VectorXd vcTest_Train;
        vcTest_Train = (cfTest.vecMean_Ut) - (cfTrain.vecMean_Ut);

        // 相减后再转置_.transpose()
        RowVectorXd RvcTest_TrainTp;
        RvcTest_TrainTp = vcTest_Train.transpose();

        // 矩阵的逆
        MatrixXd trainCovIv;
        trainCovIv = (cfTrain.matCov).inverse();

        double td = (cfTrain.matCov).determinant();

//        MatrixXi trainCovIvi;
//        trainCovIvi = trainCovIv.cast<int>();、
//        cout << "SigID = " << UsableTrainSig.nId << endl;
//        cout << "determinant = " << td << endl;
//        cout << "Matrix Cov = " << endl;
//        cout << cfTrain.matCov << endl;
//        cout << "Matrix InverseCov = " << endl;
//        cout << trainCovIv << endl;
//        cout << "float Matrix = " << endl;
//        cout << trainCovIvf << endl;
//        system("pause");


        // 测试矩阵与训练矩阵的逆相乘，。
        MatrixXd mtTestTrainMp;
        mtTestTrainMp = cfTest.matCov * (cfTrain.matCov).inverse();
        int nMtSize = mtTestTrainMp.rows();

        // 得到单位矩阵
        MatrixXd mtIdentity;
        mtIdentity.setIdentity(nMtSize,nMtSize);
        // 再减去单位矩阵
        MatrixXd mtTestTrainMp_I;
        mtTestTrainMp_I = (mtTestTrainMp - mtIdentity.setIdentity(nMtSize,nMtSize));

// 计算MaD相似度，prod的距离必须为正值，但是如果为负值，不影响KLD算法

//****************MaD算法 start********************
        double prod;
        prod = RvcTest_TrainTp * trainCovIv * vcTest_Train;
        if ( prod >= 0)
//        if (true)
        {
//            prod = fabs(prod);// 对prod取绝对值。
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

//****************MaD算法 end**********************
        double mtTrace;
        mtTrace = mtTestTrainMp_I.trace();// 求矩阵的迹
//********************KLD算法 start**********************************
        double mtdeter = (float)mtTestTrainMp.determinant();// 求矩阵的行列式
//        if (true)
        if (mtdeter >= 0)
        {
            TmpResult sigreKLD;
//            mtdeter = fabs(mtdeter); // 对mtdeter取绝对值。
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
//*********************KLD算法 end*********************************
    }
    dataFile.close();
}

bool CSimCal::ConverSig(Signatrue* sig, CalFormat* cf)
{
    vector<stuMean>::iterator iter1 = sig->vecMean.begin();
    vector<stuMean>::iterator iter2 = sig->vecMean.end();

    int nCellID = sig->vecMean.size();

    // 转换基站号和均值变为列向量
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
            // 将vector里面的值，转换到矩阵里面，转换协方差矩阵
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
// 算法改进的一种解决方法
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
                intTrainMean.neNB   = (*iter1).neNB;//测试集和训练可以共用一个基站号。
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

/**************************查看comVecTsetRsrp start************************/
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
/**************************查看comVecTsetRsrp end**************************/
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
                    break;// 比较两个签名的基站号,如果有相等的直接计算下一个基站号
                if (i == (pTrain->vecMean).size())
                {
                    // 判断到最后也没有相等的，就说明至少这个基站号训练签名就没有
                    bIsMacth = 0;
                }
            }
            if (!bIsMacth)
                return false;//只要有一个基站号不相等，函数终止，返回false
        }
    }
    else
    {
        return false;
    }
    return true;// 都匹配成功的话，返回true。

}


bool CSimCal::SimCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum)
{
//*********************************
//  保存计算相似度时每个测试签名与所有训练集比对时产生的结果。
//  这一步只是打开了一个文件
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
        /** 合并基站 **/
//        Signatrue* pTeSig = new Signatrue;
//        MergeCellID(pSig,pTeSig);
//
//        Signatrue* pTRSig = new Signatrue;
//        MergeCellID(&(*iter1),pTRSig);
        /** 合并基站 **/

        /** 不合并基站 **/
        Signatrue* pTestSig;
        Signatrue* pTrainSig;
        /** 不合并基站 **/

        // 检查签名的基站号是否有全部相等
//        bool bIsMacth = CheckCellID(pTeSig, pTRSig);
        bool bIsMacth = CheckCellID(pSig, &(*iter1));

        if (bIsMacth)
        {
            // 如果基站号全部相等，认为基站的匹配度很高。
            // 直接将两个签名全部交给后面程序计算
//            pTestSig = pTeSig;
//            pTrainSig = pTRSig;
            pTestSig = pSig;
            pTrainSig = &(*iter1);
        }
        else
        {
            // 如果不相等直接continue，放弃当前训练签名
            if (i == j )
            {
                // i和j的作用是判断测试签名是否已经跟全部
                // 的训练签名比对过，但是又由于没有基站号全部相等的而一直continue
                if (i == m_SigInfoList->size())
                {
                    // 当发现测试签名在训练集里找不到基站号全部相等的签名就会使用
                    // MeanCellIDCal让测试签名与所有的训练集签名去比对。
                    // 相当于调用了另外一个Mean算法对一个测试签名又计算了一遍，
                    // 所以一旦计算结束，会将结果保存在pSum里面，结束整个SimCellIDCal

                    MeanCellIDCal(pSig,pSum);
                    return false;
                }
            }
            i += 1;
            continue;
        }



        // 下面开始利用均值差计算
        vector<stuMean>::iterator iter3 = pTestSig->vecMean.begin();
        vector<stuMean>::iterator iter4 = pTestSig->vecMean.end();
        vector<stuMean>::iterator iter5 = pTrainSig->vecMean.begin();

        vector<double> vecDiff;// 保存一个测试签名与一个训练签名每个基站均值的差值
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
        GetCoor trainCoor, testCoor; // 测试签名的坐标
        SelTrainCoor.GetCoordinateFromTable((*iter1).nId, &trainCoor);
        SelTrainCoor.GetCoordinateFromTable( pSig->nId, &testCoor);

        CalDistanse(testCoor.TestSigX,
                    testCoor.TestSigY,
                    trainCoor.TrainSigX,
                    trainCoor.TrainSigY,
                    &dbCoor);

        // 中间结果全部保存。
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

        /** 合并基站 **/
//        Signatrue* pTeSig = new Signatrue;
//        MergeCellID(pSig,pTeSig);
//
//        Signatrue* pTRSig = new Signatrue;
//        MergeCellID(&(*iter1),pTRSig);
        /** 合并基站 **/

        Signatrue TestSig;
        Signatrue TrainSig;

        /** 合并基站 **/
//        TestSig = *pTeSig;
//        TrainSig = *pTRSig;
        /** 合并基站 **/

        // 主要的区别就在这个函数上
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

        // 主要的区别就在这个函数上
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

        // 主要的区别就在这个函数上
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
            oneSum.stuVar = stuVar; // 这个是新加的。
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
                intTrainMean.neNB   = (*iter1).neNB;//测试集和训练可以共用一个基站号。
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

// 只传进来一个签名
bool CSimCal::MergeCellID(Signatrue* pSig, Signatrue* pMergeSig)
{
    int nThreeSectorCell = 171; // 目前的基站个数是固定的，所以设置成了常数。

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
