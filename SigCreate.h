#ifndef __SIG_CREATE_H__
#define __SIG_CREATE_H__

#include "dataDef.h"


struct UePosition
{
	int nPhoneID;
	int ntime;
	double dbX;
	double dbY;
};

struct UePhyReport
{
	int nID;
	int nPhoneID;
	double dbTime;
	double dbRsrp;
};

class CSigCreate
{
public:
	CSigCreate(void);
	~CSigCreate(void);

public:
	bool CreateSigStruct();
	PhoneList* GetPhoneList(){return &m_structInfoList;};
protected:
	bool GetUePositionPerLine(const char* line,UePosition* pPos);
	bool GetUePhyReportPerLine(const char* line,UePhyReport* pRpt);
	void GetPhoneID();															//检索出所有的手机信息
	void GetCellID(vector<CellInfo>* pCellInfo);								//cellID
	std::vector<std::string> split(std::string str,std::string pattern);		//分割字符串
	
	bool GetCoordnateInfo(UePosition* pPos);		//读取某个手机的所有坐标信息
	bool GetCellInfo(vector<CellInfo>* pCellInfo,int nPhoneID);//获取cell信息
	bool CheckPhoneIDExist(int nID);	//检测是否已经存在了手机ID
	bool CheckCellIDExits(vector<CellInfo>* pCellInfo,int nID);
	vector<CoordnateInfo>* GetCoordnateInfoList(int nPhoneID);
	vector<CellInfo>* GetCellInfoList(int nPhoneID);
	CellInfo*	GetCellInfo(int nPhoneID,int nID);

protected:
	PhoneList m_structInfoList;
	vector<CellInfo> m_cellInfo;

};

#endif
