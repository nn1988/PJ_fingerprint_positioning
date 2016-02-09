#ifndef __C_V_H__
#define __C_V_H__
#include <vector>
using namespace std;

class CCov
{
public:
	CCov(void);
	~CCov(void);

public:
	void SetData(vector<vector<double> >* data ,int type = 0);		//type 0表示行向量为单位，1表示列向量为单位
	bool Calculate(int* nRaw, int* nCol);
	double GetValue(int nRaw, int nCol);
	vector<vector<double> > * GetResult(){return &m_Src;};			//返回协方差矩阵
	vector<double>* GetMean(){return &m_Mean;};						//返回均值

protected:
	double Cov(vector<double>* x,vector<double>* y);
	double Cov(vector<double>* x,int nMeanIdx,vector<double>* y,int nMeanIdx1);
	double GetMean(vector<double>* pV);
	vector<vector<double> >* m_pDrc;					//原始数据
	vector<vector<double> > m_Src;						//计算数据
	vector<double> m_Mean;								//均值
};

#endif
