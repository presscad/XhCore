#ifndef __PARTLIB_H_
#define __PARTLIB_H_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PARTLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PARTLIB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#if defined(_PARTLIB_EMBEDDED_)||defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#define PARTLIB_API
#else
#ifdef PARTLIB_EXPORTS
#define PARTLIB_API __declspec(dllexport)
#else
#define PARTLIB_API __declspec(dllimport)
#endif
#endif
#include "f_ent.h"
// This class is exported from the PartLib.dll
//����������������ࣨ��ṹ������������������������
//�Ǹֹ�� ׼��g ׼��g1 ׼��g2 ׼��g3
typedef struct PARTLIB_API{	// �Ǹֵ�:
	//double  r;//��Բ���뾶        Wjh--00.04.20
	long  g;    //������˨ʱ��׼��  Wjh--00.04.20
	long  g1;   // 2����˨ʱ, ʹ��g1��g2׼��  Wjh--00.04.20
	long  g2;
	long  g3;	// g,g1,g2�϶��õ�,rû�õ�.g3��������˨��,����ֻ����2��,����û�õ�.
	BYTE MatchStyleG(double gDist,bool bPreferMultiBoltG=true);
	long ByStyleG(int indexOfStyleG,double gDefault=0);
} JGZJ;//�Ǹ�׼��   Wjh--00.04.19
typedef struct PARTLIB_API STEELMAT{
	char cBriefMark;
	char cPreferQualityLevel;	//Ĭ�������ȼ�
	char mark[8];
	int t_lim[3];			//��Ȼ�ֱ��mm
	double E;				//����ģ��(MPa)
	double price_coef;		//�۸�ϵ��
	double fw3;				//�Ǻ��쿹��ѹ�Ϳ���ǿ�����ֵM(Pa)
	double intensity[16];	//����ǿ��ֵ
	double fy(double t);	//����ǿ��(tΪ��Ȼ�ֱ��mm)
	double f(double t);		//������ѹ����ǿ�����ֵ(tΪ��Ȼ�ֱ��mm)
	double fv(double t);	//����ǿ�����ֵ(tΪ��Ȼ�ֱ��mm)
	double fc(double t);	//�ױڳ�ѹǿ�����ֵ(tΪ��Ȼ�ֱ��mm)
	double fw1(double t){return f(t);}	//�Ժ����쿹��ѹǿ�����ֵM(Pa)
	double fvw(double t){return fv(t);}	//�Ժ����쿹��ǿ�����ֵM(Pa)
}*STEELMATPTR;
//���Ͽ��ඨ��
class PARTLIB_API CSteelMatLibrary{
public:
	CSteelMatLibrary();
	static int GetMaxCount();
	static int GetCount();
	static int SetCount(int count);
	static STEELMAT RecordAt(int index);
	static bool SetRecordAt(int i,STEELMAT mat);
	static char Q235BriefMark();
	static char Q345BriefMark();
	static char Q390BriefMark();
	static char Q420BriefMark();
	static char Q460BriefMark();
	static STEELMAT* Q235Material();
	static STEELMAT* Q345Material();
	static STEELMAT* Q390Material();
	static STEELMAT* Q420Material();
	static STEELMAT* Q460Material();
	static int GetMatIndex(char cMat);
};
//֫��b;֫��d;��Բ�뾶r;��������
typedef struct{
	double wing_wide;		//֫��
	double wing_thick;		//֫��
	double r;				//��Բ���뾶
	double area;			//�������(cm2)
	double theroy_weight;	//��������
	double Ix;		//ƽ������Ծ�(cm4)
	double Rx;		//ƽ�����ת�뾶(cm)
	double Wx;		//ƽ���Ό�����ģ��(cm3)
	double Iy0;		//��С����Ծ�(cm4)
	double Ry0;		//��С���ת�뾶(cm)
	double Wy0;		//��С�Ό�����ģ��(cm3)
	char   cType;	//�˼�����L��T��D��X��C��R��B��S
}JG_PARA_TYPE;
typedef struct ANGLE_G_REC{
	WORD wide;
	WORD g,g1,g2,g3;
}*ANGLE_G_RECPTR;
//֫��b;֫��d;��Բ�뾶r;��������
//��ת��˼�����(��Ҫָ�ɿ���ĸֹܵȻ�ת����)
typedef struct PARTLIB_API {
	double D;		//ֱ��
	double thick;	//�ں�
	double area;			//�������(cm2)
	double theroy_weight;	//��������
	double I;		//���Ծ�(cm4)
	double r;		//��ת�뾶(cm)
	double W;		//�������ģ��(cm3)
}TUBE_PARA_TYPE;
//������˼�����(��Ҫָ������ֻ���������Ĺ���)
typedef struct PARTLIB_API {
	double wide;	//���
	double thick;	//�ں�
	double theroy_weight;	//��������
}BIAN_PARA_TYPE;
//�۸���˼�����
typedef struct{
	char   guige[16];//�۸ֹ��
	double h;		//��
	double b;		//��
	double d;		//��
	double t;		//֫��
	double r;		//��Բ���뾶
	double area;			//�������(cm2)
	double theroy_weight;	//��������
	double Ix;		//ƽ������Ծ�(cm4)
	double Rx;		//ƽ�����ת�뾶(cm)
	double Wx;		//ƽ���Ό�����ģ��(cm3)
	double Iy;		//��С����Ծ�(cm4)
	double Ry;		//��С���ת�뾶(cm)
	double Wy;		//��С�Ό�����ģ��(cm3)
	double Z0;		//
}SLOT_PARA_TYPE;
//H�͸ָ˼�����
typedef struct{
	double h;		//��
	double b;		//��
	double t1;		//������
	double t2;		//��Ե���
	double r;		//��Բ���뾶
	double area;	//�������
	double theroy_weight;	//��������
	double Ix;		//ƽ������Ծ�(cm4)
	double Iy;		//��С����Ծ�(cm4)
	double Rx;		//ƽ�����ת�뾶(cm)
	double Ry;		//��С���ת�뾶(cm)
	double Wx;		//ƽ���Ό�����ģ��(cm3)
	double Wy;		//��С�Ό�����ģ��(cm3)
}HSHAPE_PARA_TYPE;
typedef struct{
	char grade[5];	//��˨����4.8;5.8;6.8;8.8;10.9
	double f;		//����ǿ�����ֵ
	double fv;		//����ǿ�����ֵ
	double fc;		//�ױڳ�ѹǿ�����ֵ
}LS_GRADE;
//��˨���
struct LS_SUB_PART{
	WORD	wiLsD;		//��˨ֱ��
	double	thick;		//������
	double	weight;		//�������
	double  dfShapeDs;	//������β���D(Ĭ��Ϊ��˨�Ա߿��)
	double _dfShapeDe;	//������β���D2(������˨Ĭ��Ϊ��˨�ԽǶ�����)
	double set_dfShapeDe(double dfDe);//������β���D2(������˨Ĭ��Ϊ��˨�ԽǶ�����)
	double get_dfShapeDe();//������β���D2(������˨Ĭ��Ϊ��˨�ԽǶ�����)
	__declspec(property(put=set_dfShapeDe,get=get_dfShapeDe)) double dfShapeDe;

};
class PARTLIB_API CLsSubPartFamily{
	int		m_nSize;				//��ϵ�к��е���˨���������
	LS_SUB_PART* m_pLsSubPartSet;	//��˨���������
public:
	static const WORD TYPE_COMMON_NUT	= 1;	//��ͨ��ĸ
	static const WORD TYPE_FASTEN_NUT	= 2;	//�۽���ĸ
	static const WORD TYPE_ANTITHEFT_NUT= 3;	//������ĸ
	static const WORD TYPE_THIN_NUT		= 4;	//����ĸ
	static const WORD TYPE_COMMON_GASKET= 5;	//ƽ��Ȧ
	static const WORD TYPE_SPRING_GASKET= 6;	//���ɵ�Ȧ
	static const WORD TYPE_LOCK_CLASP	= 7;	//���ɿ�
	//
	friend class CLsLibrary;
	WORD	m_widType;	//���ϵ������
	char	name[16];	//���ϵ������
public:
	CLsSubPartFamily();
	~CLsSubPartFamily();
	bool CreateLsSubPartFamily(int num);
	long GetCount(){return m_nSize;}
	void CleanSubParts();
	LS_SUB_PART *RecordAt(int i);
	LS_SUB_PART *FindLsSubPart(int d);
	LS_SUB_PART *GetLsSubPartSet(){return m_pLsSubPartSet;}
};
//���� ֱ�� ��Ч�� �޿۳� ͨ������ ͨ������ ��ñֱ�� ��ñ��
struct PARTLIB_API LS_XING_HAO{
	double grade;		//��˨����
	int	   d;			//��˨ֱ��
	char   guige[16];	//��˨���
	int	   valid_len;	//��Ч��
	int    no_thread_len;//�޿۳�
	int	   down_lim;	//��˨ͨ������
	int	   up_lim;		//��˨ͨ������
	int    washer_thick;//��Ȧ���
	double weight;//,pad_weight,twocap_weight;		//��˨��������(�������ɵ�)�������ɵ棬��˫ñ
	LS_XING_HAO(){memset(this,0,sizeof(LS_XING_HAO));}
};
struct PARTLIB_API ANCHOR_BOLT{
	WORD d;	//����ֱ��
	WORD wiWidth,wiThick;	//���������
	WORD wiHoleIncrement;	//���׾���Եؽ���˨ֱ��������ֵ
	WORD wiBasePlateHoleD;	//�׽Ű��϶�Ӧ�׾�
	float m_fNutH;		//��ĸ�߶�
	float m_fNutTaper;	//��ĸ�������Բ׶����¶�h/w;��=15��~30��
	float m_fNutDe;	//��ĸ���㵽�������
	float m_fNutDs;	//��ĸ�ߵ��߾���
	float m_fLe;	//��¶��(mm)
	float m_fLa;	//�޿۳�(mm) <��������
	//float m_fLo;	//˿�۳�(mm)=��¶��-�޿۳�
	static int GetCount();
	static int SetCount(int count);
	static const ANCHOR_BOLT* RecordAt(int i);
	static bool SetAnchorBolt(int i,ANCHOR_BOLT& anchorbolt);
	static const ANCHOR_BOLT* FromBoltD(WORD d);//���ݵؽ���˨����ֱ����ѯ
};
//������˨ֱ�������˨�䡢�߾��ṹ
typedef struct PARTLIB_API {
	long M_Diameter;		//��˨ֱ��
	double Aperture;		//�����׾�
	long SingleRowSpace;	//���ż��
	long doubleRowSpace;	//˫�ż��
	long EndSpace;			//�˾�
	long PlateEdgeSpace;	//����߾�
	long RollingEdgeSpace;	//���Ʊ߾�
	long CutAngleSpace;		//�нǱ߾�
	long InnerFLSpace;		//��˨�������ڲ�ֹܱ���С�߾�
	long OutterFLSpace;		//��˨��������߱���С�߾�
} LSSPACE_STRU;
//������˨ֱ�������˨�䡢�߾��ṹ
struct PARTLIB_API LSSPACEEX : public LSSPACE_STRU{
	long WasherEdgeSpace;	//���߾�
	LSSPACEEX();
	LSSPACEEX(LSSPACE_STRU& space);
	operator LSSPACE_STRU();
	LSSPACEEX operator =(const LSSPACE_STRU& LsSpace);
};
//��˨ϵ���ඨ��
class PARTLIB_API CLsFamily{
	long m_hFamily;			//��˨ϵ�б�ʶ��
	int m_nFamilySize;		//��ϵ�к��е���˨���������
	LS_XING_HAO *m_pFamily;	//��˨�������Ĵ洢��
public:
	friend class CLsLibrary;
	friend class IBoltLibrary;
	char name[17];	//��˨ϵ������
	char descr[51];	//��˨ϵ�еı�ע����
	bool m_bDualCap;//��˨ñ����,֮ǰ��Ϊint m_nLsCap,���ַ���ʵ��Ҳ��˫ñ,ԭ����˫ñ��ָ˫��ͨñ,��ֱ�Ӳ���bool���͸����� wjh-2017.11.24
	bool m_bBurglarproof;	//�Ƿ�Ϊ������˨
	bool m_bFootNail;		//�Ƿ�Ϊ�Ŷ�
	bool m_bAntiLoose;		//�Ƿ�����ɵ�Ȧ���������˫ñ�������Ի���,����ͬʱΪtrue��
	long m_hMappingAntiTheftFamily;	//��Ӧ�ķ�����˨ϵ��
	long m_hMappingAntiLooseFamily;	//��Ӧ�ķ�����˨ϵ��
	//��˨�淶
	UINT m_uiCodeSerial;		//ͬһ���ϵ��������淶�����ͬ������Ϊ��ͬһ��˨�淶
	//��˨��������Ϣ
	struct{
		WORD idSubPart;	//��ʶ
		WORD nCount;	//����
	}ls_sub_part[8];
public:
	CLsFamily();
	~CLsFamily();
	void SetFamilyHandle(long nKey){m_hFamily=nKey;}
	bool CreateFamily(int num);
	long GetCount(){return m_nFamilySize;}
	long GetFamilyHandle(){return m_hFamily;}
	__declspec(property(get=GetFamilyHandle)) long Id;
	LS_XING_HAO *RecordAt(int i);
	LS_XING_HAO *FindLsGuiGe(int d, double L0);
	LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len);
	LS_XING_HAO *GetFamily(){return m_pFamily;}
	//
	int GetComponentCount();
	int GetLsSubPartNum(int iSubPartType);
	void SetLsSubPartNum(int iSubPartType,int nNum);
	//
	void CopyLsFamily(CLsFamily* pSrcFamily);
};
//��˨����
class PARTLIB_API IBoltLibrary{
protected:
	void SetFamilyId(CLsFamily *pFamily,long hFamily);
public:
	//��˨�����������
	virtual long GetSerialId()=0;
	virtual CLsFamily* FromHandle(long h)=0;
	virtual CLsFamily* GetFirstFamily()=0;
	virtual CLsFamily* GetNextFamily()=0;
	virtual void ClearLibrary()=0;
	virtual int GetFamilyNum()=0;
	virtual CLsFamily* AppendFamily()=0;
	virtual CLsFamily* AppendFamily(long hFamily)=0;
	virtual BOOL RemoveFamily(long h)=0;
	//��˨����ѯ����
	virtual LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0)=0;
	virtual LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,long hFamily=0)=0;
	//������˨��
	virtual void Copy(IBoltLibrary *pSrcBoltLib)=0;
	virtual void CopyLsFamily(CLsFamily* pNewFamily,CLsFamily* pSrcFamily)=0;
