#ifndef _EQUATION_RESOLVER_HEADER_FILE__
#define _EQUATION_RESOLVER_HEADER_FILE__
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RESOLVER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RESOLVER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef RESOLVER_EXPORTS
#define RESOLVER_API __declspec(dllexport)
#else
#define RESOLVER_API __declspec(dllimport)
#endif
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "ArrayList.h"

#ifndef X_RESTRICT
	#define	X_RESTRICT		0x0001		//X方向约束
#endif
#ifndef Y_RESTRICT
	#define	Y_RESTRICT		0x0002		//Y方向约束
#endif
#ifndef	Z_RESTRICT
	#define	Z_RESTRICT		0x0004		//Z方向约束
#endif
#ifndef X_ROT_RESTRICT
	#define	X_ROT_RESTRICT	0x0008		//X轴转动约束
#endif
#ifndef Y_ROT_RESTRICT
	#define	Y_ROT_RESTRICT	0x0010		//Y轴转动约束
#endif
#ifndef	Z_ROT_RESTRICT
	#define	Z_ROT_RESTRICT	0x0020		//Z轴转动约束
#endif
#ifndef ALL_RESTRICT
	#define	ALL_RESTRICT	0x003F		//各方向约束
#endif
#ifndef _ENUM_JOINT_TYPE_
#define _ENUM_JOINT_TYPE_
enum JOINT_TYPE{JOINT_HINGE=0,JOINT_RIGID=1,JOINT_AUTOJUSTIFY=2};		//连接形式
#endif
enum COMPUTE_METHOD{GAUSS_REDUCTION,WAVE_FRONT_METHOD,SUB_BLOCK_METHOD};

