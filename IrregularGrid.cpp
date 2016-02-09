#include "IrregularGrid.h"




CIrregularGrid::CIrregularGrid()
{
    m_top = 0;
	m_left = 0;
	m_right = 0;
	m_bottom = 0;

	m_mapID = 0;

    m_RectSizeLveOne = 0;
    m_RectSizeLveTwo = 0;
    m_RectSizeLveThree = 0;

    m_dbBSSpace = 0; // 基站六边形的宽
    m_dbRectWidth = 0; // 基站六边形的宽
    m_dbRectHeight = 0;
    m_dbHexagonSide = 0; // 基站六边形的边长

//    m_nCol = 0;
//    m_nRaw = 0; // 不需要成员变量

    m_dbLveOneTop = 0;
    m_dbLveOneleft = 0;
    m_dbLveOneright = 0;
    m_dbLveOnebottom = 0;

    m_dbLveTwoTop = 0;
    m_dbLveTwoleft = 0;
    m_dbLveTworight = 0;
    m_dbLveTwobottom = 0;

    m_dbLveThreeTop = 0;
    m_dbLveThreeleft = 0;
    m_dbLveThreeright = 0;
    m_dbLveThreebottom = 0;
}

CIrregularGrid::~CIrregularGrid()
{
    //dtor
}


void CIrregularGrid::SetRange(double x1,double y1,double x2, double y2)//左上坐标，右下坐标
{
	m_top = y1;
	m_left = x1;
	m_right = x2;
	m_bottom = y2;
}

void CIrregularGrid::SetMapID(int ID)
{
    m_mapID = ID;
}

void CIrregularGrid::SetBSSize(double dbSpace)			//设置宽高
{
    m_dbBSSpace = dbSpace;
	m_dbRectWidth = dbSpace;
	m_dbRectHeight = (sqrt(3)/2)*dbSpace;// 一层参考网格的高度，长度就是基站间距

	m_dbHexagonSide = (sqrt(3)/3)*dbSpace;//六边形基站的边长
	m_dbLveOneHexWight = (dbSpace / 2);
	//计算实际大小
//	m_nCol = (int)((m_right - m_left)/m_dbRectWidth);	//列
//	m_nRaw = (int)((m_top - m_bottom/m_dbRectHeight));//行
//
//	m_right = m_left + m_dbRectWidth*m_nCol;
//	m_top = m_bottom + m_dbRectHeight*m_nRaw;

}

void CIrregularGrid::SetLveSize(double dbLveTwoMulti, double dbLveThreeMulti, int nDefault)
{
//    m_nDefault = nDefault;
    if (nDefault == 1)
    {
        dbLveThreeMulti = 0;
        dbLveTwoMulti = 0;
    }
    m_dbLveTwoHexShort = (m_dbHexagonSide / 2) - (dbLveTwoMulti * ((m_dbHexagonSide / 2) / 10));
    m_dbLveTwoHexSide = ((sqrt(3)/3) * m_dbLveTwoHexShort)*2;
    m_dbLveThreeHexShort = (m_dbLveTwoHexShort / 2 ) - (dbLveThreeMulti * (((m_dbLveTwoHexShort / 2) / 10)));
    m_dbLveThreeHexSide = ((sqrt(3)/3) * m_dbLveThreeHexShort)*2;


}

void CIrregularGrid::SetRectSize(int nLveOne, int nLveTwo, int nLveThree)
{
    m_RectSizeLveOne = nLveOne;
    m_RectSizeLveTwo = nLveTwo;
    m_RectSizeLveThree = nLveThree;
}

UnitRect CIrregularGrid::GetRect(double x,double y, int nTagLve)  //获取区域
{
	int nCol = (x-m_left)/m_dbRectWidth;
	int nRaw = (y-m_bottom)/m_dbRectHeight;
    HexagonPoint stuCenter;
    int nTag = 0;
    if ((nRaw % 2) != 0)
    {
        nTag = 1;
    }
    else
    {
        nTag = 2;
    }

    GetHexagonPoint(x, y, nTag, &stuCenter, nCol, nRaw);
    m_X = stuCenter.dbX; // 基站坐标
    m_Y = stuCenter.dbY;
//    cout << "x = " << stuCenter.dbX << " ";
//    cout << "y = " << stuCenter.dbY << endl;
    UnitRect stuUR;
    stuUR = GetRecInHexagon(x, y, &stuCenter, nTagLve);


	return stuUR;
}