public:
	static BOOL GetLsSpaceEx(long diameter,LSSPACEEX* pSpace);
	static BOOL SetLsSpaceEx(long diameter,LSSPACEEX& space);
	static IBoltLibrary *CreateBoltLibrary();
	static BOOL DestroyBoltLibrary(long serialId);
};
class PARTLIB_API CBoltLibBox
{
	IBoltLibrary *pBoltLib;
public:
	IBoltLibrary *BoltLib(){return pBoltLib;}
	CBoltLibBox(){pBoltLib=IBoltLibrary::CreateBoltLibrary();}
	~CBoltLibBox(){IBoltLibrary::DestroyBoltLibrary(pBoltLib->GetSerialId());}
};
class PARTLIB_API CLsLibrary{
public:
	CLsLibrary();
	static long m_hDefaultFamily;
	//��˨�����������
	static CLsFamily* DefaultFamily();
	static CLsFamily* FromHandle(long h);
	static CLsFamily* GetFirstFamily();
	static CLsFamily* GetNextFamily();
	static void ClearLibrary();
	static int GetFamilyNum();
	static CLsFamily* AppendFamily();
	static CLsFamily* AppendFamily(long hFamiy);
	static BOOL RemoveFamily(long h);
	static long SetFootNailFamily(long hDefaultNailFamily);
	static long GetFootNailFamily();
	//��˨����ѯ����
	static LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0);
	static LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,long hFamily=0);
	//��˨������������
	static CLsSubPartFamily* AppendLsSubPartFamily(WORD widType=0);
	static CLsSubPartFamily* FromLsSubPartHandle(long h);
	static CLsSubPartFamily* GetFirstLsSubPartFamily();
	static CLsSubPartFamily* GetNextLsSubPartFamily();
	static BOOL RemoveLsSubPartFamily(long h);
	static void ClearLsSubPartLibrary();
	static int GetLsSubPartFamilyNum();
};
bool PARTLIB_API InitPartLibrary(char* libfile,IBoltLibrary* pBoltLib=NULL);
//������˨ϵ���ļ�
struct LOCAL_BOLT_CODESERIES{
	long m_hFamily;			//��˨ϵ�б�ʶ��
	UINT m_uiCodeSerial;	//ͬһ���ϵ��������淶�����ͬ������Ϊ��ͬһ��˨�淶
	char m_sCodeName[50];	//��˨ϵ����ѭ�Ĺ淶����
	char m_sFilePath[200];	//��Ӧ�ı�����˨���ļ�����Դ洢·��
	//
	LOCAL_BOLT_CODESERIES(){
		m_hFamily=m_uiCodeSerial=1;
		strcpy(m_sCodeName,"");
		strcpy(m_sFilePath,"");
	}
	LOCAL_BOLT_CODESERIES operator =(const LOCAL_BOLT_CODESERIES& lsSeries)
	{
		m_hFamily=lsSeries.m_hFamily;
		m_uiCodeSerial=lsSeries.m_uiCodeSerial;
		strcpy(m_sCodeName,lsSeries.m_sCodeName);
		strcpy(m_sFilePath,lsSeries.m_sFilePath);
		return *this;
	}
};
bool PARTLIB_API InitLocalBoltSeries(const char* szBoltLibWorkFolder);
int PARTLIB_API GetLocalBoltSeries(long hFamily,LOCAL_BOLT_CODESERIES* series);
bool PARTLIB_API GetLocalLsFamily(long hFamily,char* sCodeName,CLsFamily* pLsFamily);
//�׽Ű�����ṹ����
typedef struct PARTLIB_API {
	char guige[4];	//�׽���˨���
	int	 D;			//����׾�
	int	 S;			//����׾�
	int  L;			//����߾�
	int  A;
	int  B;
	int  C;
	int  E;
	int Thick;
}DIJIAOBAN_STRU;