//////////////////////////////////////////////////////////////////////
//有限元求解
//////////////////////////////////////////////////////////////////////
typedef struct PARAM6D{
	long serial;	//6维参量的标识序号，如节点号
	WORD idBlkRef;
	GEPOINT space;	//狭义上的三个自由度，如位移(displacement),力(force)
	GEPOINT extend;	//广义上的三个扩展自由度，如转角(rotation),扭矩(moment);
}*PARAM6D_PTR;
typedef struct FEM_NODE
{
	long point_i;		//方便结构有限元分析时排序用的节点标识编号
	f3dPoint pos;		//节点位置坐标
	f3dPoint offset,pile_offset;			//节点位移(mm)
	f3dPoint rot_offset,pile_rot_offset;	//转动位移
	f3dPoint init_offset,init_rot_offset;	//节点在预应力作用下产生的预应力位移
	f3dPoint Fxyz;				//外荷载(N)
	GEPOINT Moment;				//外力矩(N*mm)
	f3dPoint internal_force;	//各单元汇集到该节点上的内力(N)
	f3dPoint internal_moment;	//各单元汇集到该节点上的内力矩(N*mm)
	f3dPoint unbalance_force;	//各节点的不平衡力
	//位移约束0x0001表示X向位移约束；0x0002表示Y向位移约束；0x0004表示Z向位移约束
	//转动约束0x0008表示X轴转动约束；0x0010表示Y轴转动约束；0x0020表示Z轴转动约束
	WORD wRestrict;
	double attachMass;	//外在附加质量，kg
	long index;			//索引号（是一个变量，表示在节点集中的索引值）
	short dimension;	//节点自由度数量
	void* pTag;			//临时存储信息用
}*FEM_NODE_PTR;
struct FEM_ELEMENT;
struct IFemElement{
	//static const int JOINT_HINGE=0;
	//static const int JOINT_RIGID=1;
	//static const int JOINT_AUTOJUSTIFY=2;
	int start_joint_type,end_joint_type;	//始终端连接类型（铰接/刚接）
	BYTE cbStartJoint,cbEndJoint;	//仅在cbStart(End)Joint!=0时，start(end)_joint_type失效
	long idElem;		//单元标识
	double E,G;			//弹性模量,剪切弹性模量,单位为(MPa)，输入项
	double Iz,Ip;		//惯性矩,极惯性矩,单位为(mm4)，输入项
	double A;			//单元截面积,单位为(mm2)，输入项
	double N;			//杆件内力(N),输出项
	double length;		//单元长度（临时存储，局部使用）
	GEPOINT lenStdVec;	//单元'始－>终'方向单位向量
	GEPOINT moment;	//梁单元内最大弯矩值(N*mm),输出项
	GEPOINT start_force,end_force;	//单元始末端产生的内力(N)
	GEPOINT start_moment,end_moment;//单元始末端产生的内力矩(N*mm)
public:
	IFemElement(){
		start_joint_type=end_joint_type=cbStartJoint=cbEndJoint=0;
	}
	virtual short ElemType()=0;
	virtual GEPOINT StartPosition()=0;
	virtual GEPOINT EndPosition()=0;
	virtual FEM_ELEMENT* FemElem()=0;
};
typedef struct FEM_ELEMENT : public IFemElement
{
	short elem_type;	//0:二力杆或经典梁；1:柔索
	long start_i,end_i;	//起终节点索引号
	FEM_NODE *pStart,*pEnd;
	bool bAnchorEnd;	//true:拉线终端对地锚固;false:拉线始端对地锚固，输入项
	double T0;			//锚固端预张力(N)，输入项
	double density;		//密度kg/mm3，输入项
	void* pTag;			//临时存储信息用
public:	//重载有限单元基类函数
	virtual short ElemType(){return elem_type;}
	virtual GEPOINT StartPosition(){return pStart->pos;}
	virtual GEPOINT EndPosition(){return pEnd->pos;}
	virtual FEM_ELEMENT* FemElem(){return this;}
}*FEM_ELEMENT_PTR;
typedef struct UNSTABLE_NODE
{
	FEM_NODE *pFemNode;			//不稳定的节点
	f3dPoint unstableVector1,unstableVector2;	//节点的不稳定方向
}UNSTABLE_NODE_PTR;
typedef struct VIBRAMODE_PROP
{
	double freqs;	//自振频率
	double period;	//自振周期(s)
	double gama_x;	//X向水平位移时振型参预系数γx(1/m)
	double gama_y;	//Y向水平位移时振型参预系数γy(1/m)
	long   num;		//等效质量节点数量，即x_arr,y_arr的元素数目
	double *x_arr;	//节点X方向振型水平位移
	double *y_arr;	//节点Y方向振型水平位移
}*VIBRAMODE_PROP_PTR;
struct RESOLVER_API CFemResolver{
	static CFemResolver* CreateResolver();
	static CFemResolver* FromHandle(long h);
	static BOOL Destroy(long h);
public:
	struct PX_INFO{
		int nLoadCase;
		double **P;
		double **X;
		PX_INFO(double** ppP,double** ppX,int nCase){
			nLoadCase=nCase;
			P=ppP;
			X=ppX;
		}
	};
	virtual long GetHandle()=0;
	virtual void SetProcessMonitor(void (*monitor_func)(int percent,const char *proc_description))=0;
	virtual BOOL truss_resolve(ATOM_LIST<FEM_NODE>* pNodeList,ATOM_LIST<FEM_ELEMENT>* pElemList,COMPUTE_METHOD method,bool bNonlinear,long nLoadStep=1,long nIteration=5,double fConvergeLim=5)=0;
	virtual long FreedomSize()=0;
	virtual UINT SetBuffSize(UINT uBufSize){return 0;}
	virtual UINT GetBuffSize(UINT uBufSize){return 0;}
	virtual BOOL truss_equation_recur(ATOM_LIST<FEM_NODE>* pNodeList,ATOM_LIST<FEM_ELEMENT>* pElemList,PX_INFO* px=NULL)=0;
	virtual BOOL truss_freqs(ATOM_LIST<FEM_NODE>* pNodeList,ATOM_LIST<FEM_ELEMENT>* pElemList,
		BOOL bCompatibleMassMatrix,VIBRAMODE_PROP modes_arr[],double eigen_vv[],long nFreqsRank,
		long nIteration,double epsilon)=0;
	virtual int  truss_stable_check(ATOM_LIST<FEM_NODE>* pNodeList,ATOM_LIST<FEM_ELEMENT>* pElemList,UNSTABLE_NODE* unstableNodeArr)=0;
};
typedef struct RESULT_FEMNODE
{
	long pointI;		//方便结构有限元分析时排序用的节点标识编号
	short dimension;	//自由度
	GEPOINT offset;		//平移位移
	GEPOINT rot_offset;	//转动位移，dimension=6时有效
	GEPOINT internal_force;	//各单元汇集到该节点上的内力(N)
	GEPOINT internal_moment;	//各单元汇集到该节点上的内力矩(N*mm)
	GEPOINT uf;			//不平衡力(N)
}*RESULT_FEMNODE_PTR;
typedef struct RESULT_FEMELEM
{
	long id;
	long hStartI,hEndI;		//单元起始及终止节点标识号
	double N;
	GEPOINT start_force,end_force;	//单元始末端产生的内力(N)
	GEPOINT moment;					//梁单元内最大弯矩值(N*mm)
	GEPOINT start_moment,end_moment;//单元始末端产生的内力矩(N*mm)
}*RESULT_FEMELEM_PTR;
//子结构法有限元求解
struct RESOLVER_API ISubBlockRef
{
	virtual long GetSerial()=0;
	virtual long SetMarkName(char* name)=0;
	virtual char* MarkName(char* name)=0;
	virtual ISubBlockRef* GetFirstSubBlockRef()=0;
	virtual ISubBlockRef* GetNextSubBlockRef()=0;
	virtual GEPOINT ToGlobal(GEPOINT& v,bool isVector=true)=0;
	virtual GEPOINT ToLocal(GEPOINT& v,bool isVector=true)=0;
};
struct RESOLVER_API ISubBlock
{
	virtual long GetSerial()=0;
	virtual long AddSubBlockRef(ISubBlock *pSubBlock, const UCS_STRU& acs)=0;
	virtual BOOL AddElem(const FEM_ELEMENT& fem_elem)=0;
	virtual int AddElems(ATOM_LIST<FEM_ELEMENT>* pElemList)=0;
	virtual bool SetBoundaryNode(long pointI)=0;
};

