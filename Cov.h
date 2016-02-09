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
	void SetData(vector<vector<double> >* data ,int type = 0);		//type 0��ʾ������Ϊ��λ��1��ʾ������Ϊ��λ
	bool Calculate(int* nRaw, int* nCol);
	double GetValue(int nRaw, int nCol);
	vector<vector<double> > * GetResult(){return &m_Src;};			//����Э�������
	vector<double>* GetMean(){return &m_Mean;};						//���ؾ�ֵ

protected:
	double Cov(vector<double>* x,vector<double>* y);
	double Cov(vector<double>* x,int nMeanIdx,vector<double>* y,int nMeanIdx1);
	double GetMean(vector<double>* pV);
	vector<vector<double> >* m_pDrc;					//ԭʼ����
	vector<vector<double> > m_Src;						//��������
	vector<double> m_Mean;								//��ֵ
};

#endif