//�������²���ȷ���ֹܲ���ϵ���˨�����Լ�λ��
typedef struct PARTLIB_API{
	int iBoltNum;	//��˨����
	int iRow;		//��˨����
	int iColumn;	//��˨����
	int iBoltD;		//��˨ֱ��
	double fBoltPosX;	//�������׸���˨��X����(�������)
	double fBoltPosY;	//�������׸���˨��Y����(�������)
	double fHorzSpace;	//��������˨�ĺ�����
	double fVertSpace;	//��������˨��������
}LAYOUT_LS_PARAM;
//�ֹܲ�嶨��ṹ(������˨������λ�ö���)
typedef struct PARTLIB_API {
	//����������������ȷ���ֹܲ������
	double d;	//����ֱ��
	double t;	//���ܺ��
	char cMaterial;	//������
	int	iLevelMark;	//�����־ ��������ʾ��Ӧ��ϵ����Ҳ����������ͬһ���ֹ������ӵĲ�ͬ���  wht 10-01-19
	char codeName[16];//������
	char datumPlatePartNo[16];	//������	wht 10-02-23
	char weldPlatePartNo[16];	//������
	char wrapPlatePartNo[16];	//������
	char cTubeMat;	//���Ӹֹܲ��� ����ȷ���������	wht 10-01-21
	//���������߸�����ȷ�������״
	double L;	//����
	double M;	//����ֹܲ��ֵĳ���
	double H;	//���
	double W;	//��߸߶�/U�Ͱ���������ľ���/ʮ�ֲ�帽�Ӱ�ĺ��
	double N;	//�нǳ���/ʮ�ֲ�帽�Ӱ�Ŀ��
	double S;	//�нǿ��/ʮ�ֲ�帽�Ӱ�ĳ���
	int Thick;	//���
	//����Ϊͳ������ز���
	double theroy_weight;//��������
	double A;	//ʮ�ֲ�帽�Ӱ�Ķ�λ����
	double R;	//ʮ�ֲ�帽�Ӱ�Ķ�λ����
	//��˨�����������˨λ�ò���
	int iBoltRect;	//��˨�������
	LAYOUT_LS_PARAM ls_param[6];
}INSERT_PLATE_PARAM;

