#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <objbase.h>
#include <algorithm>
#include "DBoperate.h"
#include "Matrix.h"

using namespace std;


std::vector<std::string> split(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//扩展字符串以方便操作
	int size=str.size();
	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}


static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


CDBoperate::CDBoperate()
{

}

CDBoperate::~CDBoperate()
{
    m_Signatrue = 0;
    m_SaveSigID = 0;
    m_SaveCoordinateID = 0;
    m_SignatureList.clear() ;
}

bool CDBoperate::SaveSignature(Signatrue* sig)
{
    m_Signatrue = sig;
}

bool CDBoperate::OpenDB(char*path)
{
    int res = sqlite3_open(path, &pDB);

    if (res)
    {
        cout << "Can't open database: " << sqlite3_errmsg(pDB) << endl;
        return false;
    }
}

bool CDBoperate::CloseDB()
{
    int res = sqlite3_close(pDB);

    if (res);
    {
//        cout << "close database false." << sqlite3_errmsg(pDB) << endl;
        return false;
    }
    cout << "success to close." << endl;
    return true;

}

bool CDBoperate::CreateTables(char* tableName)
{
    int res = sqlite3_exec(pDB, tableName,0, 0, 0);
    if (!res)
        return false;
}

bool CDBoperate::CreateOneFingerPrint(CMatrix matmap)
{
    InsertCoordinate(matmap);
    SelectCoordinateLastLineNcall();
    InsertSig();
    SelectSigLastLineNcall();
    InsertMean(m_SaveSigID);
    InsertLinker();
}

bool CDBoperate::CreateOneFingerPrint(CIrregularGrid matmap)
{
    InsertCoordinate(matmap);
    SelectCoordinateLastLineNcall();
    InsertSig();
    SelectSigLastLineNcall();
    InsertMean(m_SaveSigID);
    InsertLinker();
}

bool CDBoperate::SelectSigLastLine()
{
    string str;
    str = "select * from Sig where SigID = (select max(SigID) from Sig)";

    int res = sqlite3_exec(pDB, str.c_str(), callback, 0, &errmsg);
    if (res != SQLITE_OK)
    {
        cout << "SQL error";
    }
    return true;
}

bool CDBoperate::SelectSigLastLineNcall()
{
    sqlite3_stmt *stmt = NULL;
    const char *err_msg = NULL;

    sqlite3_prepare(pDB, "select * from Sig where SigID = (select max(SigID) from Sig)", -1, &stmt, &err_msg);
    sqlite3_bind_int(stmt, 1, 5);

    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        int col_count = sqlite3_column_count(stmt);

        const char *col_0_name = sqlite3_column_name(stmt, 0); // 获取列名
        int id = sqlite3_column_int(stmt, 0);
        int id_type = sqlite3_column_type(stmt, 0); // 获取列数据类型

        const char *col_1_name = sqlite3_column_name(stmt, 1);
        const unsigned char* keyname = sqlite3_column_text(stmt, 1);

        m_SaveSigID = id;

    }
    sqlite3_finalize(stmt); // 释放sqlite3_stmt
    return true;
}

bool CDBoperate::SelectCoordinateLastLineNcall()
{
    sqlite3_stmt *stmt = NULL;
    const char *err_msg = NULL;

    sqlite3_prepare(pDB, "select CoordinateID from Coordinate where CoordinateID = \
                    (select max(CoordinateID) from Coordinate)", -1, &stmt, &err_msg);
    sqlite3_bind_int(stmt, 1, 5);

    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        int col_count = sqlite3_column_count(stmt);// 有多少个列

        const char *col_0_name = sqlite3_column_name(stmt, 0); // 获取列名
        int id = sqlite3_column_int(stmt, 0);
        m_SaveCoordinateID = id;

    }
    sqlite3_finalize(stmt); // 释放sqlite3_stmt
}

