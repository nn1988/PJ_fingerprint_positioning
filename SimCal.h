#ifndef CSIMCAL_H
#define CSIMCAL_H

#include "dataDef.h"
#include "sqlite3.h"
#include "Matrix.h"
#include "IrregularGrid.h"
#include "eigen3/Eigen/Dense"

using namespace Eigen;

struct MinMean
{
    double dbWei;
    double dbDiff;
    MinMean()
    {
        dbWei = 0;
        dbDiff = 0;
    }
};


struct CalFormat
{
    VectorXi vecCellID_It;
    VectorXd vecMean_Ut;
    MatrixXd matCov;

};
/**  所有的相似度算法遵循原则  **/
/** 输入一个测试签名指针 Signatrue* 输出一个中间值容器指针 vector<TmpResult>*  **/


class CSimCal
{
    public:
        CSimCal(vector<Signatrue>* sigInfoList);// 要把数据库取出来的签名都传进来。
        virtual ~CSimCal();
    public:

        bool SimilarityCal(Signatrue* sig1,               // MaD和KLD传进来一个测试签名，会跟所有的训练签名做比对。
                           vector<TmpResult>* pKLDResult,
                           vector<TmpResult>* pMaDResult);
        bool GetMin(vector<TmpResult>* pVec,TmpResult *pDb); // 取得最小值,里面的SigID是训练集的
        bool GetMax(vector<TmpResult>* pVec,TmpResult *pDb); // 取得最大值,里面的SigID是训练集的
        bool GetVarMin(vector<TmpResult>* pVec,TmpResult *pDb, int nThresholdVar);

        bool CalDistanse(double x1, double y1, // 计算两个坐标之间的距离
                         double x2, double y2,
                         double* pD);
        bool CalBestCoor(Signatrue* sig1, BestCoor* bc); // 取得最近距离的坐标
        bool SimCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum); // 均值最小算法
        bool LOG_CreatSimCalReasult(Signatrue* pSig,
                                     vector<TmpResult>* pTmpKLD,
                                     vector<TmpResult>* pTmpMaD);
        bool KNNv2(int nK, vector<TmpResult>* pVec,KNNCor *pKNNcor);


        bool MeanCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum); // 功能同SimCellIDCal相同，不同点在于测试签名会和所有训练集计算。
        bool CellIDCompareCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool VarCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool PearsonCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool WeightingCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool EuclideanCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool KNNCal(Signatrue* pSig,vector<TmpResult>* pSum);
    protected:

        bool MergeCellID(Signatrue* pSig, Signatrue* pMergeSig); // 找到属于同一基站的三个扇区
        bool MergeThreeCellInfo(vector<stuMean>* pVecStuMean, stuMean* mean); // 合并三个扇区
        double SumDiffCal(vector<double>* vecDiff); // 计算均值差的和
        double WeghtDiffCal(vector<double>* vecDiff, vector<stuMean>* TestSigMean); //
        double MultiDiffCal(vector<double>* vecDiff); // 计算均差的乘积的倒数
        double SumWeighing(vector<CGIMeanDiff>* vecCGI, vector<stuMean>* TestSigMean); // 加权算法(有问题，待解决)
        bool SumVar(vector<double>* vecDiff, VarMin* stuVal); // 方差过滤
        double CalVar(vector<double>* vecDiff); // 计算方差
        double Euclidean(vector<double>* vecDiff); // 计算欧式距离

        bool CompareDiff(Signatrue* testSig, Signatrue* trainSig,// 比较两个转换后的签名不一样的元素，主要是比较CellID。
                         Signatrue* UsableTestSig,
                         Signatrue* UsableTrainSig);
        bool CheckCellID(Signatrue* pTestSig, Signatrue* pTrainSig); // 检查两个签名的基站号是不是全部相等
        bool ConverSig(Signatrue* sig, CalFormat* cf);//转换签名成可以计算的向量和矩阵

        // 比对两个签名，把基站号相同的部分取出来存放到两个新的签名里面
        bool MeanMachCal(Signatrue* testSig,
                         Signatrue* trainSig,
                         Signatrue* UsableTestSig,
                         Signatrue* UsableTrainSig);

        double ConvertToW(double dbDBm);
        bool CalPowerMean(stuMean* stuRsrp, stuMean* conRsrp);
        double CalMean( vector<double>* pV);
        bool CellIDCompareCal_two(Signatrue* pSig,vector<TmpResult>* pSum);
        bool CellIDCompareCal_one(Signatrue* pSig,vector<TmpResult>* pSum);
    protected:
        vector<Signatrue>* m_SigInfoList;
        double m_dbTotal;

};

#endif // CSIMCAL_H