//���������ṹ����(������˨������λ�ö���)
typedef struct PARTLIB_API {
	//����������������ȷ���������
	double d;		//����ֱ��
	double couple_d;//�ԽӸֹ�ֱ��
	double t;	//���ܺ��
	char cLevelMark;//�����־,��ABCD�ĸ��ȼ�
	char cMaterial;	//��������
	char codeName[17];//��������
	//�������²���ȷ��������״
	double A;	//�����⾶/�ֹ���ֱ��
	double B;	//�����ھ�
	double D;	//�����⾶
	double K;	//��˨�׶�λԲֱ��
	double L;	//��˨�׾�
	int	nBoltNum;//��˨���� 
	int nBoltD;	//��˨ֱ��
	double C;	//�������
	double H;	//�����߶�
	double N;	//��������ֱ��������R�ߴ�
	double S;	//����������� //�Ժ�����ʱS��¼S��������ܺ�ȵ����� wht 10-01-20
	double H1;	//���������߶� ƽ�������˲����ֹܶ��뷨������ľ��� 
	double R;	//�����������ǰ뾶	

	struct THICK_GROUP{
		double weight;	//��������,kg
		int t;		//��Ӧ���ܱں�,mm
	}group[5];	//�Ժ��������ݸֹܱں���еķ���A,B,C,D,E
	double Weight();//���ݵ�ǰ���ܱں�t������Ӧ�������������,kg
	char ThickGroupSymbol(int thick=0);	//���ص�ǰ�ں��ָ���ں��Ӧ�ķ����ַ� A,B,C,D,E
	bool IsSameCode(const char* code);		//�ж϶Ժ����������ܱں�����ʶ�ַ�����������Ƿ�һ��
	void UpdateCodeName();			//���¶Ժ���������(������ܱں��ʶ�ַ�)
	/*short tB;//�Ժ�������Ӧ�ֹܱں񼶱�A,B,C,D,E
	double wB;
	short tC;
	double wC;
	short tD;
	double wD;
	short tE;
	double wE;*/
	//double cur_t;	//��¼��ǰ���ܺ��		��ʱ�������洢	wht 10-01-20
	//double cur_S;	//��¼��ǰ�����������  ��ʱ�������洢
}FL_PARAM;

