#ifndef __FP_CREATE_H__
#define __FP_CREATE_H__
#include "SigCreate.h"
#include   <stdlib.h>

class CFpCreate
{
public:

	CFpCreate(PhoneList* phoneList);
	~CFpCreate(void);

	bool GetSig(int PhoneID,Signatrue* pSig);
protected:
	PhoneInfo* GetPhoneFromID(int PhoneID);
protected:

	PhoneList* m_pPhoneList;

};

#endif

