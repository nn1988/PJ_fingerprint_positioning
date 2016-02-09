# ifndef __DATA_DEF_H__
# define __DATA_DEF_H__

#include <vector>
#include <string>

using namespace std;

// 1-------------某个基站不同rsrp
struct RangeCellInfo
{
	double dbTime;
	double rsrpInfo;
	RangeCellInfo()
	{
		dbTime = 0;
		rsrpInfo = 0;
	};
};

// 2-------------各个基站rsrp
struct CellInfo
{
	int nID;
	vector<RangeCellInfo> range;
	double dbMean;
	CellInfo()
	{
		dbMean = 0;
	};

};

// 3-------------某坐标下某个时间点所有基站信息
struct CoordnateInfo
{
	int nID;
	double nX;
	double nY;
	int nTime;

	vector<CellInfo> cellInfo;

	CoordnateInfo()
	{
		nID = 0;
		nX = 0;
		nY = 0;
		nTime = 0;
	};
};

// 4------------某手机某座标信息
struct PhoneInfo
{

	int nPhoneID;
	string strDesc;
	vector<CoordnateInfo> coordnateInfo;
	PhoneInfo()
	{
		nPhoneID = 0;
	};
};

// 5------------传递从txt读取进来的所有手机信息
struct PhoneList
{
	vector<PhoneInfo> phoneInfoList;
};

// 6------------一个地图的最小方格大小
struct UnitRect
{
//	int nID;
    int raw;
    int col;
	double left;
	double top;
	double right;
	double bottom;
	double centerX;
	double centerY;
	double realX;
	double realY;
};

//-------------------------------------

// 7-------------一个基站的均值和所有rsrp
struct stuMean
{
	int neNB;
	double dbMean;
	vector<double> rsrp;
	stuMean()
	{
		neNB = 0;
		dbMean = 0;
	}
};

// 8--------------一个签名的结构，包括了rsrp的原始数据
struct Signatrue
{
	int nId;
	vector<stuMean>			vecMean;
	vector<vector<double> > vecCov;
	double nX;
	double nY;
	Signatrue()
	{
		nId = 0;
		nX = 0;
		nY = 0;
	}
};

// 9-------------向数据库中插入地图时使用的临时结构
struct MapCoordinate
{
    int nID;
    int mapID;
    int raw;
    int col;
    double leftx;
    double lefty;
    double rightx;
    double righty;
    double x_center;
    double y_center;
    double realX;
    double realY;
    MapCoordinate()
    {
            nID = 0;
            mapID = 0;
            raw = 0;
            col = 0;
            leftx = 0;
            lefty = 0;
            rightx = 0;
            righty = 0;
            x_center = 0;
            y_center = 0;
            realX = 0;
            realY = 0;
    }

};

struct VarMin
{
    double dbVar;
    double dbDiff;
    VarMin()
    {
        dbVar = 0;
        dbDiff = 0;
    }
};

// 10----------------在相似度计算函数中取得结果的临时结构
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

// 11----------------数据库获取坐标时的临结构
struct GetCoor
{
    int TestSigX;
    int TestSigY;
    int TrainSigX;
    int TrainSigY;
    int BestSigX;
    int BestSigY;
    GetCoor()
    {
        TestSigX = 0;
        TestSigY = 0;
        TrainSigX = 0;
        TrainSigY = 0;
        BestSigX = 0;
        BestSigY = 0;
    }
};

// 12------------------
struct BestCoor
{
    int BestID;
    double CalDisResult;
    BestCoor()
    {
        BestID = 0;
        CalDisResult = 0;
    }
};

// 13----------------输出时使用
struct FianlSigResult// 只是一对签名的比较,取交集。
{
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
    FianlSigResult()
    {
        TestSigID = 0;
        TrainKldSigID = 0;
        TrainMaDSigID = 0;
        BestCoorID = 0;
        BestSigX = 0;
        BestSigY = 0;
        TestSigCoordinateX = 0;
        TestSigCoordinateY = 0;
        TrainSigKldX = 0;
        TrainSigKldY = 0;
        TrainSigMaDX = 0;
        TrainSigMaDY = 0;
        KldResult = 0;
        MaDResult = 0;
        BestDisResult = 0;
    }
};

// 14------------------
struct CellMeanResult
{
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
    CellMeanResult()
    {
        TestSigID = 0;
        TrainSigID = 0;
        BestCoorID = 0;
        BestSigX = 0;
        BestSigY = 0;
        TestSigX = 0;
        TestSigY = 0;
        TrainSigX = 0;
        TrainSigY = 0;
        MeanResult = 0;
        BestDisResult = 0;
    }
};



struct ResultSort
{
    int TestSigID;
    int TrainSigID;
    int BestCoorID;
    double MeanResult;
    double BestDisResult;
    double MeanDis;
    ResultSort()
    {
        TestSigID = 0;
        TrainSigID = 0;
        BestCoorID = 0;
        MeanResult = 0;
        BestDisResult = 0;
        MeanDis = 0;
    }
};



struct CGIMeanDiff
{
    int neNB;
    double dbMean;
    CGIMeanDiff()
    {
        neNB = 0;
        dbMean = 0;
    }
};

struct HexagonPoint
{
    double dbX;
    double dbY;
    double dbPointToCenter;
    HexagonPoint()
    {
        dbX = 0;
        dbY = 0;
        dbPointToCenter = 0;
    }
};

struct KNNCor
{
    double dbX;
    double dbY;
};




#endif