bool CDBoperate::InsertCoordinate(CMatrix mat)
{
    UnitRect coor;
    coor = mat.GetRect(m_Signatrue->nX, m_Signatrue->nY);

    MapCoordinate smap;

    smap.mapID =    mat.GetMapID();
    smap.nID =      m_Signatrue->nId;
    smap.x_center = coor.centerX;
    smap.y_center = coor.centerY;
    smap.leftx =    coor.left;
    smap.lefty =    coor.top;
    smap.rightx =   coor.right;
    smap.righty =   coor.bottom;
    smap.raw =      coor.raw;
    smap.col =      coor.col;
    smap.realX =    coor.realX;
    smap.realY =    coor.realY;

    stringstream strsql;
    strsql << "insert into Coordinate(MapID,raw,col,x1,y1,x2,y2,x_center,y_center,x_real,y_real) values(";
    strsql << smap.mapID << "," ;
    strsql << smap.raw << "," ;
    strsql << smap.col << "," ;
    strsql << smap.leftx << ",";
    strsql << smap.lefty << "," ;
    strsql << smap.rightx << "," ;
    strsql << smap.righty << "," ;
    strsql << smap.x_center << "," ;
    strsql << smap.y_center << "," ;
    strsql << smap.realX << "," ;
    strsql << smap.realY << ")";
    string str = strsql.str();

    int res = sqlite3_exec(pDB, "begin transaction;", 0, 0, &errmsg );

    res = sqlite3_exec(pDB, str.c_str(), 0, 0, &errmsg);
    if(res != SQLITE_OK)
    {
        cout << "error: " << errmsg << endl;
        return false;
    }
    res = sqlite3_exec(pDB, "commit transaction;", 0, 0, &errmsg);
//    cout << "Insert Coordinate successful." << endl;
    return true;
}

bool CDBoperate::InsertCoordinate(CIrregularGrid mat)
{
    UnitRect coor;
    coor = mat.GetRect(m_Signatrue->nX, m_Signatrue->nY,3);

    MapCoordinate smap;

    smap.mapID =    mat.GetMapID();
    smap.nID =      m_Signatrue->nId;
    smap.x_center = coor.centerX;
    smap.y_center = coor.centerY;
    smap.leftx =    coor.left;
    smap.lefty =    coor.top;
    smap.rightx =   coor.right;
    smap.righty =   coor.bottom;
    smap.raw =      coor.raw;
    smap.col =      coor.col;
    smap.realX =    coor.realX;
    smap.realY =    coor.realY;

    stringstream strsql;
    strsql << "insert into Coordinate(MapID,raw,col,x1,y1,x2,y2,x_center,y_center,x_real,y_real) values(";
    strsql << smap.mapID << "," ;
    strsql << smap.raw << "," ;
    strsql << smap.col << "," ;
    strsql << smap.leftx << ",";
    strsql << smap.lefty << "," ;
    strsql << smap.rightx << "," ;
    strsql << smap.righty << "," ;
    strsql << smap.x_center << "," ;
    strsql << smap.y_center << "," ;
    strsql << smap.realX << "," ;
    strsql << smap.realY << ")";
    string str = strsql.str();

    int res = sqlite3_exec(pDB, "begin transaction;", 0, 0, &errmsg );

    res = sqlite3_exec(pDB, str.c_str(), 0, 0, &errmsg);
    if(res != SQLITE_OK)
    {
        cout << "error: " << errmsg << endl;
        return false;
    }
    res = sqlite3_exec(pDB, "commit transaction;", 0, 0, &errmsg);
//    cout << "Insert Coordinate successful." << endl;
    return true;
}

bool CDBoperate::InsertSig()
{
    string covname = CreateCovDatFile();
    string name = "\'" + covname + "\'";

    stringstream strsql;
    strsql << "insert into Sig(Cov) values(";
    strsql << name << ")";
    string str = strsql.str();

    if(!pDB)
    {
        cout << "false do not open the database! ";
        return false;
    }

    int res = sqlite3_exec(pDB, "begin transaction;", 0, 0, &errmsg );

    res = sqlite3_exec(pDB, str.c_str(), 0, 0, &errmsg);
    if(res != SQLITE_OK)
    {
        cout << "error: " << errmsg << endl;
        return false;
    }
    res = sqlite3_exec(pDB, "commit transaction;", 0, 0, &errmsg);

//   cout << "Insert Sig successful!" << endl;
}


