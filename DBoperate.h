#ifndef CDBOPERATE_H
#define CDBOPERATE_H

#include "dataDef.h"
#include "sqlite3.h"
#include "Matrix.h"
#include "IrregularGrid.h"

#pragma comment(lib, "sqlite3.lib")


struct SigTable
{
    int SigID;
    const unsigned char* Cov;
};


class CDBoperate
{
    public:
        CDBoperate();
        virtual ~CDBoperate();

        bool OpenDB(char* path); //打开数据库         ok
        bool CloseDB(); //关闭数据库                  ok
//*************************************************************
        bool CreateOneFingerPrint(CMatrix matmap);  // 插入一个签名
        bool CreateOneFingerPrint(CIrregularGrid matmap);  // 插入一个签名
        // 创建一个空表，测试用。
        // tableName格式"create table Coordinate(MapID,raw,col,x1,y1,x2,y2,x_center,y_center)"
        bool CreateTables(char* tableName);
        // 在向数据库存入签名时使用，载入一个签名进入类。
        bool SaveSignature(Signatrue* sig);
//**************************************************************
        // 以beginrow为起始位置，nRow为数据的条数取出签名, nTag标记取得哪种类型的签名 1为测试，0为训练
        bool SelectFingerPrint(int beginrow, int nRow, int nTag);
        int CountSig();                                  // 返回数据库里签名的总数
        vector<Signatrue>* GetSignatrue(){return &m_SignatureList;}// 对外的接口，返回本次取出的所有签名。
//**************************************************************
        bool GetCoordinateFromTable(int sigID, GetCoor* pResultSig);// 找到对应的坐标值
    protected:
        bool InsertCoordinate(CMatrix mat); //       ok
        bool InsertCoordinate(CIrregularGrid mat); //       ok
        bool InsertSig();
        bool InsertMean(int sigid);   //             ok
        bool InsertLinker();    //                   ok

        bool SelectSigLastLine();        //          ok
        bool SelectSigLastLineNcall();   //          ok
        bool SelectCoordinateLastLineNcall(); //     ok

        bool SelectLinker(int beginrow, int nRow);// ok
        bool SelectSig();//                          ok
        bool SelectMean();//                         ok

        vector<double>* ReadRsrpFromDat(const unsigned char* CovName);
        bool  ReadCovFromDat(const unsigned char* CovName, vector<vector<double> >* pOneVecCov);
        string CreateRsrpDatFile(vector<double> rsrp); //   ok
        string CreateCovDatFile();   //               ok
    protected:
        sqlite3* pDB;
        char* errmsg;
        Signatrue* m_Signatrue;

        int m_SaveSigID;
        int m_SaveCoordinateID;

        vector<Signatrue> m_SignatureList;

        vector<int> m_TmpSigID;
        vector<double> m_TmpCov;
        vector<vector<double> > m_OneVecCov;

        int m_CoordinateID;

};

#endif // CDBOPERATE_H
