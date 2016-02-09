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
/**  ���е����ƶ��㷨��ѭԭ��  **/
/** ����һ������ǩ��ָ�� Signatrue* ���һ���м�ֵ����ָ�� vector<TmpResult>*  **/


class CSimCal
{
    public:
        CSimCal(vector<Signatrue>* sigInfoList);// Ҫ�����ݿ�ȡ������ǩ������������
        virtual ~CSimCal();
    public:

        bool SimilarityCal(Signatrue* sig1,               // MaD��KLD������һ������ǩ����������е�ѵ��ǩ�����ȶԡ�
                           vector<TmpResult>* pKLDResult,
                           vector<TmpResult>* pMaDResult);
        bool GetMin(vector<TmpResult>* pVec,TmpResult *pDb); // ȡ����Сֵ,�����SigID��ѵ������
        bool GetMax(vector<TmpResult>* pVec,TmpResult *pDb); // ȡ�����ֵ,�����SigID��ѵ������
        bool GetVarMin(vector<TmpResult>* pVec,TmpResult *pDb, int nThresholdVar);

        bool CalDistanse(double x1, double y1, // ������������֮��ľ���
                         double x2, double y2,
                         double* pD);
        bool CalBestCoor(Signatrue* sig1, BestCoor* bc); // ȡ��������������
        bool SimCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum); // ��ֵ��С�㷨
        bool LOG_CreatSimCalReasult(Signatrue* pSig,
                                     vector<TmpResult>* pTmpKLD,
                                     vector<TmpResult>* pTmpMaD);
        bool KNNv2(int nK, vector<TmpResult>* pVec,KNNCor *pKNNcor);


        bool MeanCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum); // ����ͬSimCellIDCal��ͬ����ͬ�����ڲ���ǩ���������ѵ�������㡣
        bool CellIDCompareCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool VarCellIDCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool PearsonCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool WeightingCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool EuclideanCal(Signatrue* pSig,vector<TmpResult>* pSum);
        bool KNNCal(Signatrue* pSig,vector<TmpResult>* pSum);
    protected:

        bool MergeCellID(Signatrue* pSig, Signatrue* pMergeSig); // �ҵ�����ͬһ��վ����������
        bool MergeThreeCellInfo(vector<stuMean>* pVecStuMean, stuMean* mean); // �ϲ���������
        double SumDiffCal(vector<double>* vecDiff); // �����ֵ��ĺ�
        double WeghtDiffCal(vector<double>* vecDiff, vector<stuMean>* TestSigMean); //
        double MultiDiffCal(vector<double>* vecDiff); // �������ĳ˻��ĵ���
        double SumWeighing(vector<CGIMeanDiff>* vecCGI, vector<stuMean>* TestSigMean); // ��Ȩ�㷨(�����⣬�����)
        bool SumVar(vector<double>* vecDiff, VarMin* stuVal); // �������
        double CalVar(vector<double>* vecDiff); // ���㷽��
        double Euclidean(vector<double>* vecDiff); // ����ŷʽ����

        bool CompareDiff(Signatrue* testSig, Signatrue* trainSig,// �Ƚ�����ת�����ǩ����һ����Ԫ�أ���Ҫ�ǱȽ�CellID��
                         Signatrue* UsableTestSig,
                         Signatrue* UsableTrainSig);
        bool CheckCellID(Signatrue* pTestSig, Signatrue* pTrainSig); // �������ǩ���Ļ�վ���ǲ���ȫ�����
        bool ConverSig(Signatrue* sig, CalFormat* cf);//ת��ǩ���ɿ��Լ���������;���

        // �ȶ�����ǩ�����ѻ�վ����ͬ�Ĳ���ȡ������ŵ������µ�ǩ������
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