bool CDBoperate::InsertMean(int sigid)//最新的版本，直接在插值。
{
    int res = sqlite3_exec(pDB, "begin transaction;", 0, 0, &errmsg );

    vector<stuMean> vecmean;
    vecmean = m_Signatrue->vecMean;

    vector<stuMean>::iterator iter1 = vecmean.begin();
    vector<stuMean>::iterator iter2 = vecmean.end();

    for(; iter1 != iter2; iter1++)
    {
        vector<double> rsrp = (*iter1).rsrp;
        if ( rsrp.size() == 5 )// 只有5次都能测到值，才能存起来
        {
            stringstream strsql;
            strsql << "insert into Mean(SigID,CellID,Mean,Rsrp1,Rsrp2,Rsrp3,";
            strsql << "Rsrp4,Rsrp5) values(";
            strsql << sigid << "," << (*iter1).neNB << "," << (*iter1).dbMean;
            strsql << "," << rsrp.at(0) << "," << rsrp.at(1) << "," ;
            strsql << rsrp.at(2) << "," << rsrp.at(3) << "," << rsrp.at(4) << ")";
            string str = strsql.str();

            res = sqlite3_exec(pDB, str.c_str(), 0, 0, &errmsg);
            if(res != SQLITE_OK)
            {
                cout << "error: " << errmsg << endl;
                return false;
            }
        }
    }
    res = sqlite3_exec(pDB, "commit transaction;", 0, 0, &errmsg);

//    cout << "Insert Mean successful!" << endl;
}

bool CDBoperate::InsertLinker()
{
    stringstream strsql;
    strsql << "insert into Linker(CoordinateID, SigID) values(";
    strsql << m_SaveCoordinateID << "," << m_SaveSigID <<  ")";
    string str = strsql.str();

    if(!pDB)
    {
        cout << "false do not open the database! ";
        return false;
    }

    int res = sqlite3_exec(pDB, "begin transaction;", 0, 0, &errmsg );

    res = sqlite3_exec(pDB, str.c_str(), 0, 0, &errmsg);
    if(res != SQLITE_OK)
    {
        cout << "error: " << errmsg << endl;
        return false;
    }
    res = sqlite3_exec(pDB, "commit transaction;", 0, 0, &errmsg);

    cout << "Insert SigID= " << m_SaveSigID << endl;
}

string CDBoperate::CreateCovDatFile()
{
    vector<vector<double> >* pCov;
    pCov = &(m_Signatrue->vecCov);

    GUID guid;

    char str[50];
    char strSigName[50];

    if (S_OK == ::CoCreateGuid(&guid))
    {
        static char buf[64] = {0};
        snprintf(buf, sizeof(buf)
       , "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"
       , guid.Data1
       , guid.Data2
       , guid.Data3
       , guid.Data4[0], guid.Data4[1]
       , guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
       , guid.Data4[6], guid.Data4[7]
          );
        sprintf(str,"cov\\Cov_%s.dat",buf);
        sprintf(strSigName,"Cov_%s.dat",buf);

    }

    ofstream dataFile;
    dataFile.open(str, ios_base::out);
    if (!dataFile)
    {
        cout << "Create false !" << endl;
        return false;
    }

    vector<vector<double> >::iterator iter1 = pCov->begin();
	vector<vector<double> >::iterator iter2 = pCov->end();

	for (; iter1 != iter2; iter1++ )
    {
        vector<double> data = *iter1;
        vector<double>::iterator iter3 = data.begin();
        vector<double>::iterator iter4 = data.end();
        for (; iter3 != iter4; iter3++)
        {
            dataFile << *iter3 << " ";
        }
        dataFile << endl;

    }

    dataFile.close();
    return strSigName;
}

bool CDBoperate::SelectFingerPrint(int beginrow, int nRow, int nTag)
{
    SelectLinker(beginrow,nRow);
    SelectSig();
    SelectMean();

// 取坐标：测试签名的
    vector<int>::iterator iter1 = m_TmpSigID.begin();
    vector<int>::iterator iter2 = m_TmpSigID.end();
    for (; iter1 != iter2; iter1++)
    {
        GetCoor cooRe;
        GetCoordinateFromTable((*iter1),&cooRe);
        vector<Signatrue>::iterator iter3 = m_SignatureList.begin();
        vector<Signatrue>::iterator iter4 = m_SignatureList.end();

        for (; iter3 != iter4; iter3++)
        {
            if ((*iter3).nId == (*iter1))
            {
                if ( nTag == 1)
                {
                    (*iter3).nX = cooRe.TestSigX;
                    (*iter3).nY = cooRe.TestSigY;
                }
                else
                {
                    (*iter3).nX = cooRe.TrainSigX;
                    (*iter3).nY = cooRe.TrainSigY;
                }
            }
        }
    }
}

