#pragma once
#include "PartLib.h"
#include "ArrayList.h"

struct CAngleLibrary: public IAngleLibrary{
public:
	CAngleLibrary();
	~CAngleLibrary();
	virtual int AngleCount();
	virtual int SetAngleCount(int count);
	virtual JG_PARA_TYPE* GetAt(int index);
	virtual JG_PARA_TYPE* SetAt(int index,const JG_PARA_TYPE& anglesize);
	virtual JG_PARA_TYPE* AppendAngleSize(const JG_PARA_TYPE& anglesize);
	virtual JG_PARA_TYPE* GetAngleSizeTbl();
};
extern CAngleLibrary globalAngleLibrary;

