#include "Debug.h"

CDebug::CDebug(void)
{
}

CDebug::~CDebug(void)
{
}


void CDebug::TestCov()
{
	ifstream fPos("Tmp.txt");
	if(!fPos)
		return ;

	string s;
	//先检索出所有的手机	GetPhoneID();
	vector<vector<double> > ve;
	while( getline(fPos,s) )
	{
		std::vector<std::string> strList;
		strList = split(s,"\"");
		vector<double> v;
		//转换字符串
		double db = atof(strList.at(0).c_str());
		v.push_back(db);
		//时间转换
		db = atof(strList.at(1).c_str());
		v.push_back(db);
		//坐标转换
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

void CDebug::TestDBInsert()
{
    CSigCreate sig;
//	TestCov();

	if(sig.CreateSigStruct())
		cout << "true" << endl;
	else
		cout << "false" << endl;

// 测试地图模块，给数据库插入签名设置一个地图。
    CMatrix matmap;
    matmap.SetMapID(1);
    matmap.SetRange(3000.0, 3000.0, 8000.0, 10000.0);
    matmap.SetSize(40, 40);

	PhoneList* phlist = sig.GetPhoneList();
	CFpCreate fp(phlist);

//#if 0

// 测试数据库的所有插入操作。
	vector<PhoneInfo>::iterator iter1 = phlist->phoneInfoList.begin();
	vector<PhoneInfo>::iterator iter2 = phlist->phoneInfoList.end();
	for (; iter1 != iter2; iter1++)
    {
        int nPhoneID = (*iter1).nPhoneID;

        Signatrue tmp;
        fp.GetSig(nPhoneID,&tmp);//只是一个签名

        CDBoperate dbope;
        dbope.SaveSignature(&tmp);
        dbope.OpenDB("FingerPrintTestSet.db");
        dbope.CreateOneFingerPrint(matmap);
        dbope.CloseDB();

    }
}

void CDebug::TestDBSelect()
{
    CDBoperate dbope;
    dbope.OpenDB("FingerPrint.db");
    dbope.SelectFingerPrint(0,10,1);

//    vector<Signatrue> sigList;
//    sigList = *(dbope.GetSignatrue());

    dbope.CloseDB();
    //测试取出的签名是否正确，取出第一个签名的第一个基站的所有rsrp。
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

void CDebug::TestSimCal()
{

// 测试相似度计算模块功能
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

void CDebug::TestIrregula()
{
    CIrregularGrid CIrre ;//创建一个地图，设置尺寸
    CIrre.SetRange(1000.0, 2600.0, 1000, 865.0);
    CIrre.SetRectSize(10,20,40);
    CIrre.SetBSSize(500);
    CIrre.SetLveSize(3,3,1); // 最后一个参数作为默认标记，1为默认，0会使用前面的倍数
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
