#ifndef __MATRI_H__
#define __MATRI_H__

#include"dataDef.h"



class CMatrix
{
public:
	CMatrix(void);
	~CMatrix(void);
public:
    void SetMapID(int mapID);//�����ͼ��
    int GetMapID(){return m_mapID; };
	void SetRange(double x1,double y1,double x2, double y2);//�������꣬��������
	void SetSize(double dbWidth,double dbHeight);			//���ÿ��
	int GetRaw();		//����
	int GetCol();		//����
	void GetRange(double* x1,double* y1,double* x2, double* y2);//��ȡ��������ֵ
	UnitRect GetRect(int nRaw,int nCol, double x, double y);						//��ȡ����
	UnitRect GetRect(double x,double y);						//��ȡ����
protected:
	double m_top;
	double m_left;
	double m_right;
	double m_bottom;

	double m_dbWidth;
	double m_dbHeight;

	int m_nCol;
	int m_nRaw;

	int m_mapID;

};

#endif
