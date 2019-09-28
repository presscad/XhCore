#pragma once
#include "PartLib.h"
#include "XhCharString.h"
//螺栓规格库
class CBoltLibrary :public IBoltLibrary{
	long serialId;
	CHashList<CLsFamily> boltLib;
public:
	CBoltLibrary();
	//螺栓规格库操作函数
	void SetSerialId(long id){serialId=id;}
	long GetSerialId(){return serialId;}
	CLsFamily* FromHandle(long h);
	CLsFamily* GetFirstFamily();
	CLsFamily* GetNextFamily();
	void ClearLibrary();
	int GetFamilyNum();
	CLsFamily* AppendFamily();
	CLsFamily* AppendFamily(long hFamily);
	BOOL RemoveFamily(long h);
	//螺栓规格查询函数
	LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0);
	LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,long hFamily=0);
	//复制螺栓库
	void Copy(IBoltLibrary *pSrcBoltLib);
	void CopyLsFamily(CLsFamily* pNewFamily,CLsFamily* pSrcFamily);
};