struct RESOLVER_API CBlockResolver{
	static CBlockResolver* CreateResolver();
	static CBlockResolver* FromSerial(long serial);
	static BOOL Destroy(long serial);
private:
	FEM_NODE_PTR (*ConnectFemNodeFunc)(void* pContext,long pointI);
	FEM_ELEMENT_PTR (*ConnectFemElementFunc)(void* pContext,long idElem);
public:
	FEM_NODE_PTR ConnectFemNode(void* pContext,long pointI);
	FEM_ELEMENT_PTR ConnectFemElement(void* pContext,long idElem);
	void SetConnectFemNodeFunc(FEM_NODE_PTR (*func)(void* pContext,long pointI)){ConnectFemNodeFunc = func;}
	void SetConnectFemElementFunc(FEM_ELEMENT_PTR (*func)(void* pContext,long idElem)){ConnectFemElementFunc = func;}
	virtual long GetSerial()=0;
	//清空子结构法有限元求解器缓存
	virtual void Clear()=0;
	//向当前求解器中添加子结构组成
	virtual ISubBlock* AddSubBlock(ATOM_LIST<FEM_ELEMENT>* pElemList)=0;
	//根据标识序列号获取当前求解器中的子结构块
	virtual ISubBlock* GetSubBlock(long serial)=0;

	//清空当前结构体的子结构及单元的组成数据
	virtual void ResetStructure()=0;
	//清空主结构体中的单元和节点
	virtual void ClearBodyElems()=0;
	//向当前结构体添加子结构组成
	virtual long AddSubBlockRef(long hBlockSerial,const UCS_STRU& acs)=0;
	//向当前结构体添加单元组成
	virtual BOOL AddElem(const FEM_ELEMENT& fem_elem)=0;
	//向当前结构体添加单元组成
	virtual int AddElems(ATOM_LIST<FEM_ELEMENT>* pElemList)=0;
	//对当前结构体进行荷载加载
	virtual bool ApplyNodeLoad(DWORD dwCaseId=0)=0;
	//清空当前子结构块上的所有荷载荷矩阵，以便重新加载
	virtual void ClearNodeLoadBuffer()=0;
	//对当前结构体进行线性有限元分析,返回结构体组成的节点数目,计算失败返回0或负数
	virtual void ConnectFemNodeAndElement(void* pContext)=0;
	virtual bool LinearResolve()=0;
};
//////////////////////////////////////////////////////////////////////
//悬链线求解器
//////////////////////////////////////////////////////////////////////
struct RESOLVER_API CXuanLianResolver{
	static CXuanLianResolver* CreateResolver();
	static CXuanLianResolver* FromSerial(long serial);
	static BOOL Destroy(long serial);
public:
	virtual long GetSerial()=0;
	// 输入参数依次为电线弹性模量，电线温度伸长系数，初始水平应力，初始比载，初始温度，水平档距，高差系数(后杆减前杆高度/水平间距)
	//equation=0:表示使用悬链线状态方程式;equation=1:表示使用斜抛物线状态方程式
	virtual BOOL InitXuanLian(double E,double alfa,double sigema,double gama,double temperature,double L,double H_to_L,int equation=0)=0;
	// 指定当前悬链线的温度和比载，gama，N/m·m2，档距长变化和高差变化,m
	virtual BOOL XuanLianByCursorPara(double gama,double temperature,double deltaL=0,double deltaH=0)=0;
	// 悬链线的长度（指定温度和比载下）
	virtual double XuanLianLength()=0;
	// 悬链线的制造长度（指定温度下）
	virtual double XuanLianManufactureLength(double temperature)=0;
	// 返回当前悬链线水平应力，Pa
	virtual double XuanLianHorizonStress()=0;
	// 悬链线在xPos位置处的正切值,以左侧悬挂点为原点,向右为X轴正向,向上为Y轴正向
	virtual double XuanLianPosTangentCoef(double xPos)=0;
	// 以左侧悬挂点为原点,向右为X轴正向,向上为Y轴正向
	virtual double XuanLianPosY(double xPos)=0;
	// 以左侧悬挂点为基点,此悬链线的最低点弧垂值(向下为正),m
	virtual double XuanLianPosO()=0;
};

#endif