bool comparePoint(const HexagonPoint t1, const HexagonPoint t2)
{
    return t1.dbPointToCenter < t2.dbPointToCenter;
}

bool CIrregularGrid::GetHexagonPoint(double dbX, double dbY, int nTag, HexagonPoint* pHP, int nCol, int nRaw)
{
    if (nTag == 1) // 偶数排 从0开始起算
    {
        HexagonPoint stuUp;
        HexagonPoint stuDown1;
        HexagonPoint stuDown2;

        stuUp.dbX = (nCol*m_dbRectWidth) + (m_dbRectWidth / 2) + m_left;
        stuUp.dbY = (nRaw*m_dbRectHeight) + m_dbRectHeight + m_bottom;
        stuUp.dbPointToCenter = CalDisToPoint(dbX,dbY, &stuUp);

        stuDown1.dbX = nCol*m_dbRectWidth + m_left;
        stuDown1.dbY = nRaw*m_dbRectHeight + m_bottom;
        stuDown1.dbPointToCenter = CalDisToPoint(dbX,dbY, &stuDown1);

        stuDown2.dbX = (nCol*m_dbRectWidth) + m_dbRectWidth + m_left;
        stuDown2.dbY = nRaw*m_dbRectHeight + m_bottom;
        stuDown2.dbPointToCenter = CalDisToPoint(dbX,dbY, &stuDown2);

        vector<HexagonPoint> vecPoint;
        vecPoint.push_back(stuUp);
        vecPoint.push_back(stuDown1);
        vecPoint.push_back(stuDown2);
        sort(vecPoint.begin(), vecPoint.end(), comparePoint);
        pHP->dbX = vecPoint.at(0).dbX;
        pHP->dbY = vecPoint.at(0).dbY;
    }
    else // 奇数排
    {
        HexagonPoint stuDown;
        HexagonPoint stuUp1;
        HexagonPoint stuUp2;

        stuDown.dbX = (nCol*m_dbRectWidth) + (m_dbRectWidth / 2) + m_left;
        stuDown.dbY = (nRaw*m_dbRectHeight) + m_bottom;
        stuDown.dbPointToCenter = CalDisToPoint(dbX,dbY, &stuDown);

        stuUp1.dbX = nCol*m_dbRectWidth + m_left;
        stuUp1.dbY = nRaw*m_dbRectHeight + m_dbRectHeight + m_bottom;
        stuUp1.dbPointToCenter = CalDisToPoint(dbX,dbY, &stuUp1);

        stuUp2.dbX = (nCol*m_dbRectWidth) + m_dbRectWidth + m_left;
        stuUp2.dbY = nRaw*m_dbRectHeight + m_dbRectHeight + m_bottom;
        stuUp2.dbPointToCenter = CalDisToPoint(dbX,dbY, &stuUp2);

        vector<HexagonPoint> vecPoint;
        vecPoint.push_back(stuDown);
        vecPoint.push_back(stuUp1);
        vecPoint.push_back(stuUp2);
        sort(vecPoint.begin(), vecPoint.end(), comparePoint);
        pHP->dbX = vecPoint.at(0).dbX;
        pHP->dbY = vecPoint.at(0).dbY;
    }
}

double CIrregularGrid::CalDisToPoint(double dbX, double dbY, HexagonPoint* pHP)
{
    double dbCal = ((dbX - pHP->dbX) * (dbX - pHP->dbX)) + ((dbY - pHP->dbY) * (dbY - pHP->dbY));
    return sqrt(dbCal);m_dbLveOneTop;
}

