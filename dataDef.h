# ifndef __DATA_DEF_H__
# define __DATA_DEF_H__

#include <vector>
#include <string>

using namespace std;

// 1-------------ĳ����վ��ͬrsrp
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

// 2-------------������վrsrp
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

// 3-------------ĳ������ĳ��ʱ������л�վ��Ϣ
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

// 4------------ĳ�ֻ�ĳ������Ϣ
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

// 5------------���ݴ�txt��ȡ�����������ֻ���Ϣ
struct PhoneList
{
	vector<PhoneInfo> phoneInfoList;
};

// 6------------һ����ͼ����С�����С
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

// 7-------------һ����վ�ľ�ֵ������rsrp
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

// 8--------------һ��ǩ���Ľṹ��������rsrp��ԭʼ����
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

// 9-------------�����ݿ��в����ͼʱʹ�õ���ʱ�ṹ
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

// 13----------------���ʱʹ��
struct FianlSigResult// ֻ��һ��ǩ���ıȽ�,ȡ������
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
