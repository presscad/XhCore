#if defined(APP_EMBEDDED_MODULE_ENCRYPT)||defined(_EMBEDDED_MOD_CORE_)
#define MODCORE_API	//��ǶʱMODCORE_APIʲô��������
#else
#ifdef MODCORE_EXPORTS
#define MODCORE_API __declspec(dllexport)
#else
#define MODCORE_API __declspec(dllimport)
#endif
#endif

#pragma once
#include <math.h>
#include "stdio.h"

//��ά����
struct MODCORE_API MOD_POINT{
	double x, y, z;
	MOD_POINT() { x = y = z = 0; }
	MOD_POINT(double X,double Y,double Z) {x=X;y=Y;z=Z;}
	MOD_POINT(const double* coord3d) {
		if(coord3d==NULL)
			x = y = z = 0; 
		else
		{
			x=coord3d[0];
			y=coord3d[1];
			z=coord3d[2];
		}
	}
	MOD_POINT Set(double X=0, double Y=0, double Z=0){
		x = X;y = Y;z = Z;
		return *this;
	}
	bool IsEqual(double ex,double ey,double ez,double epsilon=1e-9) const;
	bool IsZero() const{return fabs(x)+fabs(y)+fabs(z)<1e-9;}
	operator double*(){return &x;}
	const double* operator =(double* coord){
		x=coord[0];y=coord[1];z=coord[2];
		return &x;
	}
};
struct MODCORE_API MOD_LINE{
public:
	MOD_POINT startPt,endPt;
public:
	MOD_LINE(){};
	MOD_LINE(double* pt_S,double* pt_E){startPt=MOD_POINT(pt_S);endPt=MOD_POINT(pt_E);}
};
//��ά����ϵ
struct MODCORE_API MOD_CS{
	MOD_POINT origin;
	//����ϵ��X/Y/Z������, ����������ѭ������˨��������Ϊ��λ��ʸ��
	MOD_POINT axisX, axisY, axisZ;
	MOD_CS(){axisX.x = axisY.y = axisZ.z = 1.0;}
	MOD_CS Clone()
	{
		MOD_CS cs;
		cs.FromCoordsSet(&origin.x);
		cs.origin.Set(origin.x, origin.y, origin.z);
		cs.axisX.Set(axisX.x, axisX.y, axisX.z);
		cs.axisY.Set(axisY.x, axisY.y, axisY.z);
		cs.axisZ.Set(axisZ.x, axisZ.y, axisZ.z);
		return cs;
	}
	void FromCoordsSet(const double* coords_12)
	{
		origin.Set(coords_12[0],coords_12[1],coords_12[2]);
		axisX.Set(coords_12[3],coords_12[ 4],coords_12[ 5]);
		axisY.Set(coords_12[6],coords_12[ 7],coords_12[ 8]);
		axisZ.Set(coords_12[9],coords_12[10],coords_12[11]);
	}
};
struct MODCORE_API MOD_HANG_NODE
{
	char m_ciWireType;		//������:'C'����|'E'����|'J'����
	char m_ciHangingStyle;	//�Ҵ����ͣ�0.����|'S'˫��|'V'V��
	BYTE m_ciLoopSerial;	//��·���
	BYTE m_ciPhaseSerial;	//�����
	BYTE m_ciHangOrder;		//�����
	BYTE m_ciTensionType;	//��������: 1.����; 2.����
	BYTE m_ciWireDirect;	//��������:'X'|'Y'
	char m_sHangName[50];	//�ҵ�����
	MOD_POINT m_xHangPos;	//λ��
	//
	MOD_HANG_NODE(){
		m_ciLoopSerial=0;
		m_ciPhaseSerial=0;
		m_ciHangOrder=0;
		m_ciWireDirect=0;
		m_ciWireType=0;
		m_ciHangingStyle=0;
		m_ciTensionType = 1;
		strcpy(m_sHangName,"");
	}
	BYTE GetHangPosSymbol()
	{
		if (m_ciWireType == 'J' || m_ciTensionType != 2)
			return 0;	//���߼����������߲�����ǰ��
		if (m_ciWireDirect == 'Y')
		{	//����GIM�淶Ҫ��:X��ᵣ��Y������Ϊǰ,����Ϊ��
			if (m_xHangPos.y < -0.03)
				return 'H';
			else
				return 'Q';
		}
		else //if(code.ciWireDirection=='X')
		{	//����GIM�淶Ҫ��:Y��ᵣ��X�Ḻ��Ϊǰ,����Ϊ��
			if (m_xHangPos.x < -0.03)
				return 'Q';
			else
				return 'H';
		}
	}
	void Clone(MOD_HANG_NODE* pSrcHangNode)
	{
		m_ciWireType=pSrcHangNode->m_ciWireType;
		m_ciHangingStyle=pSrcHangNode->m_ciHangingStyle;
		m_ciLoopSerial=pSrcHangNode->m_ciLoopSerial;
		m_ciPhaseSerial=pSrcHangNode->m_ciPhaseSerial;
		m_ciHangOrder=pSrcHangNode->m_ciHangOrder;
		m_ciWireDirect=pSrcHangNode->m_ciWireDirect;
		m_xHangPos=pSrcHangNode->m_xHangPos;
		strcpy(m_sHangName,pSrcHangNode->m_sHangName);
	}
};

