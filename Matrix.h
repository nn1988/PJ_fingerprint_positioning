#ifndef __MATRI_H__
#define __MATRI_H__

#include"dataDef.h"



class CMatrix
{
public:
	CMatrix(void);
	~CMatrix(void);
public:
    void SetMapID(int mapID);//输入地图号
    int GetMapID(){return m_mapID; };
	void SetRange(double x1,double y1,double x2, double y2);//左上坐标，右下坐标
	void SetSize(double dbWidth,double dbHeight);			//设置宽高
	int GetRaw();		//几行
	int GetCol();		//几列
	void GetRange(double* x1,double* y1,double* x2, double* y2);//获取真正坐标值
	UnitRect GetRect(int nRaw,int nCol, double x, double y);						//获取区域
	UnitRect GetRect(double x,double y);						//获取区域
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
