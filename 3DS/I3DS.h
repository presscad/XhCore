#pragma once
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� MY3DS_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// MY3DS_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef MY3DS_EXPORTS
#define MY3DS_API __declspec(dllexport)
#else
#define MY3DS_API __declspec(dllimport)
#endif

// �����Ǵ� 3DS.dll ������
#include "SolidBody.h"

struct MY3DS_API I3DSData
{
	virtual int GetSerial()=0;
	virtual bool AddSolidPart(CSolidBody* pSolidBody,int nId,char* sSolidName,BOOL bTransPtMMtoM=FALSE,int nParentId=-1)=0;
	virtual void Creat3DSFile(const char* sFilePath)=0;
};

class MY3DS_API C3DSFactory{
public:
	static I3DSData* Create3DSInstance();
	static I3DSData* Get3DSFromSerial(long serial);
	static BOOL Destroy(long h);
};