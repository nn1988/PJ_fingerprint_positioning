#ifndef CIRREGULARGRID_H
#define CIRREGULARGRID_H
#include "dataDef.h"
#include "SimCal.h"
#include "Matrix.h"
#include <iostream>

using namespace std;

class CIrregularGrid
{
    public:
        CIrregularGrid();
        virtual ~CIrregularGrid();




    public:
        void SetMapID(int mapID);//�����ͼ��
        int GetMapID(){return m_mapID; };

        void SetRange(double x1,double y1,double x2, double y2); //�������꣬��������
        void SetBSSize(double dbSpace);			//���û�վ���
        void SetLveSize(double dbLveTwo, double dbLveThree, int nDefault); // �Ƿ�ʹ��Ĭ�ϴ�С
        void SetRectSize(int nLveOne, int nLveTwo, int nLveThree); // ������������ĳߴ��С

        UnitRect GetRect(double x, double y, int nTag);
        bool CalTotalNumOfGrid();
        bool GetNumOfGridHaveSig();
        bool GetCorOfAllGrid();

    protected:
        UnitRect GetRecInHexagon(double dbX, double dbY, HexagonPoint* pHP, int nTag);
        bool GetHexagonPoint(double dbX, double dbY, int nTag, HexagonPoint* pHP, int nCol, int nRaw);
        double CalDisToPoint(double dbX, double dbY, HexagonPoint* pHP);
        UnitRect CalRecLve_1(double dbX, double dbY, int nTag);
        UnitRect CalRecLve_2(double dbX, double dbY);
        bool LveTwoRect_1(double dbX, double dbY, int nCol, int nRaw);
        bool LveTwoRect_2(double dbX, double dbY, int nCol, int nRaw);
        bool LveThreeRect_1(double dbX, double dbY, int nCol, int nRaw);
        bool LveThreeRect_2(double dbX, double dbY, int nCol, int nRaw);
        UnitRect GetRecLveOne(double dbX, double dbY);
        UnitRect GetRecLveTwo(double dbX, double dbY);
        UnitRect GetRecLveThree(double dbX, double dbY);
        bool CalLveOne();
        bool CalLveTwo();
        bool CalLveThree();



    protected:
        double m_X;
        double m_Y;

        int m_nDefault;
        double m_top;
        double m_left;
        double m_right;
        double m_bottom;

        int m_mapID;

        double m_RectSizeLveOne;
        double m_RectSizeLveTwo;
        double m_RectSizeLveThree;

        // ��վ������Ҳ����һ��������
        double m_dbBSSpace; // ��վ�����εĿ�
        double m_dbRectWidth; // ��վ�����εĿ�
        double m_dbRectHeight; // ��վ�������϶��㵽�¶���ľ���
        double m_dbHexagonSide; // ��վ�����εı߳�
        double m_dbLveOneHexWight; //

//        double m_nCol;
//        double m_nRaw; // ����Ҫ��Ա����

        double m_dbLveTwoHexShort;
        double m_dbLveTwoHexSide;

        double m_dbLveThreeHexShort;
        double m_dbLveThreeHexSide;

        double m_dbLveOneTop;
        double m_dbLveOneleft;
        double m_dbLveOneright;
        double m_dbLveOnebottom;

        double m_dbLveTwoTop;
        double m_dbLveTwoleft;
        double m_dbLveTworight;
        double m_dbLveTwobottom;

        double m_dbLveThreeTop;
        double m_dbLveThreeleft;
        double m_dbLveThreeright;
        double m_dbLveThreebottom;



};

#endif // CIRREGULARGRID_H