UnitRect CIrregularGrid::GetRecInHexagon(double dbX, double dbY, HexagonPoint* pHP, int nTag)
{

        // 计算一个基站范围一级六边形所在矩形的边界
        m_dbLveOneleft = pHP->dbX - (m_dbRectWidth/2);
        m_dbLveOneTop = pHP->dbY + m_dbHexagonSide;
        m_dbLveOneright = m_dbLveOneleft + m_dbRectWidth;
        m_dbLveOnebottom = m_dbLveOneTop - (2*m_dbHexagonSide);

        // 计算一个基站范围一级六边形内，二级六边形内的矩形区域的边界UnitRect rt;
        m_dbLveTwoleft = pHP->dbX - m_dbLveTwoHexSide;
        m_dbLveTwoTop = pHP->dbY + m_dbLveTwoHexShort;
        m_dbLveTworight = m_dbLveTwoleft + (m_dbLveTwoHexSide*2);
        m_dbLveTwobottom = m_dbLveTwoTop - (m_dbLveTwoHexShort*2);

        // 计算二级六边形内，三级六边形内的矩形区域的边界
        m_dbLveThreeleft = pHP->dbX - (m_dbLveThreeHexShort);
        m_dbLveThreeTop = pHP->dbY + m_dbLveThreeHexSide;
        m_dbLveThreeright = m_dbLveThreeleft + (m_dbLveThreeHexShort*2);
        m_dbLveThreebottom = m_dbLveThreeTop - (m_dbLveThreeHexSide*2);
//    }

//        cout << "LveOneleft = " << m_dbLveOneleft << endl;
//        cout << "LveOneTop = " << m_dbLveOneTop << endl;
//        cout << "LveOneright = " << m_dbLveOneright << endl;
//        cout << "LveOnebottom = " << m_dbLveOnebottom << endl;
//
//        cout << "LveTwoleft = " << m_dbLveTwoleft << endl;
//        cout << "LveTwoTop = " << m_dbLveTwoTop << endl;
//        cout << "LveTworight = " << m_dbLveTworight << endl;
//        cout << "LveTwobottom  = " << m_dbLveTwobottom << endl;
//
//        cout << "LveThreeleft  = " << m_dbLveThreeleft << endl;
//        cout << "LveThreeTop  = " << m_dbLveThreeTop << endl;
//        cout << "LveThreeright  = " << m_dbLveThreeright << endl;
//        cout << "LveThreebottom  = " << m_dbLveThreebottom << endl;

    UnitRect rt;
    if (dbY >= m_dbLveTwoTop || dbY <= m_dbLveTwobottom ||
        dbX <= m_dbLveTwoleft || dbX >= m_dbLveTworight)
    {// 如果在二级六边形的外面

        rt = GetRecLveOne(dbX, dbY);
    }
    else
    {
        rt = CalRecLve_1(dbX, dbY, nTag); // 判断在一级六边形还是二级六边形
    }
    return rt;
}

