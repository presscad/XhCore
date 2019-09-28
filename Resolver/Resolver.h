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
	#define	X_RESTRICT		0x0001		//X����Լ��
#endif
#ifndef Y_RESTRICT
	#define	Y_RESTRICT		0x0002		//Y����Լ��
#endif
#ifndef	Z_RESTRICT
	#define	Z_RESTRICT		0x0004		//Z����Լ��
#endif
#ifndef X_ROT_RESTRICT
	#define	X_ROT_RESTRICT	0x0008		//X��ת��Լ��
#endif
#ifndef Y_ROT_RESTRICT
	#define	Y_ROT_RESTRICT	0x0010		//Y��ת��Լ��
#endif
#ifndef	Z_ROT_RESTRICT
	#define	Z_ROT_RESTRICT	0x0020		//Z��ת��Լ��
#endif
#ifndef ALL_RESTRICT
	#define	ALL_RESTRICT	0x003F		//������Լ��
#endif
#ifndef _ENUM_JOINT_TYPE_
#define _ENUM_JOINT_TYPE_
enum JOINT_TYPE{JOINT_HINGE=0,JOINT_RIGID=1,JOINT_AUTOJUSTIFY=2};		//������ʽ
#endif
enum COMPUTE_METHOD{GAUSS_REDUCTION,WAVE_FRONT_METHOD,SUB_BLOCK_METHOD};

