#pragma once
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 MY3DS_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// MY3DS_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef MY3DS_EXPORTS
#define MY3DS_API __declspec(dllexport)
#else
#define MY3DS_API __declspec(dllimport)
#endif

// 此类是从 3DS.dll 导出的
#include "SolidBody.h"

struct MY3DS_API I3DSData
{
	virtual int GetSerial()=0;
	virtual bool AddSolidPart(CSolidBody* pSolidBody,int nId,char* sSolidName,BOOL iInch0M1MM2 =FALSE,int nParentId=-1)=0;
	virtual void Creat3DSFile(const char* sFilePath)=0;
};

class MY3DS_API C3DSFactory{
public:
	static I3DSData* Create3DSInstance();
	static I3DSData* Get3DSFromSerial(long serial);
	static BOOL Destroy(long h);
};