//���Է��������Է��������ṹ����(������˨������λ�ö���)
typedef struct PARTLIB_API tagFL_PARAM2{
	//����������������ȷ���������
	double d;		//����ֱ��
	double couple_d;//�ԽӸֹ�ֱ��
	char cLevelMark;//�����־,��ABCD�ĸ��ȼ�
	char cMaterial;	//��������
	char codeName[17];//��������
	//�������²���ȷ��������״
	double fInnerD;		//�����ھ�
	double fOuterD;		//�����⾶
	double fBoltCirD;	//��˨�׶�λԲֱ��
	double fBoltHoleD;	//��˨�׾�
	int	nBoltNum;		//��˨���� 
	int nBoltD;			//��˨ֱ��
	double fThick;		//�������
	double fH;			//�ֹܶ��뷨������ľ���
	double theroy_weight;//��������
	//�߰����β���
	int nRibPlateNum;	//�߰����
	struct RIB_PLALTE_PARM
	{
		double weight;	//�߰�����
		char cMat;	//�߰����
		double T;	//�߰���
		double A;	//�߰�߶�
		double B;	//�߰�ײ����
		double C;	//�߰����߶�
		double D;	//�߰嶥�����
		double E;	//���ǿ��
		double F;	//�׶�λ����
		double G;	//�׶�λ����
		double H;	//��ֱ��
	}para;
	double SumWeight();
	void CopyPropertye(const tagFL_PARAM2 src_param);
}FL_PARAM2;

