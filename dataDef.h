# ifndef __DATA_DEF_H__
# define __DATA_DEF_H__

#include <vector>
#include <string>

using namespace std;

/*
==================================================

Data struct read from txt file 

==================================================
*/

// 1 ---- a record of RSRP
struct RangeCellInfo {
	double                 dbTime;            // collecting time of every RSRP
	double                 rsrpInfo;          // RSRP
	RangeCellInfo() {
		dbTime = 0;
		rsrpInfo = 0;
	};
};

// 2 ---- collecting data in a eNB
struct CellInfo {
	int                    nID;               // cellID of every eNB
	vector<RangeCellInfo>  range;             // all data of RSRPs from all eNB
	double                 dbMean;            // mean of all RSRP value
	CellInfo() {
		nID = 0;
		dbMean = 0;
	};
};

// 3 ---- ĳ������ĳ��ʱ������л�վ��Ϣ
struct CoordnateInfo {
	int                    nID;               // coordnate ID
	double                 nX;                // X-axis position of eNB 
	double                 nY;                // Y-axis position of eNB
	int                    nTime;             // collecting time
	vector<CellInfo>       cellInfo;          // detected eNB and their RSRP
    CoordnateInfo() {
		nID = 0;
		nX = 0;
		nY = 0;
		nTime = 0;
	};
};

// ĳ�ֻ�ĳ������Ϣ
struct PhoneInfo{
	int                    nPhoneID;          // Phone ID 
	string                 strDesc;           // some information of this phone 
	vector<CoordnateInfo>  coordnateInfo;     // RSRP collected in different coordinate 
	PhoneInfo() {
		nPhoneID = 0;
	};
};

// ���ݴ�txt��ȡ�����������ֻ���Ϣ
struct PhoneList {
	vector<PhoneInfo>      phoneInfoList;     // all RSRP data read from txt file
};

// һ����ͼ����С�����С
struct UnitRect{
    int                    raw;               // row
    int                    col;               // colnum 
	double                 left;              // left margin coordinate 
	double                 top;               // top margin coordinate
	double                 right;             // right margin coordinate
	double                 bottom;            // bottom margin coordinate
	double                 centerX;           // center X-axis of a rectangle
	double                 centerY;           // center Y-axis of a rectangle
	double                 realX;             // phone X-axis in real position 
	double                 realY;             // phone Y-axis in real position
};

/*
=============================

Data struct of signature

=============================
*/

// һ����վ�ľ�ֵ������rsrp
struct stuMean{
	int                    neNB;              // cellID of a eNB
	double                 dbMean;            // mean value of all RSRPs
	vector<double>         rsrp;              // all RSRPs
	stuMean(){
		neNB = 0;
		dbMean = 0;
	}
};

// 8--------------һ��ǩ���Ľṹ��������rsrp��ԭʼ����
struct Signatrue {
	int                        nId;           // signature ID
	vector<stuMean>		       vecMean;       // list of mean for all eNB
	vector<vector<double> >    vecCov;        // covariance matrix
	double                     nX;            // X-axis: real or training location
	double                     nY;            // Y-axis: real or training location                                  
	Signatrue() {
		nId = 0;
		nX = 0;
		nY = 0;
	}
};


// 9-------------�����ݿ��в����ͼʱʹ�õ���ʱ�ṹ
struct MapCoordinate {
    int                        nID;           // useless rectangle ID
    int                        mapID;         // useless map ID
    int                        raw;           // row number
    int                        col;           // col number
    double                     leftx;         // left margin 
    double                     lefty;         // top or bottom
    double                     rightx;        // right margin
    double                     righty;        // top or bottm
    double                     x_center;      // center X-axis 
    double                     y_center;      // center Y-axis 
    double                     realX;         // real X-axis of signature
    double                     realY;         // real Y-axis of signature
    MapCoordinate(){
        nID = 0;
    }
};

struct VarMin{
    double                     dbVar;
    double                     dbDiff;
    VarMin(){
        dbVar = 0;
        dbDiff = 0;
    }
};

// 10----------------�����ƶȼ��㺯����ȡ�ý������ʱ�ṹ
struct TmpResult
{
    int SigID;
    double CalResult;
    int nCellIDDiff;
    int nTestCell;
    int nTrainCell;
    VarMin stuVar;
    TmpResult()
    {
        CalResult = 0 ;
        SigID = 0;
        nCellIDDiff = 0;
        nTestCell = 0;
        nTrainCell = 0;
    }
};

// 11----------------���ݿ��ȡ����ʱ���ٽṹ
struct GetCoor
{
    int TestSigX;
    int TestSigY;
    int TrainSigX;
    int TrainSigY;
    int BestSigX;
    int BestSigY;
    GetCoor() {
        TestSigX = 0;
    }
};

// 12------------------
struct BestCoor {
    int BestID;
    double CalDisResult;
    BestCoor() {
        BestID = 0;
        CalDisResult = 0;
    }
};

// 13----------------���ʱʹ��
struct FianlSigResult {         // ֻ��һ��ǩ���ıȽ�,ȡ������
    int TestSigID;
    int TrainKldSigID;
    int TrainMaDSigID;
    int BestCoorID;
    double BestSigX;
    double BestSigY;
    double TestSigCoordinateX;
    double TestSigCoordinateY;
    double TrainSigKldX;
    double TrainSigKldY;
    double TrainSigMaDX;
    double TrainSigMaDY;
    double KldResult;
    double MaDResult;
    double BestDisResult;
    FianlSigResult() {
        TestSigID = 0;
    }
};

// 14------------------
struct CellMeanResult {
    int TestSigID;
    int TrainSigID;
    int BestCoorID;
    double BestSigX;
    double BestSigY;
    double TestSigX;
    double TestSigY;
    double TrainSigX;
    double TrainSigY;
    double MeanResult;
    double BestDisResult;
    CellMeanResult() {
        TestSigID = 0;
    }
};

struct ResultSort {
    int TestSigID;
    int TrainSigID;
    int BestCoorID;
    double MeanResult;
    double BestDisResult;
    double MeanDis;
    ResultSort() {
        TestSigID = 0;
    }
};


struct CGIMeanDiff {
    int neNB;
    double dbMean;
    CGIMeanDiff() {
        neNB = 0;
    }
};

struct HexagonPoint {
    double dbX;
    double dbY;
    double dbPointToCenter;
    HexagonPoint() {
        dbX = 0;
        dbY = 0;
        dbPointToCenter = 0;
    }
};

struct KNNCor {
    double dbX;
    double dbY;
};



#endif