bool CDBoperate::SelectSig()
{
    sqlite3_stmt *stmt = NULL;
    const char *err_msg = NULL;

    vector<int>::iterator iter1 = m_TmpSigID.begin();
    vector<int>::iterator iter2 = m_TmpSigID.end();

    for (; iter1 != iter2; iter1++)
    {
        stringstream sqlstr;
        sqlstr << "select * from Sig where SigID=";
        sqlstr << (*iter1) << ";" ;
        string str = sqlstr.str();

        sqlite3_prepare(pDB, str.c_str(), -1, &stmt, &err_msg);
        sqlite3_bind_int(stmt, 1, 5);

        while (SQLITE_ROW == sqlite3_step(stmt))
        {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char*  CovTmpName = sqlite3_column_text(stmt, 1);

            vector<vector<double> > oneVecCov;
            bool b = ReadCovFromDat(CovTmpName, &oneVecCov);//从dat文件读取
            if (!b)
            {
                cout << "ReadCovFromDat error！" << endl;
                return false;
            }
            vector<Signatrue>::iterator iter3 = m_SignatureList.begin();
            vector<Signatrue>::iterator iter4 = m_SignatureList.end();

            for (; iter3 != iter4; iter3++)
            {
                if ((*iter3).nId == (*iter1))
                {
                    (*iter3).vecCov = oneVecCov;

                }
            }
        }
    }
    return true;
}

bool CDBoperate::ReadCovFromDat(const unsigned char* CovName,
                                vector<vector<double> >* pOneVecCov)
{
    stringstream covnamestr;
    covnamestr << "cov/" << CovName;
    string str = covnamestr.str();
    char name[50] = {"0"};
    strcpy(name,str.c_str());
    ifstream fPos(name);
	if(!fPos)
    {
        return false;
    }

	string s;

	while( getline(fPos,s) )
    {
        vector<string> strList = split( s ," ");
        vector<double> Cov;
        if(strList.size()<=0)
            return false;
        for (int i = 0; i != strList.size(); i++)
        {
            Cov.push_back( atof(strList.at(i).c_str()) );
        }
        pOneVecCov->push_back(Cov);
    }

	fPos.close();
	return true;
}


bool CDBoperate::SelectLinker(int beginrow, int nRow)
{
    sqlite3_stmt *stmt = NULL;
    const char *err_msg = NULL;

    m_SignatureList.clear() ;

    stringstream sqlstr;
    sqlstr << "select * from Linker limit ";
    sqlstr << beginrow << "," << nRow << ";" ;
    string str = sqlstr.str();

    sqlite3_prepare(pDB, str.c_str(), -1, &stmt, &err_msg);
    sqlite3_bind_int(stmt, 1, 5);

    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        int col_count = sqlite3_column_count(stmt);// 有多少个列
        int id = sqlite3_column_int(stmt, 0);
        int  coordinateID = sqlite3_column_int(stmt, 1);
        int SigID = sqlite3_column_int(stmt, 2);

        Signatrue sig;
        sig.nId = SigID;

        m_TmpSigID.push_back(SigID);
        m_SignatureList.push_back(sig);
    }

    sqlite3_finalize(stmt); // 释放sqlite3_stmt
    return true;
}

bool compare(const stuMean& t1, const stuMean& t2)
{
    return t1.dbMean > t2.dbMean;
}

