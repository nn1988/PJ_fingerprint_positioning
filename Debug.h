#pragma once

#include "dataDef.h"
#include "Cov.h"
#include "FpCreate.h"
#include "DBoperate.h"
#include "SimCal.h"
#include "IrregularGrid.h"

#include <stdio.h>
#include <fstream>
#include <iostream>


class CDebug
{
public:
	CDebug(void);
	~CDebug(void);

public:
	void TestCov();
	void TestDBInsert();
	void TestDBSelect();
	void TestSimCal();
	void TestIrregula();

};

