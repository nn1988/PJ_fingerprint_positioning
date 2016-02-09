
#include "SigCreate.h"
#include   <stdlib.h>
#include   <fstream>
#include   <iostream>

CSigCreate::CSigCreate(void)
{

}


CSigCreate::~CSigCreate(void)
{

}

bool CSigCreate::CreateSigStruct()
{

	//先检索出所有的手机ID，以便之后存储方便。可通过UePosition文件进行检索
	GetPhoneID();
	GetCellID(&m_cellInfo);

	ifstream fPos("UePosition.txt");
	if(!fPos)
		return false;
	string s;
	while( getline(fPos,s) )
    {
       UePosition pos;
	  if( GetUePositionPerLine(s.c_str(),&pos))
	  {//读取到一行手机数据
		  GetCoordnateInfo(&pos);
	  }
    }
	fPos.close();


	//基本结构建立，读取cell信息
	ifstream fCell("UePhyReport.txt");
	if(!fCell)
		return false;

	string str;
	while(getline(fCell,str))
	{
		UePhyReport rpt;
		GetUePhyReportPerLine(str.c_str(),&rpt);
		CellInfo* pC = GetCellInfo(rpt.nPhoneID,rpt.nID);

		if(!pC)
		{
			return false;
		}
		RangeCellInfo info;
		info.dbTime = rpt.dbTime;
		info.rsrpInfo = rpt.dbRsrp;

		pC->range.push_back(info);
//		pC->nID

		cout<< str.c_str()<<endl;
	}


	fCell.close();
	return true;

}

void CSigCreate::GetPhoneID()
{

	ifstream fPos("UePosition.txt");
	if(!fPos)
		return;

	string s;
	//先检索出所有的手机ID，以便之后存储方便。可通过UePosition文件进行检索


	while( getline(fPos,s) )
    {
       UePosition pos;
	  if( GetUePositionPerLine(s.c_str(),&pos))
	  {//读取到一行手机数据
		  if(CheckPhoneIDExist(pos.nPhoneID))
		  {
			  fPos.close();
			  return;
		  }
		  PhoneInfo info;
		  info.nPhoneID = pos.nPhoneID;
		  m_structInfoList.phoneInfoList.push_back(info);
	  }

    }
	fPos.close();
}

void CSigCreate::GetCellID(vector<CellInfo>* pCellInfo)			//cellID
{


	for(int i = 1; i <= 207; i++)
    {
		CellInfo info;
		info.nID = i;
		pCellInfo->push_back(info);
    }


	ifstream fPos("UePhyReport.txt");
	if(!fPos)
		return;

	string s;
	//先检索出所有的手机ID，以便之后存储方便。可通过UePosition文件进行检索


	while( getline(fPos,s) )
    {
       UePhyReport pos;
	  if( GetUePhyReportPerLine(s.c_str(),&pos))
	  {//读取到一行手机数据
		  if(CheckCellIDExits(pCellInfo,pos.nID))
		  {
			  fPos.close();
			  return;
		  }
		  CellInfo info;
		  info.nID = pos.nID;
		  pCellInfo->push_back(info);
	  }

    }
	fPos.close();

}

bool CSigCreate::GetCoordnateInfo(UePosition* pPos)
{
	vector<CoordnateInfo>* pCoordnateList = GetCoordnateInfoList(pPos->nPhoneID);
	if(!pCoordnateList)
		return false;

	CoordnateInfo info;
	info.nID = pPos->nPhoneID;
	info.nX = pPos->dbX;
	info.nY = pPos->dbY;
	info.nTime = pPos->ntime;

	//GetCellInfo(&info.cellInfo,info.nID);//获取cell信息
	info.cellInfo =  m_cellInfo;
	pCoordnateList->push_back(info);
	return true;
}

bool CSigCreate::GetCellInfo(vector<CellInfo>* pCellInfo,int nPhoneID)
{

	GetCellID(pCellInfo);		//获取到某个cell
	return true;
}

bool CSigCreate::CheckPhoneIDExist(int nID)
{
	vector<PhoneInfo>::iterator iter1 = m_structInfoList.phoneInfoList.begin();
	vector<PhoneInfo>::iterator iter2 = m_structInfoList.phoneInfoList.end();
	for(; iter1 != iter2; iter1++)
	{
		if((*iter1).nPhoneID == nID)
			return true;
	}
	return false;
}
bool CSigCreate::CheckCellIDExits(vector<CellInfo>* pCellInfo,int nID)
{
	vector<CellInfo>::iterator iter1 = pCellInfo->begin();
	vector<CellInfo>::iterator iter2 = pCellInfo->end();
		for(; iter1 != iter2; iter1++)
	{
		if((*iter1).nID == nID)
			return true;
	}
	return false;
}

bool CSigCreate::GetUePositionPerLine(const char* line,UePosition* pPos)
{
	vector<string> strList = split(line,"\"");

	if(strList.size()<=0)
		return false;
	//转换字符串
	pPos->nPhoneID = atoi(strList.at(0).c_str());

	//时间转换
	pPos->ntime = atoi(strList.at(1).c_str());

	//坐标转换
	pPos->dbX = atof(strList.at(2).c_str());
	pPos->dbY = atof(strList.at(3).c_str());

	return true;
}

bool CSigCreate::GetUePhyReportPerLine(const char* line,UePhyReport* pRpt)
{
	vector<string> strList = split(line,"\"");

	if(strList.size()<=0)
		return false;
	//转换字符串
	pRpt->dbTime = atof(strList.at(0).c_str());

	//时间转换
	pRpt->nPhoneID = atoi(strList.at(1).c_str());

	//坐标转换
	pRpt->nID = atof(strList.at(2).c_str());
	pRpt->dbRsrp = atof(strList.at(3).c_str());

	return true;
}
std::vector<std::string> CSigCreate::split(std::string str,std::string pattern)
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

vector<CoordnateInfo>* CSigCreate::GetCoordnateInfoList(int nPhoneID)
{
	vector<PhoneInfo>::iterator iter1 = m_structInfoList.phoneInfoList.begin();
	vector<PhoneInfo>::iterator iter2 = m_structInfoList.phoneInfoList.end();
	for(; iter1 != iter2; iter1++)
	{
		if((*iter1).nPhoneID == nPhoneID)
			return &(*iter1).coordnateInfo;
	}
	return NULL;
}

vector<CellInfo>* CSigCreate::GetCellInfoList(int nPhoneID)
{
	vector<CoordnateInfo>* p = GetCoordnateInfoList(nPhoneID);
	if(p)
		return &(p->at(0).cellInfo);
	return NULL;
}

CellInfo*	CSigCreate::GetCellInfo(int nPhoneID,int nID)
{
	vector<CellInfo>* p = GetCellInfoList(nPhoneID);
	vector<CellInfo>::iterator iter1 = p->begin();
	vector<CellInfo>::iterator iter2 = p->end();
	for(; iter1 != iter2; iter1++)
	{
		if((*iter1).nID == nID)
			return &(*iter1);
	}
	return NULL;
}