bool CDBoperate::SelectMean() //目前使用的版本，直接取值。
{
    sqlite3_stmt *stmt = NULL;
    const char *err_msg = NULL;

    vector<int>::iterator iter1 = m_TmpSigID.begin();
    vector<int>::iterator iter2 = m_TmpSigID.end();

    for (; iter1 != iter2; iter1++)
    {
        stringstream sqlstr;
        sqlstr << "select * from Mean where SigID=";
        sqlstr << (*iter1) << ";" ;
        string str = sqlstr.str();

        sqlite3_prepare(pDB, str.c_str(), -1, &stmt, &err_msg);
        sqlite3_bind_int(stmt, 1, 5);

        vector<stuMean> vecMean;
        while (SQLITE_ROW == sqlite3_step(stmt))
        {
            int   CellID = sqlite3_column_int(stmt, 2);
            double  Mean = sqlite3_column_double(stmt, 3);
            double rsrp1 = sqlite3_column_double(stmt, 4);
            double rsrp2 = sqlite3_column_double(stmt, 5);
            double rsrp3 = sqlite3_column_double(stmt, 6);
            double rsrp4 = sqlite3_column_double(stmt, 7);
            double rsrp5 = sqlite3_column_double(stmt, 8);

            vector<double> rsrp;
            rsrp.push_back(rsrp1);
            rsrp.push_back(rsrp2);
            rsrp.push_back(rsrp3);
            rsrp.push_back(rsrp4);
            rsrp.push_back(rsrp5);

            stuMean mean;
            mean.neNB = CellID;
            mean.dbMean = Mean;
            mean.rsrp = rsrp;
            vecMean.push_back(mean);

        }
//        vector<stuMean> vecSortMean;
//        if (vecMean.size() > 27)
//        {
//            sort(vecMean.begin(), vecMean.end(), compare);
//            for (int i = 0; i <= 27; i++)
//            vecSortMean.push_back(vecMean.at(i));
//        }

        vector<Signatrue>::iterator iter3 = m_SignatureList.begin();
        vector<Signatrue>::iterator iter4 = m_SignatureList.end();

        for (; iter3 != iter4; iter3++)
        {
            if ((*iter3).nId == (*iter1))
            {
                (*iter3).vecMean = vecMean;
            }
        }
        cout << "Select SigID= " << (*iter1) << endl;
    }
}

int CDBoperate::CountSig()
{
    sqlite3_stmt *stmt = NULL;
    const char *err_msg = NULL;

    stringstream sqlstr;
    sqlstr << "select count(*) as num from Linker;";
    string str = sqlstr.str();

    sqlite3_prepare(pDB, str.c_str(), -1, &stmt, &err_msg);
    sqlite3_bind_int(stmt, 1, 5);

    while (SQLITE_ROW == sqlite3_step(stmt))
    {
//        int col_count = sqlite3_column_count(stmt);// 有多少条
        int col_count = sqlite3_column_int(stmt, 0);

        return col_count;
    }

    sqlite3_finalize(stmt); // 释放sqlite3_stmt

    return NULL;
}


bool CDBoperate::GetCoordinateFromTable(int sigID, GetCoor* pResultSig)
{
//    cout << "Get Coordinate for SigID = "  << sigID << endl;
    m_CoordinateID = 0;
    sqlite3_stmt *stmt = NULL;
    const char *err_msg = NULL;

    stringstream sqlstr;
    sqlstr << "select * from Linker where SigID=";
    sqlstr << sigID << ";" ;
    string str = sqlstr.str();

    sqlite3_prepare(pDB, str.c_str(), -1, &stmt, &err_msg);
    sqlite3_bind_int(stmt, 1, 5);

    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        int coordinateID = sqlite3_column_int(stmt, 1);
        m_CoordinateID = coordinateID;
    }

    stringstream sqlstr1;
    sqlstr1 << "select * from Coordinate where CoordinateID= ";
    sqlstr1 << m_CoordinateID << ";" ;
    string str1 = sqlstr1.str();

    sqlite3_prepare(pDB, str1.c_str(), -1, &stmt, &err_msg);
    sqlite3_bind_int(stmt, 1, 5);

    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        int  x_Real = sqlite3_column_int(stmt, 10);
        int  y_Real = sqlite3_column_int(stmt, 11);
        int  x_center = sqlite3_column_int(stmt, 8);
        int  y_center = sqlite3_column_int(stmt, 9);

        pResultSig->TrainSigX = x_center;
        pResultSig->TrainSigY = y_center;
        pResultSig->TestSigX = x_Real;
        pResultSig->TestSigY = y_Real;
    }
    return true;
}