//�ֹܷ�����Сͷ���ù����
typedef struct PARTLIB_API	{
	double d;	//����ֱ��
	double A;	//�ԽӸֹ�ֱ�����Ⱦ�
	double B;	//�ԽӸֹ�ֱ��
	double C;	//�ԽӸֹ�ֱ��
	double D;	//�ԽӸֹ�ֱ��
}FL_COUPLE_RULE;
struct FL_COUPLE{
	double main_d;				//����ֱ��
	double couple_d;			//�Խӹ�ֱ��
	char mainFlName[17];	//�������ӷ�������
	char coupleFlName[17];	//�������ӷ�������
};
//�Ǹֽ�ͷ��ز���������,��ʱ���ڴ˴�,�Ժ�����PartLib.dll�� wht 11-01-25
struct PARTLIB_API JG_JOINT_PARA
{
	double fBaseJgWidth;		//��׼�Ǹֿ��
	double fBaseJgThick;		//��׼�Ǹֺ��
	double fInnerJgWidth;		//�ڰ��Ǹֿ��
	double fInnerJgThick;		//�ڰ��Ǹֺ��
	double fOuterPlateWidth;	//����ְ���
	double fOuterPlateThick;	//����ְ���
	double fOuterJgWidth;		//����Ǹֿ��
	double fOuterJgThick;		//����Ǹֺ��
};
//���
struct PARTLIB_API FITTING_GD{	
	char   szSizeCode[16];	//�ͺŴ���
	short  siThick;			//�Ұ���b
	short  siThroughThick;	//Ԥ�����߽Ǹ�֫(ͨ)��
	double dfTrunnionD;		//����ֱ��
	double dfHoleD;			//�ҿ�ֱ��(diameter)��
	double dfBoardH;		//��˨�����ĵ���Բ���ĳ�H
	double dfBoardH1;		//���ᵽ��Բ���ĳ�H1
	double dfBoardH2;		//�ҿ׵����������߾���H2
	double dfTrunnionL;		//�����ܳ�
	double dfBoardL;		//���(�Ұ岿��)��ȣ���Բ����
};
struct PARTLIB_API FITTING_EB{
	char   szSizeCode[16];	//�ͺŴ���
	short  siThick;			//�Ұ���b
	short  siThroughThick;	//Ԥ�����߽Ǹ�֫(ͨ)��
	double dfTrunnionD;		//����ֱ��
	double dfHoleD;			//�ҿ�ֱ��
	double dfBoardL;		//���(�Ұ岿��)���
	double dfBoardH;		//�ҿ׵����������߾���H
	double dfChinR;			//�ҿ��°�Բ���뾶
};
struct PARTLIB_API FITTING_UR
{
	char szSizeCode[16];	//�ͺŴ���
	char ciType;			//0.��ͨ��U�ιһ�;1.�ӳ���U�ιһ�
	double dfNamedL;		//���峤(ת�������������ڲ�)L��mm
	double dfShaftD;		//ת����˨ֱ��d
	double dfMdEdgeSpace;	//U�ͻ���ת����ĸ������ı߾�D��mm
	double dfT;				//U�ͻ���EB��GD�Ұ������Ӳ��������ӱں�T��mm
	double dfRingBs;		//U�ͻ���ֱ��B(s),
	double dfBoardSpaceC;	//U�ͻ���EB��GD�Ұ������Ӽ�϶(>=EB/GD�Ұ���)
	double dfRadiusOfRing;	//U�ͻ�Բ���ڲ�뾶R��һ��>=dfBoardSpaceC/2
};
//�����������������������������������������
extern PARTLIB_API WORD zhunju_N;
extern PARTLIB_API int tubeguige_N;	//��ת��˼�
extern PARTLIB_API int bianguige_N;	//�����˼�
extern PARTLIB_API int slotguige_N;	//�����˼�
extern PARTLIB_API int hw_guige_N;	//����ԵH�ָ˼�
extern PARTLIB_API int hm_guige_N;	//����ԵH�ָ˼�
extern PARTLIB_API int hn_guige_N;	//խ��ԵH�ָ˼�
extern PARTLIB_API int hp_guige_N;	//����ԵH�ָ˼�
extern PARTLIB_API int eb_guige_N;	//EB�Ұ���
extern PARTLIB_API int gd_guige_N;	//GD�Ұ���
extern PARTLIB_API int ur_guige_N;	//U�ͻ����
extern PARTLIB_API int LsTypeCount;
extern PARTLIB_API int g_nJgJointRecordNum;
//extern PARTLIB_API int nLsXingHaoCount;//��ǰ�汾��˨���ͼ�¼��
//extern PARTLIB_API LS_XING_HAO lsguige_table[];
extern PARTLIB_API LS_GRADE ls_table[];
extern PARTLIB_API ANGLE_G_REC zhunju_table[];
extern PARTLIB_API int lsspace_N;		//���15
extern PARTLIB_API LSSPACE_STRU LSSPACE_TABLE[];	//��˨ֱ�������ż�࣬˫�ż�࣬�˾࣬����߾࣬���Ʊ߾࣬�нǱ߾࣬�����ڱ߾࣬������߾�
extern PARTLIB_API const double LS_APERTURE[4];
//��Ϊ���ݾɴ���̳ж�д�������´���ֱ����IAngleLibrary���� wjh-2019.9.11
struct PARTLIB_API SYSLIB_ANGLESIZE_N{
	SYSLIB_ANGLESIZE_N() { ; }
	operator int();
	int operator =(int N);
	SYSLIB_ANGLESIZE_N(int N);
};
extern PARTLIB_API SYSLIB_ANGLESIZE_N jgguige_N;	//���250
//��Ϊ���ݾɴ���̳ж�д�������´���ֱ����IAngleLibrary���� wjh-2019.9.11
struct PARTLIB_API SYSLIB_ANGLESIZE_TBL{
	SYSLIB_ANGLESIZE_TBL() { ; }
	operator JG_PARA_TYPE*();
};
extern PARTLIB_API SYSLIB_ANGLESIZE_TBL jgguige_table;	//֫��b;֫��d;��Բ�뾶r;��������
struct IAngleLibrary {	//����֧�����޽Ǹֹ������� wjh-2019.9.11
	virtual int AngleCount()=0;
	virtual int SetAngleCount(int count)=0;
	virtual JG_PARA_TYPE* GetAt(int index)=0;
	virtual JG_PARA_TYPE* SetAt(int index,const JG_PARA_TYPE& anglesize)=0;
	virtual JG_PARA_TYPE* AppendAngleSize(const JG_PARA_TYPE& anglesize)=0;
	virtual JG_PARA_TYPE* GetAngleSizeTbl()=0;
	operator JG_PARA_TYPE*() { return GetAngleSizeTbl(); }
public:	//����
	int get_AngleCount() { return AngleCount(); }
	int set_AngleCount(UINT count) { return SetAngleCount(count); }
	__declspec(property(put=set_AngleCount,get=get_AngleCount)) int Count;
	__declspec(property(put=set_AngleCount,get=get_AngleCount)) int jgguige_N;
	JG_PARA_TYPE* get_pxAnglTbl() { return GetAngleSizeTbl(); }
	//֫��b;֫��d;��Բ�뾶r;��������(�������jgguige_table���Ӧ) wjh-2019.9.11
	__declspec(property(get=get_pxAnglTbl)) JG_PARA_TYPE* pxAnglTbl;
};
PARTLIB_API IAngleLibrary* AngleLibrary();

