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
	//�ȼ��������е��ֻ�	GetPhoneID();
	vector<vector<double> > ve;
	while( getline(fPos,s) )
	{
		std::vector<std::string> strList;
		strList = split(s,"\"");
		vector<double> v;
		//ת���ַ���
		double db = atof(strList.at(0).c_str());
		v.push_back(db);
		//ʱ��ת��
		db = atof(strList.at(1).c_str());
		v.push_back(db);
		//����ת��
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

// ���Ե�ͼģ�飬�����ݿ����ǩ������һ����ͼ��
    CMatrix matmap;
    matmap.SetMapID(1);
    matmap.SetRange(3000.0, 3000.0, 8000.0, 10000.0);
    matmap.SetSize(40, 40);

	PhoneList* phlist = sig.GetPhoneList();
	CFpCreate fp(phlist);

//#if 0

// �������ݿ�����в��������
	vector<PhoneInfo>::iterator iter1 = phlist->phoneInfoList.begin();
	vector<PhoneInfo>::iterator iter2 = phlist->phoneInfoList.end();
	for (; iter1 != iter2; iter1++)
    {
        int nPhoneID = (*iter1).nPhoneID;

        Signatrue tmp;
        fp.GetSig(nPhoneID,&tmp);//ֻ��һ��ǩ��

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
    //����ȡ����ǩ���Ƿ���ȷ��ȡ����һ��ǩ���ĵ�һ����վ������rsrp��
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

// �������ƶȼ���ģ�鹦��
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
    CIrregularGrid CIrre ;//����һ����ͼ�����óߴ�
    CIrre.SetRange(1000.0, 2600.0, 1000, 865.0);
    CIrre.SetRectSize(10,20,40);
    CIrre.SetBSSize(500);
    CIrre.SetLveSize(3,3,1); // ���һ��������ΪĬ�ϱ�ǣ�1ΪĬ�ϣ�0��ʹ��ǰ��ı���
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