UnitRect CIrregularGrid::CalRecLve_1(double dbX, double dbY, int nTag)
{
    UnitRect rt;
    if (dbX >= (m_dbLveTwoleft + ( m_dbLveTwoHexSide / 2 )) &&
        dbX <= (m_dbLveTworight - ( m_dbLveTwoHexSide / 2 )) &&
        nTag == 3)
    {
        rt = CalRecLve_2(dbX, dbY);
    }
    else if (dbX >= (m_dbLveTwoleft + ( m_dbLveTwoHexSide / 2 )) &&
             dbX <= (m_dbLveTworight - ( m_dbLveTwoHexSide / 2 )) &&
             nTag == 2)
    {
        rt = GetRecLveTwo(dbX, dbY);
    }
    int nCol = (int)((dbX - m_dbLveTwoleft) / (m_dbLveTwoHexSide / 2));
    int nRaw = (int)((dbY - m_dbLveTwobottom) / m_dbLveTwoHexShort);

    if (nCol == 0 && nRaw == 1 ) // 需要判断
    {
        if (!LveTwoRect_1(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveOne(dbX, dbY);
        }
        else
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
    }
    else if (nCol == 3 && nRaw == 1)
    {
        if (!LveTwoRect_2(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveOne(dbX, dbY);
        }
        else
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
    }
    else if (nCol == 0 && nRaw == 0)
    {
        if (!LveTwoRect_2(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveOne(dbX, dbY);
        }
        else
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
    }
    else if (nCol == 3 && nRaw == 0)
    {
        if (!LveTwoRect_1(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveOne(dbX, dbY);
        }
        else
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
    }

    return rt;
}

UnitRect CIrregularGrid::CalRecLve_2(double dbX, double dbY)
{
//        double m_dbLveThreeHexShort;
//        double m_dbLveThreeHexSide;
    UnitRect rt;
    if (dbY <= (m_Y + ( m_dbLveThreeHexSide / 2 )) &&
        dbX >= (m_Y - ( m_dbLveThreeHexSide / 2 )) )
    {
        rt = GetRecLveThree(dbX, dbY);
    }
    int nCol = (int)((dbX - m_dbLveThreeleft) / m_dbLveThreeHexShort);
    int nRaw = (int)((dbY - m_dbLveThreebottom) / m_dbLveThreeHexSide);


    if (nCol == 0 && nRaw == 0)
    {
        if (!LveThreeRect_1(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
        else
        {
            rt = GetRecLveThree(dbX, dbY);
        }
    }
    else if (nCol == 1 && nRaw == 3)
    {
        if (LveThreeRect_1(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
        else
        {
            rt = GetRecLveThree(dbX, dbY);
        }
    }
    else if (nCol == 0 && nRaw == 3)
    {
        if (LveThreeRect_2(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
        else
        {
            rt = GetRecLveThree(dbX, dbY);
        }
    }
    else if (nCol == 1 && nRaw == 0)
    {
        if (!LveThreeRect_2(dbX, dbY, nCol, nRaw))
        {
            rt = GetRecLveTwo(dbX, dbY);
        }
        else
        {
            rt = GetRecLveThree(dbX, dbY);
        }
    }
    else
    {
        rt = GetRecLveThree(dbX, dbY);
    }

    return rt;
}

bool CIrregularGrid::LveTwoRect_1(double dbX, double dbY, int nCol, int nRaw)
{
    double dbCornerX = (nCol * (m_dbLveTwoHexSide/2)) + m_dbLveTwoleft;
    double dbCornerY = (nRaw * (m_dbLveTwoHexShort)) + m_dbLveTwobottom;
    if ((dbY - dbCornerY) / (dbX - dbCornerX) <= ((m_dbLveTwoHexShort) / (m_dbLveTwoHexSide/2)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CIrregularGrid::LveTwoRect_2(double dbX, double dbY, int nCol, int nRaw)
{
    //            double m_dbLveTwoHexShort;
//        double m_dbLveTwoHexSide;
//
//        double m_dbLveThreeHexShort;
//        double m_dbLveThreeHexSide;
    double dbCornerX = (nCol * (m_dbLveTwoHexSide/2)) + (m_dbLveTwoHexSide/2) + m_dbLveTwoleft;
    double dbCornerY = (nRaw * (m_dbLveTwoHexShort)) + m_dbLveTwobottom;
    if (abs((dbY - dbCornerY) / (dbX - dbCornerX)) <= ((m_dbLveTwoHexShort) / (m_dbLveTwoHexSide/2)))
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool CIrregularGrid::LveThreeRect_1(double dbX, double dbY, int nCol, int nRaw)
{
    double dbCornerX = (nCol * (m_dbBSSpace/6)) + (m_dbBSSpace/6) + m_dbLveThreeleft;
    double dbCornerY = (nRaw * (m_dbRectHeight/12)) + m_dbLveThreebottom;
    if (abs((dbY - dbCornerY) / (dbX - dbCornerX)) <= ((m_dbRectHeight/12) / (m_dbBSSpace/6)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool CIrregularGrid::LveThreeRect_2(double dbX, double dbY, int nCol, int nRaw)
{
    double dbCornerX = (nCol * (m_dbBSSpace/6)) + m_dbLveThreeleft;
    double dbCornerY = (nRaw * (m_dbRectHeight/2)) + m_dbLveThreebottom;
    if (abs((dbY - dbCornerY) / (dbX - dbCornerX)) <= ((m_dbRectHeight/12) / (m_dbBSSpace/6)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

UnitRect CIrregularGrid::GetRecLveOne(double dbX, double dbY)
{
    int nCol = (int)((dbX-m_dbLveOneleft) / m_RectSizeLveOne);
    int nRaw = (int)((dbY-m_dbLveOnebottom) / m_RectSizeLveOne);

    UnitRect rt;
    rt.raw = nRaw;
	rt.col = nCol;
	rt.left = nCol*m_RectSizeLveOne + m_dbLveOneleft;
	rt.right = rt.left + m_RectSizeLveOne;
	rt.bottom = nRaw*m_RectSizeLveOne + m_dbLveOnebottom;
	rt.top = rt.bottom + m_RectSizeLveOne;
	rt.centerX = ((rt.right - rt.left)/2) + rt.left;
	rt.centerY = ((rt.top - rt.bottom)/2) + rt.bottom;
	rt.realX = dbX;
	rt.realY = dbY;
	return rt;
}

UnitRect CIrregularGrid::GetRecLveTwo(double dbX, double dbY)
{
    int nCol = (int)((dbX-m_dbLveTwoleft) / m_RectSizeLveTwo);
    int nRaw = (int)((dbY-m_dbLveTwobottom) / m_RectSizeLveTwo);

    UnitRect rt;
    rt.raw = nRaw;
	rt.col = nCol;
	rt.left = nCol*m_RectSizeLveTwo + m_dbLveTwoleft;
	rt.right = rt.left + m_RectSizeLveTwo;
	rt.bottom = nRaw*m_RectSizeLveTwo + m_dbLveTwobottom;
	rt.top = rt.bottom + m_RectSizeLveTwo;
	rt.centerX = ((rt.right - rt.left)/2) + rt.left;
	rt.centerY = ((rt.top - rt.bottom)/2) + rt.bottom;
	rt.realX = dbX;
	rt.realY = dbY;
	return rt;
}

UnitRect CIrregularGrid::GetRecLveThree(double dbX, double dbY)
{
    int nCol = (int)((dbX-m_dbLveThreeleft) / m_RectSizeLveThree);
    int nRaw = (int)((dbY-m_dbLveThreebottom) / m_RectSizeLveThree);

    UnitRect rt;
    rt.raw = nRaw;
	rt.col = nCol;
	rt.left = nCol*m_RectSizeLveThree + m_dbLveThreeleft;
	rt.right = rt.left + m_RectSizeLveThree;
	rt.bottom = nRaw*m_RectSizeLveThree + m_dbLveThreebottom;
	rt.top = rt.bottom + m_RectSizeLveThree;
	rt.centerX = ((rt.right - rt.left)/2) + rt.left;
	rt.centerY = ((rt.top - rt.bottom)/2) + rt.bottom;
	rt.realX = dbX;
	rt.realY = dbY;
	return rt;
}

bool CIrregularGrid::CalTotalNumOfGrid()
{



}

//
//
//bool CIrregularGrid::CalLveOne()
//{
//    int nNumOneRow = (int)(m_dbLveOneTop - m_dbLveOnebottom) / m_RectSizeLveOne;
//    int nNumOneCol = (int)(m_dbLveOneright - m_dbLveOneleft) / m_RectSizeLveOne;
//    vector<UnitRect> vecURLveOne;
//    for (int i = 0; i != nNumOneRow; i++)
//    {
//        for (int j = 0; j != nNumOneCol; j++)
//        {
//            UnitRect ur;
//            double dbX = (j*nNumOneCol) + (m_RectSizeLveOne / 2);
//            double dbY = (i*nNumOneRow) + (m_RectSizeLveOne / 2);
//            if (dbY > m_dbLveOneTop || dbY < m_dbLveOnebottom ||
//                dbX > m_dbLveOneright || dbX < m_dbLveOneleft)
//            {
//                ur.centerX = dbX;
//                ur.centerY = dbY;
//                vecURLveOne.push_back(ur);
//            }
//            else
//            {
//                int nCol = (int)((dbX - m_dbLveTwoleft) / (m_dbLveTwoHexSide / 2));
//                int nRaw = (int)((dbY - m_dbLveTwobottom) / m_dbLveTwoHexShort);
//                if (!LveTwoRect_1(dbX, dbY, nCol, nRaw))
//                {
//                    ur.centerX = dbX;
//                    ur.centerY = dbY;
//                    vecURLveOne.push_back(ur);
//                }
//            }
//        }
//    }
//}
//bool CIrregularGrid::CalLveTwo()
//{
//    int nNumTwoRow = (int)(m_dbLveTwoTop - m_dbLveTwobottom) / m_RectSizeLveTwo;
//    int nNumTwoCol = (int)(m_dbLveTworight - m_dbLveTwoleft) / m_RectSizeLveTwo;
//    vector<UnitRect> vecURLveTwo;
//    for (int i = 0; i != nNumTwoRow; i++)
//    {
//        for (int j = 0; j != nNumTwoCol; j++)
//        {
//            UnitRect ur;
//            double dbX = (j*nNumTwoCol) + (m_RectSizeLveTwo / 2);
//            double dbY = (i*nNumTwoRow) + (m_RectSizeLveTwo / 2);
//            if (dbY > m_dbLveThreeTop || dbY < m_dbLveThreebottom ||
//                dbX > m_dbLveThreeright || dbX < m_dbLveThreeleft)
//            {
//                ur.centerX = dbX;
//                ur.centerY = dbY;
//                vecURLveOne.push_back(ur);
//            }
//            else
//            {
//                int nCol = (int)((dbX - m_dbLveThreeleft) / m_dbLveThreeHexShort);
//                int nRaw = (int)((dbY - m_dbLveThreebottom) / m_dbLveThreeHexSide);
//                if (!LveThreeRect_1(dbX, dbY, nCol, nRaw))
//                {
//                    ur.centerX = dbX;
//                    ur.centerY = dbY;
//                    vecURLveTwo.push_back(ur);
//                }
//            }
//        }
//    }
//}
//
//bool CIrregularGrid::CalLveThree()
//{
//    /**
//        double m_dbLveThreeTop;
//        double m_dbLveThreeleft;
//        double m_dbLveThreeright;
//        double m_dbLveThreebottom;
//
//        double m_RectSizeLveOne;
//        double m_RectSizeLveTwo;
//        double m_RectSizeLveThree;
//**/
//    int nNumThreeRow = (int)(m_dbLveThreeTop - m_dbLveThreebottom) / m_RectSizeLveThree;
//    int nNumThreeCol = (int)(m_dbLveThreeright - m_dbLveThreeleft) / m_RectSizeLveThree;
//    vector<UnitRect> vecURLveThree;
//    for (int i = 0; i != nNumThreeRow; i++)
//    {
//        for (int j = 0; j != nNumThreeCol; j++)
//        {
//            UnitRect ur;
//            double dbX = (j*nNumThreeCol) + (m_RectSizeLveThree / 2);
//            double dbY = (i*nNumThreeRow) + (m_RectSizeLveThree / 2);
//            if (dbY > m_dbLveThreeTop || dbY < m_dbLveThreebottom ||
//                dbX > m_dbLveThreeright || dbX < m_dbLveThreleft)
//            {
//                ur.centerX = dbX;
//                ur.centerY = dbY;
//                vecURLveOne.push_back(ur);
//            }
//            else
//            {
//                int nCol = (int)((dbX - m_dbLveThreeleft) / m_dbLveThreeHexShort);
//                int nRaw = (int)((dbY - m_dbLveThreebottom) / m_dbLveThreeHexSide);
//                if (!LveTwoRect_2(dbX, dbY, nCol, nRaw))
//                {
//                    ur.centerX = dbX;
//                    ur.centerY = dbY;
//                    vecURLveTwo.push_back(ur);
//                }
//            }
//
//    }
//    }
//}
