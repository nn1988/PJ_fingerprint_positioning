#include "FpCreate.h"
#include "Cov.h"
#include "Matrix.h"
#include <iostream>


CFpCreate::CFpCreate(PhoneList* phoneList)
{
	m_pPhoneList = phoneList;
}

CFpCreate::~CFpCreate(void)
{
	m_pPhoneList = 0;
}
PhoneInfo* CFpCreate::GetPhoneFromID(int PhoneID)
{

	//m_pPhoneList
	//vector<CellInfo>* p = GetCellInfoList(nPhoneID);
	vector<PhoneInfo>::iterator iter1 = m_pPhoneList->phoneInfoList.begin();
	vector<PhoneInfo>::iterator iter2 = m_pPhoneList->phoneInfoList.end();
	for(; iter1 != iter2; iter1++)
	{
		if((*iter1).nPhoneID == PhoneID)
			return &(*iter1);
	}
	return NULL;

}
bool CFpCreate::GetSig(int PhoneID,Signatrue* pSig)
{

	PhoneInfo* pInfo = GetPhoneFromID(PhoneID);
	pSig->nId = PhoneID;
	if(pInfo == NULL)
		return false;

	CoordnateInfo* pCoorInfo = &(pInfo->coordnateInfo.at(0));
	if(pCoorInfo == NULL)
		return false;

	vector<int > veceNB;//保存基站号
	vector<vector<double> > vecCov;

	vector<CellInfo>::iterator iter1 = pCoorInfo->cellInfo.begin();
	vector<CellInfo>::iterator iter2 = pCoorInfo->cellInfo.end();

	for(;iter1 != iter2; iter1++)
	{
		CellInfo* pCellInfo = &(*iter1);

		//开始进行基站的判断
		vector<RangeCellInfo>::iterator iter3 = pCellInfo->range.begin();
		vector<RangeCellInfo>::iterator iter4 = pCellInfo->range.end();
		vector<double> vecRsrp;
		bool bIsCorrectValue = true;
		for(; iter3 != iter4;iter3++)
		{
			RangeCellInfo* pRange = &(*iter3);

			if(pRange->rsrpInfo == 0)
			{
				bIsCorrectValue = false;
				return false;
			}
			vecRsrp.push_back(pRange->rsrpInfo);



		}

		if(bIsCorrectValue)
		{
			veceNB.push_back(pCellInfo->nID);
			vecCov.push_back(vecRsrp);


		}

	}
	if(vecCov.size() <= 0)
		return false;
	//veceNB,vecCov

//**************************************************************

    vector<vector<double> > vecCovI;
    vector<int > veceNBI;

	vector<vector<double> >::iterator iter13 = vecCov.begin();
	vector<vector<double> >::iterator iter14 = vecCov.end();
	vector<int>::iterator iter17 = veceNB.begin();
	for (; iter13 != iter14; iter13++,iter17++)
	{
/*
	    cout << "size() = " << (*iter13).size() << endl;
	    vector<double>::iterator iter15 = (*iter13).begin();
	    vector<double>::iterator iter16 = (*iter13).end();
	    for (; iter15 != iter16; iter15++)
        {
            cout << (*iter15) << endl;
        }
system("pause");
*/
        if ((*iter13).size() > 0)
        {
            vecCovI.push_back((*iter13));
            veceNBI.push_back((*iter17));
        }
	}




//	cout << "siez " <<  vecCovI.size() << endl;

//***************************************************************

	CCov cov;
	cov.SetData(&vecCovI);
	int nRaw = 0,nCol = 0;
	if(!cov.Calculate(&nRaw,&nCol))
		return false;

	vector<vector<double> >* pCovResult = cov.GetResult();
	vector<double>* pMeanResult = cov.GetMean();


//	//Signatrue* pSig
//
//	struct stuMean
//{
//	int neNB;
//	double dbMean;
//};
//struct Signatrue
//{
//	int nId;
//	vector<stuMean>			vecMean;
//	vector<vector<double> > vecCov;
//	double nX;
//	double nY;
//

//    CMatrix maptomap;
//    maptomap.SetRange(3000.0, 30000.0, 8000.0, 10000.0);
//   maptomap.SetSize(40.0, 40.0);

//    UnitRect uRect;
//    uRect = maptomap.GetRect(pCoorInfo->nX, pCoorInfo->nY);


//	pSig->nX = uRect.centerX;
//	pSig->nY = uRect.centerY;

	pSig->nX = pCoorInfo->nX;
	pSig->nY = pCoorInfo->nY;



	pSig->vecCov = *pCovResult;

//		vector<int > veceNB;//保存基站号
//		vector<double>* pMeanResult = cov.GetMean();


	vector<int>::iterator iter5 = veceNBI.begin();
	vector<int>::iterator iter6 = veceNBI.end();
	vector<double>::iterator iter7 = pMeanResult->begin();

	vector<vector<double> >::iterator iter8 = vecCovI.begin();//后加的，要把原始的rsrp值也都存起来。

	pSig->vecMean.clear();

	for(; iter5 != iter6; iter5++, iter7++, iter8++)
	{
		stuMean mean;
		mean.neNB = (*iter5);
		mean.dbMean = (*iter7);
		mean.rsrp = (*iter8);
		pSig->vecMean.push_back(mean);

	}

	//CellInfo* pCellInfo = &(pCoorInfo->cellInfo);

	return false;

}