//////////////////////////////////////////////////////////////////////
//����Ԫ���
//////////////////////////////////////////////////////////////////////
typedef struct PARAM6D{
	long serial;	//6ά�����ı�ʶ��ţ���ڵ��
	WORD idBlkRef;
	GEPOINT space;	//�����ϵ��������ɶȣ���λ��(displacement),��(force)
	GEPOINT extend;	//�����ϵ�������չ���ɶȣ���ת��(rotation),Ť��(moment);
}*PARAM6D_PTR;
typedef struct FEM_NODE
{
	long point_i;		//����ṹ����Ԫ����ʱ�����õĽڵ��ʶ���
	f3dPoint pos;		//�ڵ�λ������
	f3dPoint offset,pile_offset;			//�ڵ�λ��(mm)
	f3dPoint rot_offset,pile_rot_offset;	//ת��λ��
	f3dPoint init_offset,init_rot_offset;	//�ڵ���ԤӦ�������²�����ԤӦ��λ��
	f3dPoint Fxyz;				//�����(N)
	GEPOINT Moment;				//������(N*mm)
	f3dPoint internal_force;	//����Ԫ�㼯���ýڵ��ϵ�����(N)
	f3dPoint internal_moment;	//����Ԫ�㼯���ýڵ��ϵ�������(N*mm)
	f3dPoint unbalance_force;	//���ڵ�Ĳ�ƽ����
	//λ��Լ��0x0001��ʾX��λ��Լ����0x0002��ʾY��λ��Լ����0x0004��ʾZ��λ��Լ��
	//ת��Լ��0x0008��ʾX��ת��Լ����0x0010��ʾY��ת��Լ����0x0020��ʾZ��ת��Լ��
	WORD wRestrict;
	double attachMass;	//���ڸ���������kg
	long index;			//�����ţ���һ����������ʾ�ڽڵ㼯�е�����ֵ��
	short dimension;	//�ڵ����ɶ�����
	void* pTag;			//��ʱ�洢��Ϣ��
}*FEM_NODE_PTR;
struct FEM_ELEMENT;
struct IFemElement{
	//static const int JOINT_HINGE=0;
	//static const int JOINT_RIGID=1;
	//static const int JOINT_AUTOJUSTIFY=2;
	int start_joint_type,end_joint_type;	//ʼ�ն��������ͣ��½�/�սӣ�
	BYTE cbStartJoint,cbEndJoint;	//����cbStart(End)Joint!=0ʱ��start(end)_joint_typeʧЧ
	long idElem;		//��Ԫ��ʶ
	double E,G;			//����ģ��,���е���ģ��,��λΪ(MPa)��������
	double Iz,Ip;		//���Ծ�,�����Ծ�,��λΪ(mm4)��������
	double A;			//��Ԫ�����,��λΪ(mm2)��������
	double N;			//�˼�����(N),�����
	double length;		//��Ԫ���ȣ���ʱ�洢���ֲ�ʹ�ã�
	GEPOINT lenStdVec;	//��Ԫ'ʼ��>��'����λ����
	GEPOINT moment;	//����Ԫ��������ֵ(N*mm),�����
	GEPOINT start_force,end_force;	//��Ԫʼĩ�˲���������(N)
	GEPOINT start_moment,end_moment;//��Ԫʼĩ�˲�����������(N*mm)
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
	short elem_type;	//0:�����˻򾭵�����1:����
	long start_i,end_i;	//���սڵ�������
	FEM_NODE *pStart,*pEnd;
	bool bAnchorEnd;	//true:�����ն˶Ե�ê��;false:����ʼ�˶Ե�ê�̣�������
	double T0;			//ê�̶�Ԥ����(N)��������
	double density;		//�ܶ�kg/mm3��������
	void* pTag;			//��ʱ�洢��Ϣ��
public:	//�������޵�Ԫ���ຯ��
	virtual short ElemType(){return elem_type;}
	virtual GEPOINT StartPosition(){return pStart->pos;}
	virtual GEPOINT EndPosition(){return pEnd->pos;}
	virtual FEM_ELEMENT* FemElem(){return this;}
}*FEM_ELEMENT_PTR;
typedef struct UNSTABLE_NODE
{
	FEM_NODE *pFemNode;			//���ȶ��Ľڵ�
	f3dPoint unstableVector1,unstableVector2;	//�ڵ�Ĳ��ȶ�����
}UNSTABLE_NODE_PTR;
typedef struct VIBRAMODE_PROP
{
	double freqs;	//����Ƶ��
	double period;	//��������(s)
	double gama_x;	//X��ˮƽλ��ʱ���Ͳ�Ԥϵ����x(1/m)
	double gama_y;	//Y��ˮƽλ��ʱ���Ͳ�Ԥϵ����y(1/m)
	long   num;		//��Ч�����ڵ���������x_arr,y_arr��Ԫ����Ŀ
	double *x_arr;	//�ڵ�X��������ˮƽλ��
	double *y_arr;	//�ڵ�Y��������ˮƽλ��
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
	long pointI;		//����ṹ����Ԫ����ʱ�����õĽڵ��ʶ���
	short dimension;	//���ɶ�
	GEPOINT offset;		//ƽ��λ��
	GEPOINT rot_offset;	//ת��λ�ƣ�dimension=6ʱ��Ч
	GEPOINT internal_force;	//����Ԫ�㼯���ýڵ��ϵ�����(N)
	GEPOINT internal_moment;	//����Ԫ�㼯���ýڵ��ϵ�������(N*mm)
	GEPOINT uf;			//��ƽ����(N)
}*RESULT_FEMNODE_PTR;
typedef struct RESULT_FEMELEM
{
	long id;
	long hStartI,hEndI;		//��Ԫ��ʼ����ֹ�ڵ��ʶ��
	double N;
	GEPOINT start_force,end_force;	//��Ԫʼĩ�˲���������(N)
	GEPOINT moment;					//����Ԫ��������ֵ(N*mm)
	GEPOINT start_moment,end_moment;//��Ԫʼĩ�˲�����������(N*mm)
}*RESULT_FEMELEM_PTR;
//�ӽṹ������Ԫ���
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
	//����ӽṹ������Ԫ���������
	virtual void Clear()=0;
	//��ǰ�����������ӽṹ���
	virtual ISubBlock* AddSubBlock(ATOM_LIST<FEM_ELEMENT>* pElemList)=0;
	//���ݱ�ʶ���кŻ�ȡ��ǰ������е��ӽṹ��
	virtual ISubBlock* GetSubBlock(long serial)=0;

	//��յ�ǰ�ṹ����ӽṹ����Ԫ���������
	virtual void ResetStructure()=0;
	//������ṹ���еĵ�Ԫ�ͽڵ�
	virtual void ClearBodyElems()=0;
	//��ǰ�ṹ������ӽṹ���
	virtual long AddSubBlockRef(long hBlockSerial,const UCS_STRU& acs)=0;
	//��ǰ�ṹ����ӵ�Ԫ���
	virtual BOOL AddElem(const FEM_ELEMENT& fem_elem)=0;
	//��ǰ�ṹ����ӵ�Ԫ���
	virtual int AddElems(ATOM_LIST<FEM_ELEMENT>* pElemList)=0;
	//�Ե�ǰ�ṹ����к��ؼ���
	virtual bool ApplyNodeLoad(DWORD dwCaseId=0)=0;
	//��յ�ǰ�ӽṹ���ϵ����к��غɾ����Ա����¼���
	virtual void ClearNodeLoadBuffer()=0;
	//�Ե�ǰ�ṹ�������������Ԫ����,���ؽṹ����ɵĽڵ���Ŀ,����ʧ�ܷ���0����
	virtual void ConnectFemNodeAndElement(void* pContext)=0;
	virtual bool LinearResolve()=0;
};
//////////////////////////////////////////////////////////////////////
//�����������
//////////////////////////////////////////////////////////////////////
struct RESOLVER_API CXuanLianResolver{
	static CXuanLianResolver* CreateResolver();
	static CXuanLianResolver* FromSerial(long serial);
	static BOOL Destroy(long serial);
public:
	virtual long GetSerial()=0;
	// �����������Ϊ���ߵ���ģ���������¶��쳤ϵ������ʼˮƽӦ������ʼ���أ���ʼ�¶ȣ�ˮƽ���࣬�߲�ϵ��(��˼�ǰ�˸߶�/ˮƽ���)
	//equation=0:��ʾʹ��������״̬����ʽ;equation=1:��ʾʹ��б������״̬����ʽ
	virtual BOOL InitXuanLian(double E,double alfa,double sigema,double gama,double temperature,double L,double H_to_L,int equation=0)=0;
	// ָ����ǰ�����ߵ��¶Ⱥͱ��أ�gama��N/m��m2�����೤�仯�͸߲�仯,m
	virtual BOOL XuanLianByCursorPara(double gama,double temperature,double deltaL=0,double deltaH=0)=0;
	// �����ߵĳ��ȣ�ָ���¶Ⱥͱ����£�
	virtual double XuanLianLength()=0;
	// �����ߵ����쳤�ȣ�ָ���¶��£�
	virtual double XuanLianManufactureLength(double temperature)=0;
	// ���ص�ǰ������ˮƽӦ����Pa
	virtual double XuanLianHorizonStress()=0;
	// ��������xPosλ�ô�������ֵ,��������ҵ�Ϊԭ��,����ΪX������,����ΪY������
	virtual double XuanLianPosTangentCoef(double xPos)=0;
	// ��������ҵ�Ϊԭ��,����ΪX������,����ΪY������
	virtual double XuanLianPosY(double xPos)=0;
	// ��������ҵ�Ϊ����,�������ߵ���͵㻡��ֵ(����Ϊ��),m
	virtual double XuanLianPosO()=0;
};

#endif