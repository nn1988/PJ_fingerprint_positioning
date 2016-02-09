#include "Matrix.h"


CMatrix::CMatrix(void)
{
	m_top = 0;
	m_left = 0;
	m_right = 0;
	m_bottom = 0;

	m_dbWidth = 0;
	m_dbHeight = 0;

	m_nCol = 0;
	m_nRaw = 0;
	m_mapID = 0;
}


CMatrix::~CMatrix(void)
{
}

void CMatrix::SetMapID(int ID)
{
    m_mapID = ID;
}

void CMatrix::SetRange(double x1,double y1,double x2, double y2)//�������꣬��������
{
	m_top = y1;
	m_left = x1;
	m_right = x2;
	m_bottom = y2;
}
void CMatrix::SetSize(double dbWidth,double dbHeight)			//���ÿ��
{
	m_dbWidth = dbWidth;
	m_dbHeight = dbHeight;
	//����ʵ�ʴ�С
	m_nCol = (int)((m_right - m_left)/dbWidth);	//��
	m_nRaw = (int)((m_top - m_bottom/dbHeight));//��

	m_right = m_left + m_dbWidth*m_nCol;
	m_top = m_bottom + m_dbHeight*m_nRaw;

}
int CMatrix::GetRaw()		//����
{
	return m_nRaw;
}
int CMatrix::GetCol()		//����
{
	return m_nCol;
}
void CMatrix::GetRange(double* x1,double* y1,double* x2, double* y2)//��ȡ��������ֵ
{
	*x1 = m_left;
	*y1 = m_top ;
	*x2 = m_right;
	*y2 = m_bottom;
}
UnitRect CMatrix::GetRect(int nRaw,int nCol, double x, double y)						//��ȡ����
{
	UnitRect rt;
	rt.raw = nRaw;
	rt.col = nCol;
	rt.left = nCol*m_dbWidth + m_left;
	rt.right = rt.left + m_dbWidth;
	rt.bottom = nRaw*m_dbHeight + m_bottom;
	rt.top = rt.bottom + m_dbHeight;
	rt.centerX = ((rt.right - rt.left)/2) + rt.left;
	rt.centerY = ((rt.top - rt.bottom)/2) + rt.bottom;
	rt.realX = x;
	rt.realY = y;
	return rt;
}

UnitRect CMatrix::GetRect(double x,double y)						//��ȡ����
{
	int nCol = (x-m_left)/m_dbWidth;
	int nRaw = (y-m_bottom)/m_dbHeight;
	return GetRect(nRaw,nCol, x, y);
}
