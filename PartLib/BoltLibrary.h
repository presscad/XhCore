#pragma once
#include "PartLib.h"
#include "XhCharString.h"
//��˨����
class CBoltLibrary :public IBoltLibrary{
	long serialId;
	CHashList<CLsFamily> boltLib;
public:
	CBoltLibrary();
	//��˨�����������
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
	//��˨����ѯ����
	LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0);
	LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,long hFamily=0);
	//������˨��
	void Copy(IBoltLibrary *pSrcBoltLib);
	void CopyLsFamily(CLsFamily* pNewFamily,CLsFamily* pSrcFamily);
};
