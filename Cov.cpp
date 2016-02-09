
#include "Cov.h"
#include <assert.h>
#include <iostream>
#include   <stdlib.h>

CCov::CCov(void)
{
	m_pDrc = 0;
}


CCov::~CCov(void)
{
	m_Src.clear();
}
void CCov::SetData(vector<vector<double> >* data ,int type)		//type 0表示行向量为单位，1表示列向量为单位
{
	m_pDrc = data;
}

bool CCov::Calculate(int* nRaw, int* nCol)
{
	m_Src.clear();
	m_Mean.clear();
	int nCount = m_pDrc->size();



//***********************************************************
/*

	vector<vector<double> >::iterator iter13 = m_pDrc->begin();
	vector<vector<double> >::iterator iter14 = m_pDrc->end();
	for (int q =0; iter13 != iter14; iter13++,q++)
	{
        cout << q << endl;
        vector<double>::iterator iter15 = (*iter13).begin();
        vector<double>::iterator iter16 = (*iter13).end();
        for (; iter15 != iter16; iter15++)
        {
           cout << (*iter15) << endl;
        }
	}

*/
//*****************************************************************
	vector<vector<double> >::iterator iter1 = m_pDrc->begin();
	vector<vector<double> >::iterator iter2 = m_pDrc->end();
	for(; iter1 != iter2; iter1++)
	{
		double dbValue = GetMean(&(*iter1));
		m_Mean.push_back(dbValue);
	}


	for(int i = 0; i < nCount; i++)
	{
		vector<double> v;
		vector<double>* pV1 = &m_pDrc->at(i);

		for(int j = 0; j < nCount; j++)
		{

			vector<double>* pV2 = &m_pDrc->at(j);
			double dbValue = Cov(pV2,pV1);
			v.push_back(dbValue);
		}
		m_Src.push_back(v);

	}
	*nRaw = m_Src.size();
	*nCol = m_Src.at(0).size();

	return true;
}

double CCov::Cov(vector<double>* x,vector<double>* y)
{
//	assert(x->size() == y->size());

	double x_mean = GetMean(x);
	double y_mean = GetMean(y);

	double dbValue = 0;
	vector<double>::iterator iterx1 = x->begin();
	vector<double>::iterator iterx2 = x->end();
	vector<double>::iterator itery1 = y->begin();
	vector<double>::iterator itery2 = y->end();

	for(;iterx1 != iterx2;iterx1++,itery1++)
	{

		double dbX = (*iterx1) - x_mean;
		double dbY = (*itery1) - y_mean;
		dbValue += dbX*dbY;
	}
	dbValue /= (x->size() -1);
	return dbValue;
}

double CCov::Cov(vector<double>* x,int nMeanIdx,vector<double>* y,int nMeanIdx1)
{
//		assert(x->size() == y->size());

	double x_mean = m_Mean.at(nMeanIdx);
	double y_mean = m_Mean.at(nMeanIdx1);

	double dbValue = 0;
	vector<double>::iterator iterx1 = x->begin();
	vector<double>::iterator iterx2 = x->end();
	vector<double>::iterator itery1 = y->begin();
	vector<double>::iterator itery2 = y->end();

	for(;iterx1 != iterx2;iterx1++,itery1++)
	{
		double dbX = (*iterx1) - x_mean;
		double dbY = (*itery1) - y_mean;
		dbValue += dbX*dbY;
	}
	dbValue /= (x->size() -1);
	return dbValue;
}

double CCov::GetMean(vector<double>* pV)
{


	vector<double>::iterator iter1 = pV->begin();
	vector<double>::iterator iter2 = pV->end();
	double dbValue = 0;
	for(; iter1 != iter2; iter1++)
	{
		dbValue += (*iter1);
	}
	dbValue /= pV->size();
	return dbValue;
}

double CCov::GetValue(int nRaw, int nCol)
{
	vector<double>* pV = &m_Src.at(nRaw);
	double dbValue = pV->at(nCol);
	return dbValue;
}