extern PARTLIB_API SLOT_PARA_TYPE slotguige_table[];//��h;��d;��b;֫��t;
extern PARTLIB_API HSHAPE_PARA_TYPE hw_guige_table[];
extern PARTLIB_API HSHAPE_PARA_TYPE hm_guige_table[];
extern PARTLIB_API HSHAPE_PARA_TYPE hn_guige_table[];
extern PARTLIB_API HSHAPE_PARA_TYPE hp_guige_table[];
extern PARTLIB_API FITTING_GD gxarrGDLib[];
extern PARTLIB_API FITTING_EB gxarrEBLib[];
extern PARTLIB_API FITTING_UR gxarrURLib[];
extern PARTLIB_API int rollend_param_N;	//���Ͳ����¼��
extern PARTLIB_API int uend_param_N;	//U�Ͳ����¼��
extern PARTLIB_API int xend_param_N;	//X�Ͳ����¼��
extern PARTLIB_API int flp_param_N;		//ƽ���������¼��
extern PARTLIB_API int fld_param_N;		//�Ժ��������¼��
extern PARTLIB_API int flg_param_N;		//���Է������¼��
extern PARTLIB_API int flr_param_N;		//���Է������¼��
extern PARTLIB_API int fl_couple_rule_N;	//�����Խӹ����¼��
extern PARTLIB_API TUBE_PARA_TYPE tubeguige_table[];
extern PARTLIB_API BIAN_PARA_TYPE bianguige_table[];
extern PARTLIB_API DIJIAOBAN_STRU diban_weld_table[];	//����ѥ��׽Ű�
extern PARTLIB_API DIJIAOBAN_STRU diban_lslj_table[];	//��˨����ѥ��׽Ű�
extern PARTLIB_API INSERT_PLATE_PARAM rollend_param_table[];//���Ͳ���
extern PARTLIB_API INSERT_PLATE_PARAM uend_param_table[];	//U�Ͳ���
extern PARTLIB_API INSERT_PLATE_PARAM xend_param_table[];	//X�Ͳ���
extern PARTLIB_API FL_PARAM flp_param_table[];				//ƽ��������
extern PARTLIB_API FL_PARAM fld_param_table[];				//�Ժ�������
extern PARTLIB_API FL_PARAM2 flg_param_tbl[];				//���Է�����
extern PARTLIB_API FL_PARAM2 flr_param_tbl[];				//���Է�����
extern PARTLIB_API FL_COUPLE_RULE fl_couple_rule_table[];	//�����Խӹ����
extern PARTLIB_API JG_JOINT_PARA jg_joint_table[];	//�Ǹֽ�ͷ��Ӧ��
PARTLIB_API JG_JOINT_PARA *FindJgJointPara(double main_w,double main_t);
PARTLIB_API BOOL FindInnerAngleGuiGe(double main_w,double main_t,double &inner_w,double &inner_t);
PARTLIB_API BOOL FindOuterAngleGuiGe(double main_w,double main_t,double &outer_w,double &outer_t);
PARTLIB_API BOOL FindOuterPlateGuiGe(double main_w,double main_t,double &plate_w,double &plate_t);
//�����������������������������������������
//	 _____     |  o o  |
//	|_o_o_|	or |_o_o_o_|	����>�ܳ�
PARTLIB_API int GetLsGroupLen(int M_Diameter, int nLsN, int nRowsN);//�����˨�鳤��
PARTLIB_API BOOL GetLsSpace(LSSPACE_STRU& LsSpace, long diameter);
//��ѯ�Ǹֵ���Բ���뾶r
PARTLIB_API double FindJg_r(double wing_wide);
PARTLIB_API int getjgzj(JGZJ &jgzj, double wing_wide);//
PARTLIB_API BIAN_PARA_TYPE* FindBianType(double wide,double thick);
PARTLIB_API LS_GRADE *FindLsMatType(char *grade);
//PARTLIB_API LS_XING_HAO *FindLsGuiGe(int d, double L0,double grade=0);
//PARTLIB_API LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,double grade=0);
//ls_type012,0:����˨��;1:����˨��һ����;2:����˨��˫ñ��
//PARTLIB_API double FindLsWei(long d,long valid_len,int ls_type012=0);
//PARTLIB_API void FindLsMinL0AndMaxL0(int ls_d, int *min_L0, int *max_L0);//����ָ��ֱ����˨����Сͨ���Լ����ͨ��
PARTLIB_API void FindLsD_H(int d, double *D,double *H,double *H2);//��ѯ��˨��ֱ������ñ��
PARTLIB_API void FindBoltCapD_H(int d, double *D,double *H,int idBoltSubPart=0);//��ѯ��˨��ֱ������ñ��
PARTLIB_API double FindPadWei(int thick, int d);//thick��ʾ��Ȧ�ĺ��,d��ʾ�˵�Ȧ�ĸ�����˨ֱ��(���)
PARTLIB_API JG_PARA_TYPE* FindJgType(double wide,double thick,char cType='L');
PARTLIB_API TUBE_PARA_TYPE* FindTubeType(double D,double thick);
PARTLIB_API SLOT_PARA_TYPE* FindSlotType(char *guige);	//���ݹ����Ҳ۸�
PARTLIB_API HSHAPE_PARA_TYPE* FindHWType(double wide,double height);
PARTLIB_API HSHAPE_PARA_TYPE* FindHMType(double wide,double height);
PARTLIB_API HSHAPE_PARA_TYPE* FindHNType(double wide,double height);
PARTLIB_API HSHAPE_PARA_TYPE* FindHPType(double wide,double height);
PARTLIB_API FITTING_EB* FindEBFitting(const char* sSpec);
PARTLIB_API FITTING_GD* FindGDFitting(const char* sSpec);
PARTLIB_API FITTING_UR* FindURFitting(const char* sSPec);
//PARTLIB_API double GetJgUnitLenWei(double wing_wide,double wing_thick);//����Ϊ���ȵ�λ
//PARTLIB_API double GetTubeUnitLenWei(double tube_d,double tube_t);//����Ϊ���ȵ�λ	�ֹܵ�λ��������
PARTLIB_API char QuerySteelBriefMatMark(char *steelmark);
PARTLIB_API BOOL QuerySteelMatMark(char briefmark,char* steelmark,int niMaxStrBuffLen=8);
PARTLIB_API int QuerySteelMatIndex(char briefmark);
PARTLIB_API STEELMAT *QuerySteelMatByBriefMark(char briefmark);
PARTLIB_API double CalSteelPlankWei(double volume);//���������λΪmm3 ���������λΪ����(Kg)
PARTLIB_API int QueryDiBanParaByWide(int wide,char cItem,BOOL is_weld=FALSE);//���ݵ׽Ű������ѯ�׽Ű����������ֵ
PARTLIB_API INSERT_PLATE_PARAM* FindRollEndParam(const char *codeName);	//���Ҳ��Ͳ�����
PARTLIB_API INSERT_PLATE_PARAM* FindUEndParam(const char *codeName);		//����U�Ͳ�����
PARTLIB_API INSERT_PLATE_PARAM* FindXEndParam(const char *codeName);		//����X�Ͳ�����
PARTLIB_API INSERT_PLATE_PARAM* FindRollEndParam(double d,double t,char cTubeMat,int iLevel);	//���Ҳ��Ͳ�����
PARTLIB_API INSERT_PLATE_PARAM* FindUEndParam(double d,double t,char cTubeMat,int iLevel);//����U�Ͳ�����
PARTLIB_API INSERT_PLATE_PARAM* FindXEndParam(double d,double t,char cTubeMat,int iLevel);//����X�Ͳ�����
//PARTLIB_API FL_PARAM* FindFlPParam(double d,double t,char cMaterial,char cMark);//����ƽ����������
//PARTLIB_API FL_PARAM* FindFlPParam(double d,double t,char cMark);				//����ƽ����������,���Բ���
PARTLIB_API FL_PARAM* FindFlPParam(const char *codeName);								//����ƽ����������
//
PARTLIB_API FL_PARAM* FindFlDParam(double d,double couple_d,char cMaterial,char cMark);//���ҶԺ���������
PARTLIB_API FL_PARAM* FindFlDParam(double d,double couple_d,char cMark);			//���ҶԺ���������,���Բ���
PARTLIB_API FL_PARAM* FindFlDParam(const char *codeName);								//���ҶԺ���������
//
PARTLIB_API FL_PARAM2* FindFlGParam(const char *codeName);
PARTLIB_API FL_PARAM2* FindFlGParam(double d,double couple_d,char cMat,char cMark);
PARTLIB_API FL_PARAM2* FindFlGParam(double d,double couple_d,char cMark);
//
PARTLIB_API FL_PARAM2* FindFlRParam(const char *codeName);
PARTLIB_API FL_PARAM2* FindFlRParam(double d,double couple_d,char cMat,char cMark);
PARTLIB_API FL_PARAM2* FindFlRParam(double d,double couple_d,char cMark);
//��ѯ�ֹܷ�����Сͷ��Է���:0��ʾδ�ҵ���Է���;1��ʾ���ܷ����ҵ�������Է���δ�ҵ�;2���������ӹܵ����ȫ���ҵ�
PARTLIB_API int QueryFlCouple(double fTubeD,double fOtherTubeD,char cFlType,FL_COUPLE* couple=NULL);
//
//PARTLIB_API	FL_COUPLE_RULE* QueryFlCoupleRule(double fTubeD,char cMark);		//��ѯ�����Խӹ���
//PARTLIB_API	FL_COUPLE_RULE* QueryFlCoupleRule(double fTubeD,double fOtherTubeD);//��ѯ�����Խӹ���
//PARTLIB_API	char QueryFlLevelMark(FL_COUPLE_RULE* pFlCoupleRule,double fTubeD);	//��ѯ�����ȼ���־
//PARTLIB_API double QueryCoupleTubeD(double fMainTubeD,char cMark);//�������ܹ���Լ������ԽӼ�����ҶԽӸֹ�ֱ��
#endif