class IModModel;
class IModHeightGroup;
class MODCORE_API IModNode
{
public:
	virtual IModModel* BelongModel()=0;
	virtual void SetBelongModel(IModModel* pModel)=0;
	virtual long GetId()=0;
	virtual void SetCfgword(BYTE* cfgword_bytes24)=0;
	virtual void SetOrg(MOD_POINT pt)=0;
	virtual void SetLdsOrg(MOD_POINT pt)=0;
	virtual void SetLayer(char cLayer)=0;
	virtual BOOL IsLegNode()=0;
	virtual MOD_POINT NodePos()=0;
	virtual void Clone(IModNode* pSrcNode)=0;
};
class CSolidBody;
class MODCORE_API IModRod
{
public:
	virtual IModModel* BelongModel()=0;
	virtual void SetBelongModel(IModModel* pModel)=0;
	virtual BOOL IsLegRod()=0;
	virtual BOOL IsAngle()=0;
	//
	virtual void SetLayer(char cLayer)=0;
	virtual void SetRodType(BYTE type)=0;
	virtual void SetMaterial(char cMat)=0;
	virtual void SetCfgword(BYTE* cfgword_bytes24)=0;
	virtual void SetWidth(double fWidth)=0;
	virtual void SetThick(double fThick)=0;
	virtual void SetWingXVec(MOD_POINT vWingX)=0;
	virtual void SetWingYVec(MOD_POINT vWingY)=0;
	virtual void SetNodeS(IModNode* pNodeS)=0;
	virtual void SetNodeE(IModNode* pNodeE)=0;
	virtual void Clone(IModRod* pSrcRod)=0;
	//
	virtual long GetId()=0;
	virtual BYTE GetRodType()=0;
	virtual BOOL Create3dSolidModel(CSolidBody *pSolidBody,BOOL bToLds=TRUE)=0;
	virtual MOD_POINT LinePtS()=0;
	virtual MOD_POINT LinePtE()=0;
	virtual MOD_LINE GetBaseLineToLdsModel()=0;
	virtual double GetWidth()=0;
	virtual double GetThick()=0;
};
class MODCORE_API IModTowerInstance
{
public:
	virtual IModHeightGroup* BelongHuGao()=0;
	virtual MOD_POINT GetBaseLocation(int iLegQuad)=0;
	//�ڵ�
	virtual int GetModNodeNum()=0;
	virtual IModNode* EnumModNodeFir()=0;
	virtual IModNode* EnumModNodeNext()=0;
	//�˼�
	virtual int GetModRodNum()=0;
	virtual IModRod* EnumModRodFir()=0;
	virtual IModRod* EnumModRodNext()=0;
	//
	virtual bool ExportModFile(const char* sFileName)=0;
};
class MODCORE_API IModHeightGroup
{
public:
	virtual void SetBelongModel(IModModel* pModel)=0;
	virtual IModModel* BelongModel()=0;
	virtual int GetNo()=0;
	virtual void SetLowestZ(double fLowestZ)=0;
	virtual void SetLegCfg(BYTE* cfgword_bytes24)=0;
	virtual void GetName(char* sName)=0;
	virtual double GetNamedHeight()=0;
	virtual void SetNameHeight(double fNameHeight)=0;
	virtual double GetLegDiffDist(int iLegSerial)=0;
	virtual int* GetLegSerialId(const char* sLegDiffDist)=0;
	virtual int* EnumFirstLegSerial()=0;
	virtual int* EnumNextLegSerial()=0;
	virtual IModTowerInstance* GetTowerInstance(int legSerialQuad1, int legSerialQuad2, int legSerialQuad3, int legSerialQuad4)=0;
};
class MODCORE_API IModModel
{
public:
	virtual long GetSerialId()=0;
	virtual void SetTowerHeight(double fTowerHeight)=0;
	virtual MOD_CS BuildUcsByModCS()=0;
	//�ڵ�
	virtual IModNode* AppendNode(DWORD key)=0;
	virtual IModNode* FindNode(DWORD key)=0;
	virtual IModNode* EnumFirstNode()=0;
	virtual IModNode* EnumNextNode()=0;
	//�˼�
	virtual IModRod* AppendRod(DWORD key)=0;
	virtual IModRod* FindRod(DWORD key)=0;
	virtual IModRod* EnumFirstRod()=0;
	virtual IModRod* EnumNextRod()=0;
	//����
	virtual IModHeightGroup* AppendHeightGroup(DWORD key)=0;
	virtual IModHeightGroup* EnumFirstHGroup()=0;
	virtual IModHeightGroup* EnumNextHGroup()=0;
	virtual IModHeightGroup* GetHeightGroup(int iNo)=0;
	virtual IModHeightGroup* GetHeightGroup(const char* sName)=0;
	//�ҵ�
	virtual MOD_HANG_NODE* AppendHangNode()=0;
	virtual int GetHangNodeNum()=0;
	virtual MOD_HANG_NODE* GetHangNodeById(int index)=0;
	//��д
	virtual BOOL ImportModFile(const char* sModFile)=0;
	virtual void ReadModFile(FILE* fp,BOOL bUtf8=FALSE)=0;
	virtual void WriteModFile(FILE* fp,BOOL bUtf8=FALSE)=0;
	virtual void WriteModFileByUtf8(FILE* fp)=0;
	virtual void WriteModFileByAnsi(FILE* fp)=0;
	virtual void InitSingleModData(double fNameH)=0;
	virtual void InitMultiModData()=0;
};
class MODCORE_API CModModelFactory
{
public:
	static IModModel* CreateModModel();
	static IModModel* ModModelFromSerial(long serial);
	static bool Destroy(long serial);
};
//
MODCORE_API BOOL IsUTF8File(const char* mod_file);
MODCORE_API void ANSIToUnicode(const char* src_str,wchar_t* des_str);
MODCORE_API void UnicodeToANSI(const wchar_t* src_str,char* des_str);
MODCORE_API void UTF8ToUnicode(const char* src_str,wchar_t* des_str);
MODCORE_API void UnicodeToUTF8(const wchar_t* src_str,char* des_str);
MODCORE_API void ANSIToUTF8(const char* src_str,char* des_str);
MODCORE_API void UTF8ToANSI(const char* src_str,char* des_str);
MODCORE_API BYTE SetModCfgwordSchema(BYTE cbMultiLegSchema);
