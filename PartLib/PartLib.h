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
//－－－－－－输出类（或结构）声明－－－－－－－－－
//角钢规格 准距g 准距g1 准距g2 准距g3
typedef struct PARTLIB_API{	// 角钢的:
	//double  r;//内圆弧半径        Wjh--00.04.20
	long  g;    //单排螺栓时的准距  Wjh--00.04.20
	long  g1;   // 2排螺栓时, 使用g1和g2准距  Wjh--00.04.20
	long  g2;
	long  g3;	// g,g1,g2肯定用到,r没用到.g3是三排螺栓的,现在只考虑2排,所以没用到.
	BYTE MatchStyleG(double gDist,bool bPreferMultiBoltG=true);
	long ByStyleG(int indexOfStyleG,double gDefault=0);
} JGZJ;//角钢准距   Wjh--00.04.19
typedef struct PARTLIB_API STEELMAT{
	char cBriefMark;
	char cPreferQualityLevel;	//默认质量等级
	char mark[8];
	int t_lim[3];			//厚度或直径mm
	double E;				//弹性模量(MPa)
	double price_coef;		//价格系数
	double fw3;				//角焊缝抗拉压和抗剪强度设计值M(Pa)
	double intensity[16];	//各组强度值
	double fy(double t);	//屈服强度(t为厚度或直径mm)
	double f(double t);		//抗拉、压、弯强度设计值(t为厚度或直径mm)
	double fv(double t);	//抗剪强度设计值(t为厚度或直径mm)
	double fc(double t);	//孔壁承压强度设计值(t为厚度或直径mm)
	double fw1(double t){return f(t);}	//对焊焊缝抗拉压强度设计值M(Pa)
	double fvw(double t){return fv(t);}	//对焊焊缝抗剪强度设计值M(Pa)
}*STEELMATPTR;
//材料库类定义
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
//肢宽b;肢厚d;内圆半径r;理论重量
typedef struct{
	double wing_wide;		//肢宽
	double wing_thick;		//肢厚
	double r;				//内圆弧半径
	double area;			//截面面积(cm2)
	double theroy_weight;	//理论重量
	double Ix;		//平行轴惯性矩(cm4)
	double Rx;		//平行轴回转半径(cm)
	double Wx;		//平行轴抗弯截面模量(cm3)
	double Iy0;		//最小轴惯性矩(cm4)
	double Ry0;		//最小轴回转半径(cm)
	double Wy0;		//最小轴抗弯截面模量(cm3)
	char   cType;	//杆件类型L、T、D、X、C、R、B、S
}JG_PARA_TYPE;
typedef struct ANGLE_G_REC{
	WORD wide;
	WORD g,g1,g2,g3;
}*ANGLE_G_RECPTR;
//肢宽b;肢厚d;内圆半径r;理论重量
//回转类杆件参数(主要指可抗弯的钢管等回转构件)
typedef struct PARTLIB_API {
	double D;		//直径
	double thick;	//壁厚
	double area;			//截面面积(cm2)
	double theroy_weight;	//理论重量
	double I;		//惯性矩(cm4)
	double r;		//回转半径(cm)
	double W;		//抗弯截面模量(cm3)
}TUBE_PARA_TYPE;
//扁铁类杆件参数(主要指扁铁等只承受拉力的构件)
typedef struct PARTLIB_API {
	double wide;	//宽度
	double thick;	//壁厚
	double theroy_weight;	//理论重量
}BIAN_PARA_TYPE;
//槽钢类杆件参数
typedef struct{
	char   guige[16];//槽钢规格
	double h;		//高
	double b;		//宽
	double d;		//厚
	double t;		//肢宽
	double r;		//内圆弧半径
	double area;			//截面面积(cm2)
	double theroy_weight;	//理论重量
	double Ix;		//平行轴惯性矩(cm4)
	double Rx;		//平行轴回转半径(cm)
	double Wx;		//平行轴抗弯截面模量(cm3)
	double Iy;		//最小轴惯性矩(cm4)
	double Ry;		//最小轴回转半径(cm)
	double Wy;		//最小轴抗弯截面模量(cm3)
	double Z0;		//
}SLOT_PARA_TYPE;
//H型钢杆件参数
typedef struct{
	double h;		//高
	double b;		//宽
	double t1;		//腹板厚度
	double t2;		//翼缘厚度
	double r;		//内圆弧半径
	double area;	//截面面积
	double theroy_weight;	//理论重量
	double Ix;		//平行轴惯性矩(cm4)
	double Iy;		//最小轴惯性矩(cm4)
	double Rx;		//平行轴回转半径(cm)
	double Ry;		//最小轴回转半径(cm)
	double Wx;		//平行轴抗弯截面模量(cm3)
	double Wy;		//最小轴抗弯截面模量(cm3)
}HSHAPE_PARA_TYPE;
typedef struct{
	char grade[5];	//螺栓级别4.8;5.8;6.8;8.8;10.9
	double f;		//抗拉强度设计值
	double fv;		//抗剪强度设计值
	double fc;		//孔壁承压强度设计值
}LS_GRADE;
//螺栓零件
struct LS_SUB_PART{
	WORD	wiLsD;		//螺栓直径
	double	thick;		//零件厚度
	double	weight;		//零件重量
	double  dfShapeDs;	//零件外形参数D(默认为螺栓对边宽度)
	double _dfShapeDe;	//零件外形参数D2(六角螺栓默认为螺栓对角顶点宽度)
	double set_dfShapeDe(double dfDe);//零件外形参数D2(六角螺栓默认为螺栓对角顶点宽度)
	double get_dfShapeDe();//零件外形参数D2(六角螺栓默认为螺栓对角顶点宽度)
	__declspec(property(put=set_dfShapeDe,get=get_dfShapeDe)) double dfShapeDe;

};
class PARTLIB_API CLsSubPartFamily{
	int		m_nSize;				//该系列含有的螺栓子零件数量
	LS_SUB_PART* m_pLsSubPartSet;	//螺栓子零件集合
public:
	static const WORD TYPE_COMMON_NUT	= 1;	//普通螺母
	static const WORD TYPE_FASTEN_NUT	= 2;	//扣紧螺母
	static const WORD TYPE_ANTITHEFT_NUT= 3;	//防盗螺母
	static const WORD TYPE_THIN_NUT		= 4;	//薄螺母
	static const WORD TYPE_COMMON_GASKET= 5;	//平垫圈
	static const WORD TYPE_SPRING_GASKET= 6;	//弹簧垫圈
	static const WORD TYPE_LOCK_CLASP	= 7;	//防松扣
	//
	friend class CLsLibrary;
	WORD	m_widType;	//零件系列种类
	char	name[16];	//零件系列名称
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
//级别 直径 有效长 无扣长 通厚上限 通厚下限 螺帽直径 螺帽高
struct PARTLIB_API LS_XING_HAO{
	double grade;		//螺栓级别
	int	   d;			//螺栓直径
	char   guige[16];	//螺栓规格
	int	   valid_len;	//有效长
	int    no_thread_len;//无扣长
	int	   down_lim;	//螺栓通厚下限
	int	   up_lim;		//螺栓通厚上限
	int    washer_thick;//垫圈厚度
	double weight;//,pad_weight,twocap_weight;		//螺栓理论重量(不带弹簧垫)、带弹簧垫，及双帽
	LS_XING_HAO(){memset(this,0,sizeof(LS_XING_HAO));}
};
struct PARTLIB_API ANCHOR_BOLT{
	WORD d;	//名义直径
	WORD wiWidth,wiThick;	//垫板宽度与厚度
	WORD wiHoleIncrement;	//垫板孔径相对地脚螺栓直径的增大值
	WORD wiBasePlateHoleD;	//底脚板上对应孔径
	float m_fNutH;		//螺母高度
	float m_fNutTaper;	//螺母端面过渡圆锥面的坡度h/w;β=15°~30°
	float m_fNutDe;	//螺母顶点到顶点距离
	float m_fNutDs;	//螺母边到边距离
	float m_fLe;	//出露长(mm)
	float m_fLa;	//无扣长(mm) <底座板厚度
	//float m_fLo;	//丝扣长(mm)=出露长-无扣长
	static int GetCount();
	static int SetCount(int count);
	static const ANCHOR_BOLT* RecordAt(int i);
	static bool SetAnchorBolt(int i,ANCHOR_BOLT& anchorbolt);
	static const ANCHOR_BOLT* FromBoltD(WORD d);//根据地脚螺栓名义直径查询
};
//根据螺栓直径获得螺栓间、边距表结构
typedef struct PARTLIB_API {
	long M_Diameter;		//螺栓直径
	double Aperture;		//构件孔径
	long SingleRowSpace;	//单排间距
	long doubleRowSpace;	//双排间距
	long EndSpace;			//端距
	long PlateEdgeSpace;	//连板边距
	long RollingEdgeSpace;	//轧制边距
	long CutAngleSpace;		//切角边距
	long InnerFLSpace;		//螺栓到法兰内侧钢管壁最小边距
	long OutterFLSpace;		//螺栓到法兰外边绷最小边距
} LSSPACE_STRU;
//根据螺栓直径获得螺栓间、边距表结构
struct PARTLIB_API LSSPACEEX : public LSSPACE_STRU{
	long WasherEdgeSpace;	//垫板边距
	LSSPACEEX();
	LSSPACEEX(LSSPACE_STRU& space);
	operator LSSPACE_STRU();
	LSSPACEEX operator =(const LSSPACE_STRU& LsSpace);
};
//螺栓系列类定义
class PARTLIB_API CLsFamily{
	long m_hFamily;			//螺栓系列标识号
	int m_nFamilySize;		//该系列含有的螺栓规格种类数
	LS_XING_HAO *m_pFamily;	//螺栓规格种类的存储区
public:
	friend class CLsLibrary;
	friend class IBoltLibrary;
	char name[17];	//螺栓系列名称
	char descr[51];	//螺栓系列的备注描述
	bool m_bDualCap;//螺栓帽数量,之前名为int m_nLsCap,后发现防盗实际也是双帽,原本的双帽是指双普通帽,故直接采用bool类型更合适 wjh-2017.11.24
	bool m_bBurglarproof;	//是否为防盗螺栓
	bool m_bFootNail;		//是否为脚钉
	bool m_bAntiLoose;		//是否带防松垫圈（与防盗、双帽另两属性互斥,不能同时为true）
	long m_hMappingAntiTheftFamily;	//对应的防盗螺栓系列
	long m_hMappingAntiLooseFamily;	//对应的防松螺栓系列
	//螺栓规范
	UINT m_uiCodeSerial;		//同一规格系列内如果规范序号相同，则认为是同一螺栓规范
	//螺栓零件组成信息
	struct{
		WORD idSubPart;	//标识
		WORD nCount;	//数量
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
//螺栓规格库
class PARTLIB_API IBoltLibrary{
protected:
	void SetFamilyId(CLsFamily *pFamily,long hFamily);
public:
	//螺栓规格库操作函数
	virtual long GetSerialId()=0;
	virtual CLsFamily* FromHandle(long h)=0;
	virtual CLsFamily* GetFirstFamily()=0;
	virtual CLsFamily* GetNextFamily()=0;
	virtual void ClearLibrary()=0;
	virtual int GetFamilyNum()=0;
	virtual CLsFamily* AppendFamily()=0;
	virtual CLsFamily* AppendFamily(long hFamily)=0;
	virtual BOOL RemoveFamily(long h)=0;
	//螺栓规格查询函数
	virtual LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0)=0;
	virtual LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,long hFamily=0)=0;
	//复制螺栓库
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
	//螺栓规格库操作函数
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
	//螺栓规格查询函数
	static LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0);
	static LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,long hFamily=0);
	//螺栓零件库操作函数
	static CLsSubPartFamily* AppendLsSubPartFamily(WORD widType=0);
	static CLsSubPartFamily* FromLsSubPartHandle(long h);
	static CLsSubPartFamily* GetFirstLsSubPartFamily();
	static CLsSubPartFamily* GetNextLsSubPartFamily();
	static BOOL RemoveLsSubPartFamily(long h);
	static void ClearLsSubPartLibrary();
	static int GetLsSubPartFamilyNum();
};
bool PARTLIB_API InitPartLibrary(char* libfile,IBoltLibrary* pBoltLib=NULL);
//本地螺栓系列文件
struct LOCAL_BOLT_CODESERIES{
	long m_hFamily;			//螺栓系列标识号
	UINT m_uiCodeSerial;	//同一规格系列内如果规范序号相同，则认为是同一螺栓规范
	char m_sCodeName[50];	//螺栓系列遵循的规范名称
	char m_sFilePath[200];	//对应的本地螺栓库文件的相对存储路径
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
//底脚板参数结构定义
typedef struct PARTLIB_API {
	char guige[4];	//底脚螺栓规格
	int	 D;			//座板孔径
	int	 S;			//座板孔距
	int  L;			//座板边距
	int  A;
	int  B;
	int  C;
	int  E;
	int Thick;
}DIJIAOBAN_STRU;

//根据以下参数确定钢管插板上的螺栓个数以及位置
typedef struct PARTLIB_API{
	int iBoltNum;	//螺栓个数
	int iRow;		//螺栓行数
	int iColumn;	//螺栓列数
	int iBoltD;		//螺栓直径
	double fBoltPosX;	//矩阵中首个螺栓的X坐标(相对坐标)
	double fBoltPosY;	//矩阵中首个螺栓的Y坐标(相对坐标)
	double fHorzSpace;	//相邻两螺栓的横向间距
	double fVertSpace;	//相邻两螺栓的纵向间距
}LAYOUT_LS_PARAM;
//钢管插板定义结构(包括螺栓个数及位置定义)
typedef struct PARTLIB_API {
	//根据以下三个参数确定钢管插板类型
	double d;	//主管直径
	double t;	//主管厚度
	char cMaterial;	//插板材质
	int	iLevelMark;	//级别标志 可用来表示管应力系数，也可用来区分同一规格钢管上连接的不同插板  wht 10-01-19
	char codeName[16];//插板代号
	char datumPlatePartNo[16];	//基板编号	wht 10-02-23
	char weldPlatePartNo[16];	//垂板编号
	char wrapPlatePartNo[16];	//包板编号
	char cTubeMat;	//增加钢管材质 用来确定插板类型	wht 10-01-21
	//根据以下七个参数确定插板形状
	double L;	//长度
	double M;	//插入钢管部分的长度
	double H;	//宽度
	double W;	//卷边高度/U型板上下两面的距离/十字插板附加板的厚度
	double N;	//切角长度/十字插板附加板的宽度
	double S;	//切角宽度/十字插板附加板的长度
	int Thick;	//厚度
	//以下为统材料相关参数
	double theroy_weight;//理论重量
	double A;	//十字插板附加板的定位参数
	double R;	//十字插板附加板的定位参数
	//螺栓矩阵个数及螺栓位置参数
	int iBoltRect;	//螺栓矩阵个数
	LAYOUT_LS_PARAM ls_param[6];
}INSERT_PLATE_PARAM;

//法兰参数结构定义(包括螺栓个数及位置定义)
typedef struct PARTLIB_API {
	//根据以下三个参数确定法兰规格
	double d;		//主管直径
	double couple_d;//对接钢管直径
	double t;	//主管厚度
	char cLevelMark;//级别标志,分ABCD四个等级
	char cMaterial;	//法兰材质
	char codeName[17];//法兰代号
	//根据以下参数确定法兰形状
	double A;	//焊端外径/钢管外直径
	double B;	//法兰内径
	double D;	//法兰外径
	double K;	//螺栓孔定位圆直径
	double L;	//螺栓孔径
	int	nBoltNum;//螺栓个数 
	int nBoltD;	//螺栓直径
	double C;	//法兰厚度
	double H;	//法兰高度
	double N;	//法兰跟部直径，包含R尺寸
	double S;	//法兰顶部厚度 //对焊法兰时S记录S相对于主管厚度的增量 wht 10-01-20
	double H1;	//法兰顶部高度 平焊法兰此参数钢管端与法兰底面的距离 
	double R;	//法兰颈部倒角半径	

	struct THICK_GROUP{
		double weight;	//理论重量,kg
		int t;		//对应主管壁厚,mm
	}group[5];	//对焊法兰根据钢管壁厚进行的分组A,B,C,D,E
	double Weight();//根据当前主管壁厚t返回相应分组的理论重量,kg
	char ThickGroupSymbol(int thick=0);	//返回当前壁厚或指定壁厚对应的分组字符 A,B,C,D,E
	bool IsSameCode(const char* code);		//判断对焊法兰除主管壁厚分组标识字符外其余代号是否一致
	void UpdateCodeName();			//更新对焊法兰代号(添加主管壁厚标识字符)
	/*short tB;//对焊法兰对应钢管壁厚级别A,B,C,D,E
	double wB;
	short tC;
	double wC;
	short tD;
	double wD;
	short tE;
	double wE;*/
	//double cur_t;	//记录当前主管厚度		临时变量不存储	wht 10-01-20
	//double cur_S;	//记录当前法兰顶部厚度  临时变量不存储
}FL_PARAM;

//刚性法兰及柔性法兰参数结构定义(包括螺栓个数及位置定义)
typedef struct PARTLIB_API tagFL_PARAM2{
	//根据以下三个参数确定法兰规格
	double d;		//主管直径
	double couple_d;//对接钢管直径
	char cLevelMark;//级别标志,分ABCD四个等级
	char cMaterial;	//法兰材质
	char codeName[17];//法兰代号
	//根据以下参数确定法兰形状
	double fInnerD;		//法兰内径
	double fOuterD;		//法兰外径
	double fBoltCirD;	//螺栓孔定位圆直径
	double fBoltHoleD;	//螺栓孔径
	int	nBoltNum;		//螺栓个数 
	int nBoltD;			//螺栓直径
	double fThick;		//法兰厚度
	double fH;			//钢管端与法兰底面的距离
	double theroy_weight;//理论重量
	//肋板外形参数
	int nRibPlateNum;	//肋板个数
	struct RIB_PLALTE_PARM
	{
		double weight;	//肋板重量
		char cMat;	//肋板材质
		double T;	//肋板厚度
		double A;	//肋板高度
		double B;	//肋板底部宽度
		double C;	//肋板外侧高度
		double D;	//肋板顶部宽度
		double E;	//倒角宽度
		double F;	//孔定位参数
		double G;	//孔定位参数
		double H;	//孔直径
	}para;
	double SumWeight();
	void CopyPropertye(const tagFL_PARAM2 src_param);
}FL_PARAM2;

//钢管法兰大小头配置规则表
typedef struct PARTLIB_API	{
	double d;	//主管直径
	double A;	//对接钢管直径，等径
	double B;	//对接钢管直径
	double C;	//对接钢管直径
	double D;	//对接钢管直径
}FL_COUPLE_RULE;
struct FL_COUPLE{
	double main_d;				//主管直径
	double couple_d;			//对接管直径
	char mainFlName[17];	//主管连接法兰代号
	char coupleFlName[17];	//主管连接法兰代号
};
//角钢接头相关参数及函数,暂时放在此处,以后移至PartLib.dll中 wht 11-01-25
struct PARTLIB_API JG_JOINT_PARA
{
	double fBaseJgWidth;		//基准角钢宽度
	double fBaseJgThick;		//基准角钢厚度
	double fInnerJgWidth;		//内包角钢宽度
	double fInnerJgThick;		//内包角钢厚度
	double fOuterPlateWidth;	//外包钢板宽度
	double fOuterPlateThick;	//外包钢板厚度
	double fOuterJgWidth;		//外包角钢宽度
	double fOuterJgThick;		//外包角钢厚度
};
//金具
struct PARTLIB_API FITTING_GD{	
	char   szSizeCode[16];	//型号代码
	short  siThick;			//挂板厚度b
	short  siThroughThick;	//预留挂线角钢肢(通)厚
	double dfTrunnionD;		//耳轴直径
	double dfHoleD;			//挂孔直径(diameter)Ф
	double dfBoardH;		//螺栓孔中心到下圆弧的长H
	double dfBoardH1;		//耳轴到上圆弧的长H1
	double dfBoardH2;		//挂孔到耳轴中心线距离H2
	double dfTrunnionL;		//耳轴总长
	double dfBoardL;		//金具(挂板部分)宽度，上圆弧长
};
struct PARTLIB_API FITTING_EB{
	char   szSizeCode[16];	//型号代码
	short  siThick;			//挂板厚度b
	short  siThroughThick;	//预留挂线角钢肢(通)厚
	double dfTrunnionD;		//耳轴直径
	double dfHoleD;			//挂孔直径
	double dfBoardL;		//金具(挂板部分)宽度
	double dfBoardH;		//挂孔到耳轴中心线距离H
	double dfChinR;			//挂孔下巴圆弧半径
};
struct PARTLIB_API FITTING_UR
{
	char szSizeCode[16];	//型号代码
	char ciType;			//0.普通型U形挂环;1.加长型U形挂环
	double dfNamedL;		//名义长(转轴中心至环壁内侧)L，mm
	double dfShaftD;		//转轴螺栓直径d
	double dfMdEdgeSpace;	//U型环与转轴螺母贴合面的边距D，mm
	double dfT;				//U型环与EB或GD挂板间的连接部单侧连接壁厚T，mm
	double dfRingBs;		//U型环柱直径B(s),
	double dfBoardSpaceC;	//U型环与EB或GD挂板间的连接间隙(>=EB/GD挂板厚度)
	double dfRadiusOfRing;	//U型环圆环内侧半径R，一般>=dfBoardSpaceC/2
};
//－－－－－－输出变量声明－－－－－－－－－
extern PARTLIB_API WORD zhunju_N;
extern PARTLIB_API int tubeguige_N;	//回转类杆件
extern PARTLIB_API int bianguige_N;	//扁钢类杆件
extern PARTLIB_API int slotguige_N;	//扁钢类杆件
extern PARTLIB_API int hw_guige_N;	//宽翼缘H钢杆件
extern PARTLIB_API int hm_guige_N;	//中翼缘H钢杆件
extern PARTLIB_API int hn_guige_N;	//窄翼缘H钢杆件
extern PARTLIB_API int hp_guige_N;	//等翼缘H钢杆件
extern PARTLIB_API int eb_guige_N;	//EB挂板规格
extern PARTLIB_API int gd_guige_N;	//GD挂板规格
extern PARTLIB_API int ur_guige_N;	//U型环规格
extern PARTLIB_API int LsTypeCount;
extern PARTLIB_API int g_nJgJointRecordNum;
//extern PARTLIB_API int nLsXingHaoCount;//当前版本螺栓类型记录数
//extern PARTLIB_API LS_XING_HAO lsguige_table[];
extern PARTLIB_API LS_GRADE ls_table[];
extern PARTLIB_API ANGLE_G_REC zhunju_table[];
extern PARTLIB_API int lsspace_N;		//最大15
extern PARTLIB_API LSSPACE_STRU LSSPACE_TABLE[];	//螺栓直径，单排间距，双排间距，端距，连板边距，轧制边距，切角边距，法兰内边距，法兰外边距
extern PARTLIB_API const double LS_APERTURE[4];
//仅为兼容旧代码继承而写，建议新代码直接用IAngleLibrary操作 wjh-2019.9.11
struct PARTLIB_API SYSLIB_ANGLESIZE_N{
	SYSLIB_ANGLESIZE_N() { ; }
	operator int();
	int operator =(int N);
	SYSLIB_ANGLESIZE_N(int N);
};
extern PARTLIB_API SYSLIB_ANGLESIZE_N jgguige_N;	//最大250
//仅为兼容旧代码继承而写，建议新代码直接用IAngleLibrary操作 wjh-2019.9.11
struct PARTLIB_API SYSLIB_ANGLESIZE_TBL{
	SYSLIB_ANGLESIZE_TBL() { ; }
	operator JG_PARA_TYPE*();
};
extern PARTLIB_API SYSLIB_ANGLESIZE_TBL jgguige_table;	//肢宽b;肢厚d;内圆半径r;理论重量
struct IAngleLibrary {	//此类支持无限角钢规格库数量 wjh-2019.9.11
	virtual int AngleCount()=0;
	virtual int SetAngleCount(int count)=0;
	virtual JG_PARA_TYPE* GetAt(int index)=0;
	virtual JG_PARA_TYPE* SetAt(int index,const JG_PARA_TYPE& anglesize)=0;
	virtual JG_PARA_TYPE* AppendAngleSize(const JG_PARA_TYPE& anglesize)=0;
	virtual JG_PARA_TYPE* GetAngleSizeTbl()=0;
	operator JG_PARA_TYPE*() { return GetAngleSizeTbl(); }
public:	//属性
	int get_AngleCount() { return AngleCount(); }
	int set_AngleCount(UINT count) { return SetAngleCount(count); }
	__declspec(property(put=set_AngleCount,get=get_AngleCount)) int Count;
	__declspec(property(put=set_AngleCount,get=get_AngleCount)) int jgguige_N;
	JG_PARA_TYPE* get_pxAnglTbl() { return GetAngleSizeTbl(); }
	//肢宽b;肢厚d;内圆半径r;理论重量(与旧属性jgguige_table相对应) wjh-2019.9.11
	__declspec(property(get=get_pxAnglTbl)) JG_PARA_TYPE* pxAnglTbl;
};
PARTLIB_API IAngleLibrary* AngleLibrary();

extern PARTLIB_API SLOT_PARA_TYPE slotguige_table[];//宽h;厚d;高b;肢宽t;
extern PARTLIB_API HSHAPE_PARA_TYPE hw_guige_table[];
extern PARTLIB_API HSHAPE_PARA_TYPE hm_guige_table[];
extern PARTLIB_API HSHAPE_PARA_TYPE hn_guige_table[];
extern PARTLIB_API HSHAPE_PARA_TYPE hp_guige_table[];
extern PARTLIB_API FITTING_GD gxarrGDLib[];
extern PARTLIB_API FITTING_EB gxarrEBLib[];
extern PARTLIB_API FITTING_UR gxarrURLib[];
extern PARTLIB_API int rollend_param_N;	//槽型插板库记录数
extern PARTLIB_API int uend_param_N;	//U型插板库记录数
extern PARTLIB_API int xend_param_N;	//X型插板库记录数
extern PARTLIB_API int flp_param_N;		//平焊法兰库记录数
extern PARTLIB_API int fld_param_N;		//对焊法兰库记录数
extern PARTLIB_API int flg_param_N;		//刚性法兰库记录数
extern PARTLIB_API int flr_param_N;		//柔性法兰库记录数
extern PARTLIB_API int fl_couple_rule_N;	//法兰对接规则记录数
extern PARTLIB_API TUBE_PARA_TYPE tubeguige_table[];
extern PARTLIB_API BIAN_PARA_TYPE bianguige_table[];
extern PARTLIB_API DIJIAOBAN_STRU diban_weld_table[];	//焊接靴板底脚板
extern PARTLIB_API DIJIAOBAN_STRU diban_lslj_table[];	//螺栓连接靴板底脚板
extern PARTLIB_API INSERT_PLATE_PARAM rollend_param_table[];//槽型插板库
extern PARTLIB_API INSERT_PLATE_PARAM uend_param_table[];	//U型插板库
extern PARTLIB_API INSERT_PLATE_PARAM xend_param_table[];	//X型插板库
extern PARTLIB_API FL_PARAM flp_param_table[];				//平焊法兰库
extern PARTLIB_API FL_PARAM fld_param_table[];				//对焊法兰库
extern PARTLIB_API FL_PARAM2 flg_param_tbl[];				//刚性法兰库
extern PARTLIB_API FL_PARAM2 flr_param_tbl[];				//柔性法兰库
extern PARTLIB_API FL_COUPLE_RULE fl_couple_rule_table[];	//法兰对接规则表
extern PARTLIB_API JG_JOINT_PARA jg_joint_table[];	//角钢接头对应表
PARTLIB_API JG_JOINT_PARA *FindJgJointPara(double main_w,double main_t);
PARTLIB_API BOOL FindInnerAngleGuiGe(double main_w,double main_t,double &inner_w,double &inner_t);
PARTLIB_API BOOL FindOuterAngleGuiGe(double main_w,double main_t,double &outer_w,double &outer_t);
PARTLIB_API BOOL FindOuterPlateGuiGe(double main_w,double main_t,double &plate_w,double &plate_t);
//－－－－－－输出变量声明－－－－－－－－－
//	 _____     |  o o  |
//	|_o_o_|	or |_o_o_o_|	——>总长
PARTLIB_API int GetLsGroupLen(int M_Diameter, int nLsN, int nRowsN);//获得螺栓组长度
PARTLIB_API BOOL GetLsSpace(LSSPACE_STRU& LsSpace, long diameter);
//查询角钢的内圆弧半径r
PARTLIB_API double FindJg_r(double wing_wide);
PARTLIB_API int getjgzj(JGZJ &jgzj, double wing_wide);//
PARTLIB_API BIAN_PARA_TYPE* FindBianType(double wide,double thick);
PARTLIB_API LS_GRADE *FindLsMatType(char *grade);
//PARTLIB_API LS_XING_HAO *FindLsGuiGe(int d, double L0,double grade=0);
//PARTLIB_API LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,double grade=0);
//ls_type012,0:单螺栓重;1:单螺栓带一垫重;2:单螺栓带双帽重
//PARTLIB_API double FindLsWei(long d,long valid_len,int ls_type012=0);
//PARTLIB_API void FindLsMinL0AndMaxL0(int ls_d, int *min_L0, int *max_L0);//查找指定直径螺栓的最小通厚以及最大通厚
PARTLIB_API void FindLsD_H(int d, double *D,double *H,double *H2);//查询螺栓的直径及螺帽高
PARTLIB_API void FindBoltCapD_H(int d, double *D,double *H,int idBoltSubPart=0);//查询螺栓的直径及螺帽高
PARTLIB_API double FindPadWei(int thick, int d);//thick表示垫圈的厚度,d表示此垫圈的附属螺栓直径(规格)
PARTLIB_API JG_PARA_TYPE* FindJgType(double wide,double thick,char cType='L');
PARTLIB_API TUBE_PARA_TYPE* FindTubeType(double D,double thick);
PARTLIB_API SLOT_PARA_TYPE* FindSlotType(char *guige);	//根据规格查找槽钢
PARTLIB_API HSHAPE_PARA_TYPE* FindHWType(double wide,double height);
PARTLIB_API HSHAPE_PARA_TYPE* FindHMType(double wide,double height);
PARTLIB_API HSHAPE_PARA_TYPE* FindHNType(double wide,double height);
PARTLIB_API HSHAPE_PARA_TYPE* FindHPType(double wide,double height);
PARTLIB_API FITTING_EB* FindEBFitting(const char* sSpec);
PARTLIB_API FITTING_GD* FindGDFitting(const char* sSpec);
PARTLIB_API FITTING_UR* FindURFitting(const char* sSPec);
//PARTLIB_API double GetJgUnitLenWei(double wing_wide,double wing_thick);//以米为长度单位
//PARTLIB_API double GetTubeUnitLenWei(double tube_d,double tube_t);//以米为长度单位	钢管单位长度重量
PARTLIB_API char QuerySteelBriefMatMark(char *steelmark);
PARTLIB_API BOOL QuerySteelMatMark(char briefmark,char* steelmark,int niMaxStrBuffLen=8);
PARTLIB_API int QuerySteelMatIndex(char briefmark);
PARTLIB_API STEELMAT *QuerySteelMatByBriefMark(char briefmark);
PARTLIB_API double CalSteelPlankWei(double volume);//输入体积单位为mm3 输出重量单位为公斤(Kg)
PARTLIB_API int QueryDiBanParaByWide(int wide,char cItem,BOOL is_weld=FALSE);//根据底脚板宽来查询底脚板的其它参数值
PARTLIB_API INSERT_PLATE_PARAM* FindRollEndParam(const char *codeName);	//查找槽型插板参数
PARTLIB_API INSERT_PLATE_PARAM* FindUEndParam(const char *codeName);		//查找U型插板参数
PARTLIB_API INSERT_PLATE_PARAM* FindXEndParam(const char *codeName);		//查找X型插板参数
PARTLIB_API INSERT_PLATE_PARAM* FindRollEndParam(double d,double t,char cTubeMat,int iLevel);	//查找槽型插板参数
PARTLIB_API INSERT_PLATE_PARAM* FindUEndParam(double d,double t,char cTubeMat,int iLevel);//查找U型插板参数
PARTLIB_API INSERT_PLATE_PARAM* FindXEndParam(double d,double t,char cTubeMat,int iLevel);//查找X型插板参数
//PARTLIB_API FL_PARAM* FindFlPParam(double d,double t,char cMaterial,char cMark);//查找平焊法兰参数
//PARTLIB_API FL_PARAM* FindFlPParam(double d,double t,char cMark);				//查找平焊法兰参数,忽略材质
PARTLIB_API FL_PARAM* FindFlPParam(const char *codeName);								//查找平焊法兰参数
//
PARTLIB_API FL_PARAM* FindFlDParam(double d,double couple_d,char cMaterial,char cMark);//查找对焊法兰参数
PARTLIB_API FL_PARAM* FindFlDParam(double d,double couple_d,char cMark);			//查找对焊法兰参数,忽略材质
PARTLIB_API FL_PARAM* FindFlDParam(const char *codeName);								//查找对焊法兰参数
//
PARTLIB_API FL_PARAM2* FindFlGParam(const char *codeName);
PARTLIB_API FL_PARAM2* FindFlGParam(double d,double couple_d,char cMat,char cMark);
PARTLIB_API FL_PARAM2* FindFlGParam(double d,double couple_d,char cMark);
//
PARTLIB_API FL_PARAM2* FindFlRParam(const char *codeName);
PARTLIB_API FL_PARAM2* FindFlRParam(double d,double couple_d,char cMat,char cMark);
PARTLIB_API FL_PARAM2* FindFlRParam(double d,double couple_d,char cMark);
//查询钢管法兰大小头配对返回:0表示未找到配对法兰;1表示主管法兰找到，但配对法兰未找到;2主管与连接管的配对全都找到
PARTLIB_API int QueryFlCouple(double fTubeD,double fOtherTubeD,char cFlType,FL_COUPLE* couple=NULL);
//
//PARTLIB_API	FL_COUPLE_RULE* QueryFlCoupleRule(double fTubeD,char cMark);		//查询法兰对接规则
//PARTLIB_API	FL_COUPLE_RULE* QueryFlCoupleRule(double fTubeD,double fOtherTubeD);//查询法兰对接规则
//PARTLIB_API	char QueryFlLevelMark(FL_COUPLE_RULE* pFlCoupleRule,double fTubeD);	//查询法兰等级标志
//PARTLIB_API double QueryCoupleTubeD(double fMainTubeD,char cMark);//根据主管规格以及法兰对接级别查找对接钢管直径
#endif